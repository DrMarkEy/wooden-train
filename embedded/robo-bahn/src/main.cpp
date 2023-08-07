#include <Arduino.h>
#include <engine.h>
#include <buttons.h>
#include <connectivity/wifi.h>

#define PIN_A 17
#define PIN_B 16


void setup() {

  Serial.begin(115200);
  Serial.print("Baureihe 101, Version ");
  Serial.println(VERSION_CODE);

 initialize_engine();
  initialize_wifi();  
}

// the loop function runs over and over again forever
void loop() {
  update_wifi();  

  if(!inOTAUpdate) {
  update_button_states();
  }
}