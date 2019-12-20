/****************************************************************************************************************************
 * examples/ESP8266_BlynkServoControl.ino
 * For ESP8266 boards
 * Written by Khoi Hoang
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP8266_ISR_Servo
 * Licensed under MIT license
 * Version: v1.0.2
 * 
 * The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
 * The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
 * The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
 * Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!
 * 
 * Now these new 16 ISR-based PWM servo contro uses only 1 hardware timer.
 * The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
 * Therefore, their executions are not blocked by bad-behaving functions / tasks.
 * This important feature is absolutely necessary for mission-critical tasks. 
 * 
 * Notes:
 * Special design is necessary to share data between interrupt code and the rest of your program.
 * Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume 
 * variable can not spontaneously change. Because your function may change variables while your program is using them, 
 * the compiler needs this hint. But volatile alone is often not enough.
 * When accessing shared variables, usually interrupts must be disabled. Even with volatile, 
 * if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly. 
 * If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled 
 * or the entire sequence of your code which accesses the data.
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      04/12/2019 Initial release
 *  1.0.1   K Hoang      13/12/2019 Add more features getPosition and getPulseWidth. Optimize.
 *  1.0.2   K Hoang      20/12/2019 Add more Blynk examples.Change example names to avoid duplication. 
 *****************************************************************************************************************************/

/****************************************************************************************************************************
 * This example will demonstrate the nearly perfect accuracy compared to software timers by printing the actual elapsed millisecs.
 * Being ISR-based timers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet
 * and Blynk services. You can also have many (up to 16) timers to use.
 * This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
 * You'll see blynkTimer is blocked while connecting to WiFi / Internet / Blynk, and elapsed time is very unaccurate
 * In this super simple example, you don't see much different after Blynk is connected, because of no competing task is
 * written
 * 
 * From ESP32 Servo Example Using Arduino ESP32 Servo Library
 * John K. Bennett
 * March, 2017
 * 
 * Different servos require different pulse widths to vary servo angle, but the range is 
 * an approximately 500-2500 microsecond pulse every 20ms (50Hz). In general, hobbyist servos
 * sweep 180 degrees, so the lowest number in the published range for a particular servo
 * represents an angle of 0 degrees, the middle of the range represents 90 degrees, and the top
 * of the range represents 180 degrees. So for example, if the range is 1000us to 2000us,
 * 1000us would equal an angle of 0, 1500us would equal 90 degrees, and 2000us would equal 1800
 * degrees.
 * 
 * Circuit:
 * Servo motors have three wires: power, ground, and signal. The power wire is typically red,
 * the ground wire is typically black or brown, and the signal wire is typically yellow,
 * orange or white. Since the ESP32 can supply limited current at only 3.3V, and servos draw
 * considerable power, we will connect servo power to the VBat pin of the ESP32 (located
 * near the USB connector). THIS IS ONLY APPROPRIATE FOR SMALL SERVOS. 
 * 
 * We could also connect servo power to a separate external
 * power source (as long as we connect all of the grounds (ESP32, servo, and external power).
 * In this example, we just connect ESP32 ground to servo ground. The servo signal pins
 * connect to any available GPIO pins on the ESP32 (in this example, we use pins
 * 22, 19, 23, & 18).
 * 
 * In this example, we assume four Tower Pro SG90 small servos.
 * The published min and max for this servo are 500 and 2400, respectively.
 * These values actually drive the servos a little past 0 and 180, so
 * if you are particular, adjust the min and max values to match your needs.
 * Experimentally, 550 and 2350 are pretty close to 0 and 180.* 
*****************************************************************************************************************************/

#ifndef ESP8266
#error This code is designed to run on ESP8266 platform, not Arduino nor ESP32! Please check your Tools->Board setting.
#endif

#define BLYNK_PRINT Serial

//See file .../hardware/espressif/esp32/variants/(esp32|doitESP32devkitV1)/pins_arduino.h
#define LED_BUILTIN       2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED
//PIN_D0 can't be used for PWM/I2C
#define PIN_D0            16        // Pin D0 mapped to pin GPIO16/USER/WAKE of ESP8266. This pin is also used for Onboard-Blue LED. PIN_D0 = 0 => LED ON
#define PIN_D1            5         // Pin D1 mapped to pin GPIO5 of ESP8266
#define PIN_D2            4         // Pin D2 mapped to pin GPIO4 of ESP8266
#define PIN_D3            0         // Pin D3 mapped to pin GPIO0/FLASH of ESP8266
#define PIN_D4            2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266
#define PIN_LED           2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED
#define PIN_D5            14        // Pin D5 mapped to pin GPIO14/HSCLK of ESP8266
#define PIN_D6            12        // Pin D6 mapped to pin GPIO12/HMISO of ESP8266
#define PIN_D7            13        // Pin D7 mapped to pin GPIO13/RXD2/HMOSI of ESP8266
#define PIN_D8            15        // Pin D8 mapped to pin GPIO15/TXD2/HCS of ESP8266

#define USE_SPIFFS    true
//#define USE_SPIFFS    false

#define USE_BLYNK_WM    true            // https://github.com/khoih-prog/Blynk_WM
//#define USE_BLYNK_WM    false

//LIBRARIES INCLUDED
#include <ESP8266WiFi.h>

#if USE_BLYNK_WM
  #include <BlynkSimpleEsp8266_WM.h>                    // https://github.com/khoih-prog/Blynk_WM
#else
  #include <BlynkSimpleEsp8266.h>

  //BLYNK AUTHENTICATION TOKEN
  char auth[] = "******";
  
  
  // MY WIFI CREDENTIALS
  char ssid[] = "****";
  char pass[] = "****";
  
#endif
  
#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

#include "ESP8266_ISR_Servo.h"

// MG996R servo has a running current of  500mA to 900mA @6V and a stall current of 2.5A @ 6V
// Power supply must be adequate
// Published values for SG90 servos; adjust if needed
#define MIN_MICROS      800  //544
#define MAX_MICROS      2450

int servoIndex1  = -1;
int servoIndex2  = -1;
int servoIndex3  = -1;

int servo1Pin = PIN_D6; //SERVO1 PIN
int servo2Pin = PIN_D7; //SERVO2 PIN
int servo3Pin = PIN_D8; //SERVO3 PIN
    
BlynkTimer timer;

// These are Blynk Slider or any Widget (STEP, Numeric Input, being able to output (unsigned) int value from 0-180.
// You have to map from 0-180 inside widget or in your code. Otherwise, the library will remap the input for you.
#define BLYNK_VPIN_SERVO1_CONTROL       V21
#define BLYNK_VPIN_SERVO2_CONTROL       V22
#define BLYNK_VPIN_SERVO3_CONTROL       V23

//READING FROM VIRTUAL PINS
// SERVO1
BLYNK_WRITE(BLYNK_VPIN_SERVO1_CONTROL)
{
  ISR_Servo.setPosition(servoIndex1, param.asInt());
}

//SERVO2
BLYNK_WRITE(BLYNK_VPIN_SERVO2_CONTROL)
{
  ISR_Servo.setPosition(servoIndex2, param.asInt());
}

//SERVO3
BLYNK_WRITE(BLYNK_VPIN_SERVO3_CONTROL)
{
  ISR_Servo.setPosition(servoIndex3, param.asInt());
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
  {
    if (Blynk.connected())
      Serial.print("B");        // B means connected to Blynk
    else
      Serial.print("H");        // H means connected to WiFi but no Blynk
  }
  else
    Serial.print("F");          // F means not connected to WiFi and Blynk
  
  if (num == 80) 
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0) 
  {
    Serial.print(" ");
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("\nStarting");
  
  #if USE_BLYNK_WM
    Blynk.begin();
  #else
    Blynk.begin(auth, ssid, pass);
  #endif
 
  servoIndex1 = ISR_Servo.setupServo(servo1Pin, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ISR_Servo.setupServo(servo2Pin, MIN_MICROS, MAX_MICROS);
  servoIndex3 = ISR_Servo.setupServo(servo3Pin, MIN_MICROS, MAX_MICROS);
  
  if (servoIndex1 != -1)
    Serial.println("Setup Servo1 OK");
  else
    Serial.println("Setup Servo1 failed");

  if (servoIndex2 != -1)
    Serial.println("Setup Servo2 OK");
  else
    Serial.println("Setup Servo2 failed");    

  if (servoIndex3 != -1)
    Serial.println("Setup Servo3 OK");
  else
    Serial.println("Setup Servo3 failed");

  timer.setInterval(30000L, heartBeatPrint);     
}

void loop()
{
  Blynk.run();
  timer.run();
}
