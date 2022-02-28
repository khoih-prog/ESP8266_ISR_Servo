/****************************************************************************************************************************
  ESP8266_ISR_Servo_Impl.h
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

#ifndef ESP8266_ISR_SERVO_IMPL_H
#define ESP8266_ISR_SERVO_IMPL_H

#include <string.h>

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG      1
#endif

static ESP8266_ISR_Servo ISR_Servo;  // create servo object to control up to 16 servos

void IRAM_ATTR ESP8266_ISR_Servo_Handler()
{
  ISR_Servo.run();
}

ESP8266_ISR_Servo::ESP8266_ISR_Servo()
  : numServos (-1)
{
}

void ESP8266_ISR_Servo::init()
{
  // Interval in microsecs
  if ( ITimer.attachInterruptInterval(TIMER_INTERVAL_MICRO, (timer_callback) ESP8266_ISR_Servo_Handler ) )
  {
    ISR_SERVO_LOGERROR("Starting  ITimer OK");
  }
  else
  {
    // Can't set ITimer correctly. Select another freq. or interval
    ISR_SERVO_LOGERROR("Fail setup ESP8266_ITimer"); 
  }

  for (int8_t servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    memset((void*) &servo[servoIndex], 0, sizeof (servo_t));
    servo[servoIndex].count    = 0;
    servo[servoIndex].enabled  = false;
    // Intentional bad pin, good only from 0-16 for Digital, A0 = 17
    servo[servoIndex].pin      = ESP8266_WRONG_PIN;
  }

  numServos   = 0;

  // Init timerCount
  timerCount  = 1;
}


void IRAM_ATTR ESP8266_ISR_Servo::run()
{
  static int servoIndex;

  for (servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    if ( servo[servoIndex].enabled  && (servo[servoIndex].pin <= ESP8266_MAX_PIN) )
    {
      if ( timerCount == servo[servoIndex].count )
      {
        // PWM to LOW, will be HIGH again when timerCount = 1
        digitalWrite(servo[servoIndex].pin, LOW);
      }
      else if (timerCount == 1)
      {
        // PWM to HIGH, will be LOW again when timerCount = servo[servoIndex].count
        digitalWrite(servo[servoIndex].pin, HIGH);
      }
    }
  }

  // Reset when reaching 20000us / 10us = 2000
  if (timerCount++ >= REFRESH_INTERVAL / TIMER_INTERVAL_MICRO)
  {
    ISR_SERVO_LOGDEBUG("Reset count");

    timerCount = 1;
  }
}


// find the first available slot
// return -1 if none found
int8_t ESP8266_ISR_Servo::findFirstFreeSlot()
{
  // all slots are used
  if (numServos >= MAX_SERVOS)
    return -1;

  // return the first slot with no count (i.e. free)
  for (int8_t servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    if (servo[servoIndex].enabled == false)
    {
      ISR_SERVO_LOGDEBUG1("Index =", servoIndex);

      return servoIndex;
    }
  }

  // no free slots found
  return -1;
}


int8_t ESP8266_ISR_Servo::setupServo(const uint8_t& pin, const uint16_t& min, const uint16_t& max)
{
  int servoIndex;

  if (pin > ESP8266_MAX_PIN)
    return -1;

  if (numServos < 0)
    init();

  servoIndex = findFirstFreeSlot();

  if (servoIndex < 0)
    return -1;

  servo[servoIndex].pin        = pin;
  servo[servoIndex].min        = min;
  servo[servoIndex].max        = max;
  servo[servoIndex].count      = min / TIMER_INTERVAL_MICRO;
  servo[servoIndex].position   = 0;
  servo[servoIndex].enabled    = true;

  pinMode(pin, OUTPUT);

  numServos++;
  
  ISR_SERVO_LOGDEBUG3("Index =", servoIndex, ", count =", servo[servoIndex].count);
  ISR_SERVO_LOGDEBUG3("min =", servo[servoIndex].min, ", max =", servo[servoIndex].max);

  return servoIndex;
}

bool ESP8266_ISR_Servo::setPosition(const uint8_t& servoIndex, const uint16_t& position)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP8266_MAX_PIN) )
  {
    servo[servoIndex].position  = position;
    servo[servoIndex].count     = map(position, 0, 180, servo[servoIndex].min, servo[servoIndex].max) / TIMER_INTERVAL_MICRO;

    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);
    
    return true;
  }

  // false return for non-used numServo or bad pin
  return false;
}

// returns last position in degrees if success, or -1 on wrong servoIndex
int ESP8266_ISR_Servo::getPosition(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return -1;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP8266_MAX_PIN) )
  {
    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return (servo[servoIndex].position);
  }

  // return 0 for non-used numServo or bad pin
  return -1;
}


// setPulseWidth will set servo PWM Pulse Width in microseconds, corresponding to certain position in degrees
// by using PWM, turn HIGH 'pulseWidth' microseconds within REFRESH_INTERVAL (20000us)
// min and max for each individual servo are enforced
// returns true on success or -1 on wrong servoIndex
bool ESP8266_ISR_Servo::setPulseWidth(const uint8_t& servoIndex, uint16_t& pulseWidth)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP8266_MAX_PIN) )
  {
    if (pulseWidth < servo[servoIndex].min)
      pulseWidth = servo[servoIndex].min;
    else if (pulseWidth > servo[servoIndex].max)
      pulseWidth = servo[servoIndex].max;

    servo[servoIndex].count     = pulseWidth / TIMER_INTERVAL_MICRO;
    servo[servoIndex].position  = map(pulseWidth, servo[servoIndex].min, servo[servoIndex].max, 0, 180);

    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return true;
  }

  // false return for non-used numServo or bad pin
  return false;
}

// returns pulseWidth in microsecs (within min/max range) if success, or 0 on wrong servoIndex
unsigned int ESP8266_ISR_Servo::getPulseWidth(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return 0;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP8266_MAX_PIN) )
  {
    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return (servo[servoIndex].count * TIMER_INTERVAL_MICRO );
  }

  // return 0 for non-used numServo or bad pin
  return 0;
}


void ESP8266_ISR_Servo::deleteServo(const uint8_t& servoIndex)
{
  if ( (numServos == 0) || (servoIndex >= MAX_SERVOS) )
  {
    return;
  }

  // don't decrease the number of servos if the specified slot is already empty
  if (servo[servoIndex].enabled)
  {
    memset((void*) &servo[servoIndex], 0, sizeof (servo_t));

    servo[servoIndex].enabled   = false;
    servo[servoIndex].position  = 0;
    servo[servoIndex].count     = 0;
    // Intentional bad pin, good only from 0-16 for Digital, A0=17
    servo[servoIndex].pin       = ESP8266_WRONG_PIN;

    // update number of servos
    numServos--;
  }
}

bool ESP8266_ISR_Servo::isEnabled(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  if (servo[servoIndex].pin > ESP8266_MAX_PIN)
  {
    // Disable if something wrong
    servo[servoIndex].pin     = ESP8266_WRONG_PIN;
    servo[servoIndex].enabled = false;
    return false;
  }

  return servo[servoIndex].enabled;
}


bool ESP8266_ISR_Servo::enable(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  if (servo[servoIndex].pin > ESP8266_MAX_PIN)
  {
    // Disable if something wrong
    servo[servoIndex].pin     = ESP8266_WRONG_PIN;
    servo[servoIndex].enabled = false;
    return false;
  }

  // Bug fix. See "Fixed count >= min comparison for servo enable."
  // (https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
  if ( servo[servoIndex].count >= servo[servoIndex].min / TIMER_INTERVAL_MICRO )
    servo[servoIndex].enabled = true;

  return true;
}


bool ESP8266_ISR_Servo::disable(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  if (servo[servoIndex].pin > ESP8266_MAX_PIN)
    servo[servoIndex].pin     = ESP8266_WRONG_PIN;

  servo[servoIndex].enabled = false;

  return true;
}

void ESP8266_ISR_Servo::enableAll()
{
  // Enable all servos with a enabled and count != 0 (has PWM) and good pin
  for (int8_t servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    // Bug fix. See "Fixed count >= min comparison for servo enable."
    // (https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
    if ( (servo[servoIndex].count >= servo[servoIndex].min / TIMER_INTERVAL_MICRO ) && !servo[servoIndex].enabled 
      && (servo[servoIndex].pin <= ESP8266_MAX_PIN) )
    {
      servo[servoIndex].enabled = true;
    }
  }
}

void ESP8266_ISR_Servo::disableAll()
{
  // Disable all servos
  for (int8_t servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    servo[servoIndex].enabled = false;
  }
}

bool ESP8266_ISR_Servo::toggle(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  servo[servoIndex].enabled = !servo[servoIndex].enabled;

  return true;
}


int8_t ESP8266_ISR_Servo::getNumServos()
{
  return numServos;
}

#endif    // ESP8266_ISR_SERVO_IMPL_H
