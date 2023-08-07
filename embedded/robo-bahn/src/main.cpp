#include <Arduino.h>
#include <engine.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>

BluetoothConnector* bluetooth;

void setup() {

  Serial.begin(115200);
  Serial.print("Baureihe 101, Version ");
  Serial.println(VERSION_CODE);

  initialize_engine();
  initialize_wifi();  
  bluetooth = new BluetoothConnector();
  bluetooth->on(BLUETOOTH_COMMAND_START, 0, [](byte command, byte* buffer, byte bufferSize){
    Serial.println("Received Drive forward command!");
    engine_drive_forward();
  });

    bluetooth->on(BLUETOOTH_COMMAND_STOP, 0, [](byte command, byte* buffer, byte bufferSize){
    Serial.println("Received stop command!");
    engine_stop();
  });
}

// the loop function runs over and over again forever
void loop() {  
  update_wifi();  

  if(!inOTAUpdate) {
    update_button_states();
    bluetooth->Loop();
  }
}