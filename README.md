# ESP8266_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP8266_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP8266_ISR_Servo.svg)](https://github.com/khoih-prog/ESP8266_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP8266_ISR_Servo.svg)](http://github.com/khoih-prog/ESP8266_ISR_Servo/issues)


---
---

### Version v1.0.2

1. Add example using [Blynk] (http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Version v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP8266.

---

This library enables you to use **1 Hardware Timer** on an ESP8266-based board to control up to **16 independent servo motors**.


#### Why do we need this ISR-based Servo controller?

Imagine you have a system with a **mission-critical function** controlling a **robot arm** or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is blocking the loop() or setup().

So your function might not be executed, and the result would be disastrous.

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a **Hardware Timer with Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are **much more precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the **loop() or setup() is blocked by certain operation**. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is your function is now part of an **ISR (Interrupt Service Routine)**, and must be `lean / mean`, and follow certain rules. More to read on:

[HOWTO Attach Interrupt](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Important Notes:

1. Inside the attached function, delay() won’t work and the value returned by millis() will not increment. Serial data received while in the function may be lost. You should declare as volatile any variables that you modify within the attached function.

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisite

1. [`Arduino IDE 1.8.12+` for Arduino](https://www.arduino.cc/en/Main/Software)
2. [`ESP8266 core 2.7.3+` for Arduino](https://github.com/esp8266/Arduino#installing-with-boards-manager) for ESP8266 boards.

---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `ESP8266_ISR_Servo`, then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP8266_ISR_Servo) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [ESP8266_ISR_Servo](https://github.com/khoih-prog/ESP8266_ISR_Servo) page.
2. Download the latest release `ESP8266_ISR_Servo-master.zip`.
3. Extract the zip file to `ESP8266_ISR_Servo-master` directory 
4. Copy whole `ESP8266_ISR_Servo-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install **ESP8266_ISR_Servo** library by using [Library Manager](https://docs.platformio.org/en/latest/librarymanager/). Search for ESP8266_ISR_Servo in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---

## More useful Information

The **ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler**. They're only better than UNO / Mega.

The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.

The timer1's 23-bit counter can terribly count only up to 8,388,607. So the timer1 maximum interval is very short.

Using 256 prescaler, **maximum timer1 interval is only 26.843542 seconds !!!**

The timer1 counters can be configured to support automatic reload.

---

## New from v1.0.1

1. Add functions `getPosition()` and `getPulseWidth()`
2. Optimize the code
3. Add more complicated example

Now these new `16 ISR-based Servo controllers` just use one ESP8266 Hardware Timer. The number 16 is just arbitrarily chosen, and depending 
on application, you can increase that number to 32, 48, etc. without problem.
The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
Therefore, their executions are not blocked by bad-behaving functions / tasks.
This important feature is absolutely necessary for mission-critical tasks. 

The `MultipleServos` example, which controls 6 servos independently, will demonstrate the nearly perfect accuracy.
Being ISR-based servo controllers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet
and Blynk services.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

## Supported Boards

- ESP8266

---

### HOWTO Usage

How to use:

```
#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

#include "ESP8266_ISR_Servo.h"

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS      800  //544
#define MAX_MICROS      2450

int servoIndex1  = -1;
int servoIndex2  = -1;

void setup() 
{
  Serial.begin(115200);
  Serial.println("\nStarting");

  servoIndex1 = ISR_Servo.setupServo(D8, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ISR_Servo.setupServo(D7, MIN_MICROS, MAX_MICROS);
  
  if (servoIndex1 != -1)
    Serial.println("Setup Servo1 OK");
  else
    Serial.println("Setup Servo1 failed");

  if (servoIndex2 != -1)
    Serial.println("Setup Servo2 OK");
  else
    Serial.println("Setup Servo2 failed");
}

void loop() 
{
  int position;

  if ( ( servoIndex1 != -1) && ( servoIndex2 != -1) )
  {
    for (position = 0; position <= 180; position++) 
    { 
      // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      ISR_Servo.setPosition(servoIndex1, position);
      ISR_Servo.setPosition(servoIndex2, 180 - position);
      // waits 50ms for the servo to reach the position
      delay(50  /*15*/);
    }
    
    for (position = 180; position >= 0; position--) 
    { 
      // goes from 180 degrees to 0 degrees
      ISR_Servo.setPosition(servoIndex1, position);
      ISR_Servo.setPosition(servoIndex2, 180 - position);
      // waits 50ms for the servo to reach the position
      delay(50  /*15*/);
    }
  }
}

```

---
---

### Examples: 

 1. [ESP8266_BlynkServoControl](examples/ESP8266_BlynkServoControl)
 2. [ESP8266_ISR_MultiServos](examples/ESP8266_ISR_MultiServos)
 3. [ESP8266_MultipleRandomServos](examples/ESP8266_MultipleRandomServos) 
 4. [ESP8266_MultipleServos](examples/ESP8266_MultipleServos) 
 5. [ISR_MultiServos](examples/ISR_MultiServos)
 6. [MultipleRandomServos](examples/MultipleRandomServos)
 7. [MultipleServos](examples/MultipleServos)
 
---

### Example [Argument_Complex](examples/Argument_Complex)

```cpp
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
```

---


## TO DO

1. Search for bug and improvement.


## DONE

1. Similar features for Arduino (UNO, Mega, etc...) and ESP32 
2. Add functions `getPosition()` and `getPulseWidth()`
3. Optimize the code
4. Add more complicated examples

---
---

### Version v1.0.2

1. Add example using [Blynk] (http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Version v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP8266.

---

### Issues ###

Submit issues to: [ESP8266_ISR_Servo issues](https://github.com/khoih-prog/ESP8266_ISR_Servo/issues)

---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

## Copyright

Copyright 2019- Khoi Hoang
