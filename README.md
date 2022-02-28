# ESP8266_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP8266_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP8266_ISR_Servo.svg)](https://github.com/khoih-prog/ESP8266_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP8266_ISR_Servo.svg)](http://github.com/khoih-prog/ESP8266_ISR_Servo/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>

---
---

## Table of Contents

* [Important Change from v1.3.0](#Important-Change-from-v130)
* [Why do we need this ESP8266_ISR_Servo library](#why-do-we-need-this-esp8266_isr_servo-library)
  * [Features](#features)
  * [Important Notes about using ISR](#important-notes-about-using-isr)
  * [Currently supported Boards](#currently-supported-boards)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [More useful Information](#more-useful-information)
  * [ESP8266 Hardware Timers](#esp8266-hardware-timers)
  * [New functions](#new-functions)
  * [What special in this ESP8266_ISR_Servo library](#what-special-in-this-esp8266_isr_servo-library)
* [HOWTO Usage](#howto-usage)
* [Examples](#examples)
  * [ 1. **multiFileProject**](examples/multiFileProject) **New**
  * [ 2. ESP8266_ISR_MultiServos](examples/ESP8266_ISR_MultiServos)
  * [ 3. ESP8266_MultipleRandomServos](examples/ESP8266_MultipleRandomServos)
  * [ 4. ESP8266_MultipleServos](examples/ESP8266_MultipleServos)
  * [ 5. ISR_MultiServos](examples/ISR_MultiServos)
  * [ 6. MultipleRandomServos](examples/MultipleRandomServos)
  * [ 7. MultipleServos](examples/MultipleServos)
* [Example ESP8266_MultipleRandomServos](#example-ESP8266_MultipleRandomServos)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ESP8266_MultipleRandomServos on ESP8266_NODEMCU_ESP12E](#1-esp8266_multiplerandomservos-on-esp8266_nodemcu_esp12e)
  * [2. ESP8266_ISR_MultiServos on ESP8266_NODEMCU_ESP12E](#2-esp8266_isr_multiservos-on-esp8266_nodemcu_esp12e)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.3.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)



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

## Prerequisites

1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
2. [`ESP8266 Core 3.0.2+`](https://github.com/esp8266/Arduino) for ESP8266-based boards. [![Latest release](https://img.shields.io/github/release/esp8266/Arduino.svg)](https://github.com/esp8266/Arduino/releases/latest/). To use ESP8266 core 2.7.1+ for LittleFS. 


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
3. Install [**ESP8266_ISR_Servo** library](https://registry.platformio.org/libraries/khoih-prog/ESP8266_ISR_Servo) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/ESP8266_ISR_Servo/installation). Search for **ESP8266_ISR_Servo** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "ESP8266_ISR_Servo.hpp"     //https://github.com/khoih-prog/ESP8266_ISR_Servo
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ESP8266_ISR_Servo.h"           //https://github.com/khoih-prog/ESP8266_ISR_Servo
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)



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

---

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

https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/9a8757117d2c901bed222990f1154b4cfa3fb7fd/examples/ESP8266_ISR_MultiServos/ESP8266_ISR_MultiServos.ino#L70-L147


---
---

### Examples: 

 1. [**multiFileProject**](examples/multiFileProject) **New**
 2. [ESP8266_ISR_MultiServos](examples/ESP8266_ISR_MultiServos)
 3. [ESP8266_MultipleRandomServos](examples/ESP8266_MultipleRandomServos) 
 4. [ESP8266_MultipleServos](examples/ESP8266_MultipleServos) 
 5. [ISR_MultiServos](examples/ISR_MultiServos)
 6. [MultipleRandomServos](examples/MultipleRandomServos)
 7. [MultipleServos](examples/MultipleServos)
 
---
---

### Example [ESP8266_MultipleRandomServos](examples/ESP8266_MultipleRandomServos)

https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/9a8757117d2c901bed222990f1154b4cfa3fb7fd/examples/ESP8266_MultipleRandomServos/ESP8266_MultipleRandomServos.ino#L61-L233


---
---

### Debug Terminal Output Samples

### 1. ESP8266_MultipleRandomServos on ESP8266_NODEMCU_ESP12E

```
Starting ESP8266_MultipleRandomServos on ESP8266_NODEMCU_ESP12E
ESP8266_ISR_Servo v1.3.0
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

### 2. ESP8266_ISR_MultiServos on ESP8266_NODEMCU_ESP12E


```
Starting ESP8266_ISR_MultiServos on ESP8266_NODEMCU_ESP12E
ESP8266_ISR_Servo v1.3.0
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
6. Convert to `h-only` style.
7. Add example [multiFileProject](examples/multiFileProject) to demo for multiple-file project
8. Optimize code by using passing by `reference` instead of by `value`


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
---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/ESP8266_ISR_Servo/blob/master/LICENSE)

---

## Copyright

Copyright 2019- Khoi Hoang
