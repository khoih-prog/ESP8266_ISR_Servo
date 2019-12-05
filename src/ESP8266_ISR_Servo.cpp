/****************************************************************************************************************************
 * ESP8266_ISR_Servo.cpp
 * For ESP8266 boards
 * Written by Khoi Hoang
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP8266_ISR_Servo
 * Licensed under MIT license
 * Version: 1.0.0
 *
 * The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
 * The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
 * The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
 * Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!
 * 
 * Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long miliseconds)
 * The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
 * Therefore, their executions are not blocked by bad-behaving functions / tasks.
 * This important feature is absolutely necessary for mission-critical tasks. 
 *
 * Loosely based on SimpleTimer - A timer library for Arduino.
 * Author: mromani@ottotecnica.com
 * Copyright (c) 2010 OTTOTECNICA Italy
 * 
 * Based on BlynkTimer.h
 * Author: Volodymyr Shymanskyy

 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      04/12/2019 Initial coding
*****************************************************************************************************************************/

#include "ESP8266_ISR_Servo.h"
#include <string.h>

#ifndef ISR_SERVO_DEBUG
#define ISR_SERVO_DEBUG      1
#endif

//extern void ICACHE_RAM_ATTR ESP8266_ISR_Servo_Handler(void);
ESP8266_ISR_Servo ISR_Servo;  // create servo object to control up to 16 servos

void ICACHE_RAM_ATTR ESP8266_ISR_Servo_Handler(void)
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
    #if (ISR_SERVO_DEBUG > 0)
      Serial.println("Starting  ITimer OK");
    #endif
  }
  else
  {
    #if (ISR_SERVO_DEBUG > 0)
      // Can't set ITimer correctly. Select another freq. or interval
      Serial.println("Fail setup ITimer");
    #endif
  }

  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++) 
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


void ICACHE_RAM_ATTR ESP8266_ISR_Servo::run() 
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
    #if (ISR_SERVO_DEBUG > 1)
      Serial.println("Reset count");
    #endif
    
    timerCount = 1;
  }
}


// find the first available slot
// return -1 if none found
int ESP8266_ISR_Servo::findFirstFreeSlot() 
{
  // all slots are used
  if (numServos >= MAX_SERVOS) 
    return -1;

  // return the first slot with no count (i.e. free)
  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++) 
  {
    if (servo[servoIndex].enabled == false) 
    {
      #if (ISR_SERVO_DEBUG > 1)
        Serial.println("Index = " + String(servoIndex));
      #endif
      
      return servoIndex;
    }
  }

  // no free slots found
  return -1;
}


int ESP8266_ISR_Servo::setupServo(uint8_t pin, int min, int max) 
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

  #if (ISR_SERVO_DEBUG > 0)
    Serial.print("Index = " + String(servoIndex) + ", count = " + String(servo[servoIndex].count));
    Serial.println(", min = " + String(servo[servoIndex].min) + ", max = " + String(servo[servoIndex].max));
  #endif

  return servoIndex;
}

bool ESP8266_ISR_Servo::setPosition(unsigned servoIndex, unsigned long position) 
{
  if (servoIndex >= MAX_SERVOS) 
    return false;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP8266_MAX_PIN) ) 
  {
    servo[servoIndex].position  = position;
    servo[servoIndex].count     = map(position, 0, 180, servo[servoIndex].min, servo[servoIndex].max) / TIMER_INTERVAL_MICRO;

    #if (ISR_SERVO_DEBUG > 0)
      Serial.println("Idx = " + String(servoIndex) + ", cnt = " + String(servo[servoIndex].count) + ", pos = " + String(servo[servoIndex].position));
    #endif

    return true;
  }
  
  // false return for non-used numServo or bad pin
  return false;
}

void ESP8266_ISR_Servo::deleteServo(unsigned servoIndex) 
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

bool ESP8266_ISR_Servo::isEnabled(unsigned servoIndex) 
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


bool ESP8266_ISR_Servo::enable(unsigned servoIndex) 
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

  if ( servo[servoIndex].count >= servo[servoIndex].min )
    servo[servoIndex].enabled = true;

  return true;
}


bool ESP8266_ISR_Servo::disable(unsigned servoIndex) 
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
  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++) 
  {
    if ( (servo[servoIndex].count >= servo[servoIndex].min ) && !servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP8266_MAX_PIN) ) 
    {
      servo[servoIndex].enabled = true;
    }
  }
}

void ESP8266_ISR_Servo::disableAll() 
{
  // Disable all servos
  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++) 
  {
    servo[servoIndex].enabled = false;
  }
}

bool ESP8266_ISR_Servo::toggle(unsigned servoIndex) 
{
  if (servoIndex >= MAX_SERVOS) 
    return false;

  servo[servoIndex].enabled = !servo[servoIndex].enabled;
  
  return true;
}


int ESP8266_ISR_Servo::getNumServos() 
{
    return numServos;
}
