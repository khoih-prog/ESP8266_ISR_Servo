/****************************************************************************************************************************
  ESP8266_ISR_Servo.hpp
  For ESP8266 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ESP8266_ISR_Servo
  Licensed under MIT license

  The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
  The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
  The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
  Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!

  Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long miliseconds)
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Loosely based on SimpleTimer - A timer library for Arduino.
  Author: mromani@ottotecnica.com
  Copyright (c) 2010 OTTOTECNICA Italy

  Based on BlynkTimer.h
  Author: Volodymyr Shymanskyy
  
  The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
  The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
  The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
  Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!

  Version: 1.3.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      04/12/2019 Initial coding
  1.0.1   K Hoang      05/12/2019 Add more features getPosition and getPulseWidth. Optimize.
  1.0.2   K Hoang      20/12/2019 Add more Blynk examples.Change example names to avoid duplication.
  1.1.0   K Hoang      03/01/2021 Fix bug. Add TOC and Version String.
  1.2.0   K Hoang      18/05/2021 Update to match new ESP8266 core v3.0.0
  1.3.0   K Hoang      28/02/2022 Convert to `h-only` style. Optimize code by using passing by `reference`
 *****************************************************************************************************************************/

#pragma once

#ifndef ESP8266_ISR_SERVO_HPP
#define ESP8266_ISR_SERVO_HPP

#ifndef ESP8266
  #error This code is designed to run on ESP8266 platform! Please check your Tools->Board setting.
#endif

#if !defined(ESP8266_ISR_SERVO_VERSION)
  #define ESP8266_ISR_SERVO_VERSION             "ESP8266_ISR_Servo v1.3.0"
  
  #define ESP8266_ISR_SERVO_VERSION_MAJOR       1
  #define ESP8266_ISR_SERVO_VERSION_MINOR       3
  #define ESP8266_ISR_SERVO_VERSION_PATCH       0

  #define ESP8266_ISR_SERVO_VERSION_INT         1003000
  
#endif

#include <stddef.h>

#ifdef ESP8266
  extern "C"
  {
    #include "ets_sys.h"
    #include "os_type.h"
    #include "mem.h"
  }
#else
  #include <inttypes.h>
#endif

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#include "ESP8266_ISR_Servo_Debug.h"

#include "ESP8266FastTimerInterrupt.h"

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG       0
#endif

#define ESP8266_MAX_PIN         17
#define ESP8266_WRONG_PIN       255

// From Servo.h - Copyright (c) 2009 Michael Margolis.  All right reserved.

#define MIN_PULSE_WIDTH         544       // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH         2400      // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH     1500      // default pulse width when servo is attached
#define REFRESH_INTERVAL        20000     // minumim time to refresh servos in microseconds 


class ESP8266_ISR_Servo
{

  public:
    // maximum number of servos
    const static uint8_t MAX_SERVOS = 16;

    // constructor
    ESP8266_ISR_Servo();

    // destructor
    ~ESP8266_ISR_Servo()
    {
      ITimer.detachInterrupt();
    }

    void IRAM_ATTR run();

    // Bind servo to the timer and pin, return servoIndex
    int8_t setupServo(const uint8_t& pin, const uint16_t& min = MIN_PULSE_WIDTH, const uint16_t& max = MAX_PULSE_WIDTH);

    // setPosition will set servo to position in degrees
    // by using PWM, turn HIGH 'duration' microseconds within REFRESH_INTERVAL (20000us)
    // returns true on success or -1 on wrong servoIndex
    bool setPosition(const uint8_t& servoIndex, const uint16_t& position);

    // returns last position in degrees if success, or -1 on wrong servoIndex
    int getPosition(const uint8_t& servoIndex);

    // setPulseWidth will set servo PWM Pulse Width in microseconds, corresponding to certain position in degrees
    // by using PWM, turn HIGH 'pulseWidth' microseconds within REFRESH_INTERVAL (20000us)
    // min and max for each individual servo are enforced
    // returns true on success or -1 on wrong servoIndex
    bool setPulseWidth(const uint8_t& servoIndex, uint16_t& pulseWidth);

    // returns pulseWidth in microsecs (within min/max range) if success, or 0 on wrong servoIndex
    unsigned int getPulseWidth(const uint8_t& servoIndex);

    // destroy the specified servo
    void deleteServo(const uint8_t& servoIndex);

    // returns true if the specified servo is enabled
    bool isEnabled(const uint8_t& servoIndex);

    // enables the specified servo
    bool enable(const uint8_t& servoIndex);

    // disables the specified servo
    bool disable(const uint8_t& servoIndex);

    // enables all servos
    void enableAll();

    // disables all servos
    void disableAll();

    // enables the specified servo if it's currently disabled,
    // and vice-versa
    bool toggle(const uint8_t& servoIndex);

    // returns the number of used servos
    int8_t getNumServos();

    // returns the number of available servos
    int8_t getNumAvailableServos() 
    {
      if (numServos <= 0)
        return MAX_SERVOS;
      else 
        return MAX_SERVOS - numServos;
    };

  private:

    void init();

    // find the first available slot
    int8_t findFirstFreeSlot();

    typedef struct
    {
      uint8_t       pin;                  // pin servo connected to
      unsigned long count;                // In microsecs
      uint16_t      position;             // In degrees
      bool          enabled;              // true if enabled
      uint16_t      min;
      uint16_t      max;
    } servo_t;

    volatile servo_t servo[MAX_SERVOS];

    // actual number of servos in use (-1 means uninitialized)
    volatile int8_t numServos;

    // Use 10 microsecs timer, just  fine enough to control Servo, normally requiring pulse width (PWM) 500-2000us in 20ms.
#define TIMER_INTERVAL_MICRO        10

    // timerCount starts at 1, and counting up to (REFRESH_INTERVAL / TIMER_INTERVAL_MICRO) = (20000 / 10) = 2000
    // then reset to 1. Use this to calculate when to turn ON / OFF pulse to servo
    // For example, servo1 uses pulse width 1000us => turned ON when timerCount = 1, turned OFF when timerCount = 1000 / TIMER_INTERVAL_MICRO = 100
    volatile unsigned long timerCount;

    // Init ESP32 timer 0
    ESP8266Timer ITimer;
};

//extern ESP8266_ISR_Servo ISR_Servo;  // create servo object to control up to 16 servos


#endif    // ESP8266_ISR_SERVO_HPP
