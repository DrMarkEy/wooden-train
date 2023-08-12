/*
  engine.h - Controls the engine of the train
  Created by Marc Mendler, August 2023
*/
#ifndef engine_h
#define engine_h

#include <Arduino.h>
#include <config.h>

class Engine {

  private:
  byte speed;
  bool forward = true;

  void update() {
    if(forward) {
      digitalWrite(PIN_MOTOR_A, false);
      analogWrite(PIN_MOTOR_B, (float)speed / SPEED_STEPS * 255);
    }
    else {
      analogWrite(PIN_MOTOR_A, (float)speed / SPEED_STEPS * 255);
      digitalWrite(PIN_MOTOR_B, false); 
    }
  }

  public:

  Engine()
  {
    // initialize pins as an output.
    pinMode(PIN_MOTOR_A, OUTPUT);
    pinMode(PIN_MOTOR_B, OUTPUT);
  }

  void setDirection(bool _forward)
  {
    forward = _forward;
    update();
  }

  void stop()
  {
    speed = 0;
    update();
  }

  void setSpeed(byte _speed)
  {
    speed = _speed;
    update();
  }

};
#endif