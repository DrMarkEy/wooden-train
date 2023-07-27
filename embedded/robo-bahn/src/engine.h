#include <Arduino.h>
#include <config.h>


void initialize_engine()
{
  // initialize pins as an output.
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);
}

// TODO: Set speed

void engine_drive_forward()
{
  digitalWrite(PIN_MOTOR_A, false);
  digitalWrite(PIN_MOTOR_B, true);  
}

void engine_drive_backward()
{
  digitalWrite(PIN_MOTOR_A, true);
  digitalWrite(PIN_MOTOR_B, false);  
}

void engine_stop()
{
  digitalWrite(PIN_MOTOR_A, false);
  digitalWrite(PIN_MOTOR_B, false);  
}