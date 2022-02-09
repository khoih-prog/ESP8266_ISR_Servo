# ESP8266_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP8266_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP8266_ISR_Servo.svg)](https://github.com/khoih-prog/ESP8266_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP8266_ISR_Servo.svg)](http://github.com/khoih-prog/ESP8266_ISR_Servo/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 50px !important;width: 181px !important;" ></a>

---
---

## Table of Contents

* [Why do we need this ESP8266_ISR_Servo library](#why-do-we-need-this-esp8266_isr_servo-library)
  * [Features](#features)
  * [Important Notes about using ISR](#important-notes-about-using-isr)
  * [Currently supported Boards](#currently-supported-boards)
* [Changelog](#changelog)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.0](#releases-v110)
  * [Releases v1.0.2](#releases-v102)
  * [Releases v1.0.1](#releases-v101)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [More useful Information](#more-useful-information)
  * [ESP8266 Hardware Timers](#esp8266-hardware-timers)
  * [New functions](#new-functions)
  * [What special in this ESP8266_ISR_Servo library](#what-special-in-this-esp8266_isr_servo-library)
* [HOWTO Usage](#howto-usage)
* [Examples](#examples)
  * [ 1. **ESP8266_BlynkServoControl**](examples/ESP8266_BlynkServoControl)
  * [ 2. ESP8266_ISR_MultiServos](examples/ESP8266_ISR_MultiServos)
  * [ 3. ESP8266_MultipleRandomServos](examples/ESP8266_MultipleRandomServos)
  * [ 4. ESP8266_MultipleServos](examples/ESP8266_MultipleServos)
  * [ 5. ISR_MultiServos](examples/ISR_MultiServos)
  * [ 6. MultipleRandomServos](examples/MultipleRandomServos)
  * [ 7. MultipleServos](examples/MultipleServos)
* [Example ESP8266_BlynkServoControl](#example-esp8266_blynkservocontrol)
  * [1. File ESP8266_BlynkServoControl.ino](#1-file-esp8266_blynkservocontrolino)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ESP8266_BlynkServoControl using LITTLEFS with SSL on ESP8266_NODEMCU_ESP12E](#1-esp8266_blynkservocontrol-using-littlefs-with-ssl-on-esp8266_nodemcu_esp12e)
  * [2. ESP8266_MultipleRandomServos on ESP8266_NODEMCU_ESP12E](#2-esp8266_multiplerandomservos-on-esp8266_nodemcu_esp12e)
  * [3. ESP8266_ISR_MultiServos on ESP8266_NODEMCU_ESP12E](#3-esp8266_isr_multiservos-on-esp8266_nodemcu_esp12e)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Releases](#releases)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Why do we need this [ESP8266_ISR_Servo library](https://github.com/khoih-prog/ESP8266_ISR_Servo)

#### Features

Imagine you have a system with a **mission-critical function** controlling a **robot arm** or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is blocking the loop() or setup().

So your function might not be executed, and the result would be disastrous.

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a **Hardware Timer with Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are **much more precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the **loop() or setup() is blocked by certain operation**. For example, certain function is blocking while it's connecting to WiFi or some services.

This library enables you to use **1 Hardware Timer** on an ESP8266-based board to control up to **16 independent servo motors**.


#### Important Notes about using ISR

1. Inside the attached function, delay() won’t work and the value returned by millis() will not increment. Serial data received while in the function may be lost. You should declare as volatile any variables that you modify within the attached function.

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

3. Avoid using Serial.print()-related functions inside ISR. Just for temporary debug purpose, but even this also can crash the system any time. Beware.

4. Your functions are now part of **ISR (Interrupt Service Routine)**, and must be `lean / mean`, and follow certain rules. More to read on:

[HOWTO Attach Interrupt](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)


#### Currently supported Boards

This [**ESP8266_ISR_Servo** library](https://github.com/khoih-prog/ESP8266_ISR_Servo) currently supports these following boards:

 1. **ESP8266-based boards**.

---
---

## Changelog

### Releases v1.2.0

1. Update to match new ESP8266 core v3.0.0

### Releases v1.1.0

1. Fix bug. See [Fixed count >= min comparison for servo enable](https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
2. Clean-up all compiler warnings possible.
3. Add Table of Contents
4. Add Version String
5. Fix and Optimize old examples

### Releases v1.0.2

1. Add example using [**Blynk**](http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Releases v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP8266.


---
---

## Prerequisites

1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
2. [`ESP8266 Core 3.0.2+`](https://github.com/esp8266/Arduino) for ESP8266-based boards. [![Latest release](https://img.shields.io/github/release/esp8266/Arduino.svg)](https://github.com/esp8266/Arduino/releases/latest/). To use ESP8266 core 2.7.1+ for LittleFS. 
3. [`Blynk_WM library v1.6.2+`](https://github.com/khoih-prog/Blynk_WM) to use with some examples. To install. check [![arduino-library-badge](https://www.ardu-badge.com/badge/Blynk_WiFiManager.svg?)](https://www.ardu-badge.com/Blynk_WiFiManager)
4. [`ESP_DoubleResetDetector library v1.2.1+`](https://github.com/khoih-prog/ESP_DoubleResetDetector) to use with some examples. To install. check [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_DoubleResetDetector.svg?)](https://www.ardu-badge.com/ESP_DoubleResetDetector)

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

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**ESP8266_ISR_Servo** library](https://platformio.org/lib/show/6920/ESP8266_ISR_Servo) by using [Library Manager](https://platformio.org/lib/show/6920/ESP8266_ISR_Servo/installation). Search for **ESP8266_ISR_Servo** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

## More useful Information

### ESP8266 Hardware Timers

The **ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler**. They're only better than UNO / Mega.

The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.

The timer1's 23-bit counter can terribly count only up to 8,388,607. So the timer1 maximum interval is very short.

Using 256 prescaler, **maximum timer1 interval is only 26.843542 seconds !!!**

The timer1 counters can be configured to support automatic reload.

---

### New functions

```
// returns last position in degrees if success, or -1 on wrong servoIndex
int getPosition(unsigned servoIndex);

// returns pulseWidth in microsecs (within min/max range) if success, or 0 on wrong servoIndex
unsigned int getPulseWidth(unsigned servoIndex);
```

### What special in this [ESP8266_ISR_Servo library](https://github.com/khoih-prog/ESP8266_ISR_Servo)

Now these new `16 ISR-based Servo controllers` just use one ESP8266 Hardware Timer. The number 16 is just arbitrarily chosen, and depending on application, you can increase that number to 32, 48, etc. without problem.

The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers

Therefore, their executions are not blocked by bad-behaving functions / tasks. This important feature is absolutely necessary for mission-critical tasks. 

The `MultipleServos` example, which controls 6 servos independently, will demonstrate the nearly perfect accuracy.
Being ISR-based servo controllers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---
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
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting ESP8266_ISR_MultiServos on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP8266_ISR_SERVO_VERSION);
  
  servoIndex1 = ISR_Servo.setupServo(D8, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ISR_Servo.setupServo(D7, MIN_MICROS, MAX_MICROS);

  if (servoIndex1 != -1)
    Serial.println(F("Setup Servo1 OK"));
  else
    Serial.println(F("Setup Servo1 failed"));

  if (servoIndex2 != -1)
    Serial.println(F("Setup Servo2 OK"));
  else
    Serial.println(F("Setup Servo2 failed"));
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

      if (position % 30 == 0)
      {
        Serial.print(F("Servo1 pos = ")); Serial.print(position);
        Serial.print(F(", Servo2 pos = ")); Serial.println(180 - position);
      }
      
      ISR_Servo.setPosition(servoIndex1, position);
      ISR_Servo.setPosition(servoIndex2, 180 - position);
      // waits 15ms for the servo to reach the position
      delay(50  /*15*/);
    }

    delay(5000);

    for (position = 180; position >= 0; position--)
    {
      // goes from 180 degrees to 0 degrees
      ISR_Servo.setPosition(servoIndex1, position);
      ISR_Servo.setPosition(servoIndex2, 180 - position);
      // waits 15ms for the servo to reach the position
      delay(50  /*15*/);
    }

    delay(5000);
  }
}

```

---
---

### Examples: 

 1. [**ESP8266_BlynkServoContro**](examples/ESP8266_BlynkServoControl)
 2. [ESP8266_ISR_MultiServos](examples/ESP8266_ISR_MultiServos)
 3. [ESP8266_MultipleRandomServos](examples/ESP8266_MultipleRandomServos) 
 4. [ESP8266_MultipleServos](examples/ESP8266_MultipleServos) 
 5. [ISR_MultiServos](examples/ISR_MultiServos)
 6. [MultipleRandomServos](examples/MultipleRandomServos)
 7. [MultipleServos](examples/MultipleServos)
 
---
---

### Example [ESP8266_BlynkServoControl](examples/ESP8266_BlynkServoControl)


#### 1. File [ESP8266_BlynkServoControl.ino](examples/ESP8266_BlynkServoControl/ESP8266_BlynkServoControl.ino)

```cpp
// Please use the default Config Portal password MyESP_XXXXXX, where ESP_XXXXXX is the Config Portal SSID

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"


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
      Serial.print(F("B"));        // B means connected to Blynk
    else
      Serial.print(F("H"));        // H means connected to WiFi but no Blynk
  }
  else
    Serial.print(F("F"));          // F means not connected to WiFi and Blynk

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);

#if (USE_LITTLEFS)
  Serial.print(F("\nStarting ESP8266_BlynkServoControl using LITTLEFS"));
#elif (USE_SPIFFS)
  Serial.print(F("\nStarting ESP8266_BlynkServoControl using SPIFFS"));
#else
  Serial.print(F("\nStarting ESP8266_BlynkServoControl using EEPROM"));
#endif

#if USE_SSL
  Serial.print(F(" with SSL on ")); Serial.println(ARDUINO_BOARD);
#else
  Serial.print(F(" without SSL on ")); Serial.println(ARDUINO_BOARD);
#endif

  Serial.println(BLYNK_WM_VERSION);
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
  Serial.println(ESP8266_ISR_SERVO_VERSION);

  Blynk.begin(HOST_NAME);

  servoIndex1 = ISR_Servo.setupServo(servo1Pin, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ISR_Servo.setupServo(servo2Pin, MIN_MICROS, MAX_MICROS);
  servoIndex3 = ISR_Servo.setupServo(servo3Pin, MIN_MICROS, MAX_MICROS);

  if (servoIndex1 != -1)
    Serial.println(F("Setup Servo1 OK"));
  else
    Serial.println(F("Setup Servo1 failed"));

  if (servoIndex2 != -1)
    Serial.println(F("Setup Servo2 OK"));
  else
    Serial.println(F("Setup Servo2 failed"));

  if (servoIndex3 != -1)
    Serial.println(F("Setup Servo3 OK"));
  else
    Serial.println(F("Setup Servo3 failed"));

  timer.setInterval(30000L, heartBeatPrint);
}

void loop()
{
  Blynk.run();
  timer.run();
}
```

---
---

### Debug Terminal Output Samples

### 1. ESP8266_BlynkServoControl using LITTLEFS with SSL on ESP8266_NODEMCU_ESP12E


```
Starting ESP8266_BlynkServoControl using LITTLEFS with SSL on ESP8266_NODEMCU_ESP12E
Blynk_WM v1.5.0
ESP_DoubleResetDetector v1.1.1
ESP8266_ISR_Servo v1.2.0
[290] Hostname=8266-Master-Controller
[309] LoadCfgFile 
[309] OK
[309] CCSum=0x3817,RCSum=0x3817
[312] LoadCredFile 
[312] OK
[312] CrCCsum=0x2670,CrRCsum=0x2670
[312] Hdr=SSL_ESP8266,BrdName=ESP8266_ISR_Servo
[313] SSID=HueNet1,PW=12345678
[316] SSID1=HueNet2,PW1=12345678
[319] Server=account.duckdns.org,Token=token1
[325] Server1=account.duckdns.org,Token1=token2
[331] Port=9443
[333] ======= End Config Data =======
[336] Connecting MultiWifi...
[6594] WiFi connected after time: 1
[6594] SSID: HueNet1, RSSI = -36
[6594] Channel: 2, IP address: 192.168.2.166
[6594] bg: WiFi OK. Try Blynk
[6596] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on ESP8266_NODEMCU_ESP12E

[22618] NTP time: Sun Jan  3 07:08:22 2021
[22619] BlynkArduinoClient.connect: Connecting to account.duckdns.org:9443
[23287] Certificate OK
[23295] Ready (ping: 2ms).
[23366] Connected to Blynk Server = account.duckdns.org, Token = token1
[23366] bg: WiFi+Blynk OK
[ISR_SERVO] ESP8266FastTimerInterrupt: _fre = 5000000.00 , _count = 50
[ISR_SERVO] Starting  ITimer OK
Setup Servo1 OK
Setup Servo2 OK
Setup Servo3 OK
```
---

### 2. ESP8266_MultipleRandomServos on ESP8266_NODEMCU_ESP12E

```
Starting ESP8266_MultipleRandomServos on ESP8266_NODEMCU_ESP12E
ESP8266_ISR_Servo v1.2.0
[ISR_SERVO] ESP8266FastTimerInterrupt: _fre = 5000000.00 , _count = 50
[ISR_SERVO] Starting  ITimer OK
Setup OK Servo index = 0
Setup OK Servo index = 1
Setup OK Servo index = 2
Setup OK Servo index = 3
Setup OK Servo index = 4
Setup OK Servo index = 5
Servos @ 0 degree
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
800
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
800
[ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
800
[ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 80 , pos = 0
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 80 , pos = 0
800
[ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 80 , pos = 0
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 80 , pos = 0
800
[ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 80 , pos = 0
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos @ 90 degree
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
1620
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
1620
[ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
1620
[ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 162 , pos = 90
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 162 , pos = 90
1620
[ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 162 , pos = 90
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 162 , pos = 90
1620
[ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 162 , pos = 90
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 162 , pos = 90
1620
```

---

### 3. ESP8266_ISR_MultiServos on ESP8266_NODEMCU_ESP12E


```
Starting ESP8266_ISR_MultiServos on ESP8266_NODEMCU_ESP12E
ESP8266_ISR_Servo v1.2.0
[ISR_SERVO] ESP8266FastTimerInterrupt: _fre = 5000000.00 , _count = 50
[ISR_SERVO] Starting  ITimer OK
Setup Servo1 OK
Setup Servo2 OK
Servo1 pos = 0, Servo2 pos = 180
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 245 , pos = 180
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 1
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 244 , pos = 179
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 81 , pos = 2
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 243 , pos = 178
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 82 , pos = 3
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 242 , pos = 177
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 83 , pos = 4
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 241 , pos = 176
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 84 , pos = 5
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 240 , pos = 175
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 85 , pos = 6
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 239 , pos = 174
[ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 86 , pos = 7
[ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 238 , pos = 173
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level from 0 to 2. Be careful and using level 2 only for temporary debug purpose only.

```cpp
#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.

---
---

## Releases

### Releases v1.2.0

1. Update to match new ESP8266 core v3.0.0

### Releases v1.1.0

1. Fix bug. See [Fixed count >= min comparison for servo enable](https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
2. Clean-up all compiler warnings possible.
3. Add Table of Contents
4. Add Version String
5. Fix and Optimize old examples

### Releases v1.0.2

1. Add example using [Blynk](http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Releases v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP8266.

---
---

### Issues ###

Submit issues to: [ESP8266_ISR_Servo issues](https://github.com/khoih-prog/ESP8266_ISR_Servo/issues)

---
---

## TO DO

1. Search for bug and improvement.


## DONE

1. Similar features for Arduino (UNO, Mega, etc...) and ESP32 
2. Add functions `getPosition()` and `getPulseWidth()`
3. Optimize the code
4. Add more complicated examples
5. Update to match new ESP8266 core v3.0.0

---
---

### Contributions and Thanks

1. Thanks to [raphweb](https://github.com/raphweb) for the PR [Fixed count >= min comparison for servo enable.](https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1) to fix bug and leading to the new releases v1.1.0

<table>
  <tr>
    <td align="center"><a href="https://github.com/raphweb"><img src="https://github.com/raphweb.png" width="100px;" alt="raphweb"/><br /><sub><b>⭐️ raphweb</b></sub></a><br /></td>
  </tr> 
</table>


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/master/LICENSE)

---

## Copyright

Copyright 2019- Khoi Hoang
