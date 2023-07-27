#include <Arduino.h>
#include <engine.h>
#include <buttons.h>

#define PIN_A 17
#define PIN_B 16


void setup() {
 initialize_engine();
 //engine_drive_forward();
}

// the loop function runs over and over again forever
void loop() {
  update_button_states();
}