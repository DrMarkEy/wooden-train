#include <Arduino.h>

#define PIN_A 17
#define PIN_B 16


void setup() {
  // initialize pins as an output.
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  // Spin the motor clockwise direction for 2 seconds
  digitalWrite(PIN_A,LOW);
  digitalWrite(PIN_B,HIGH);  
}