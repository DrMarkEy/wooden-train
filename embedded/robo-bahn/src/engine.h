/*
  engine.h - Controls the engine of the train
  Created by Marc Mendler, August 2023
*/
#ifndef engine_h
#define engine_h

#include <Arduino.h>
#include <config.h>

class Engine {

  public:

  Engine()
  {
    // initialize pins as an output.
    pinMode(PIN_MOTOR_A, OUTPUT);
    pinMode(PIN_MOTOR_B, OUTPUT);
  }

  // TODO: Set speed

  void drive_forward()
  {
    digitalWrite(PIN_MOTOR_A, false);
    digitalWrite(PIN_MOTOR_B, true);  
  }

  void drive_backward()
  {
    digitalWrite(PIN_MOTOR_A, true);
    digitalWrite(PIN_MOTOR_B, false);  
  }

  void stop()
  {
    digitalWrite(PIN_MOTOR_A, false);
    digitalWrite(PIN_MOTOR_B, false);  
  }

};
#endif