#include <Arduino.h>
#include <engine.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>

BluetoothConnector* bluetooth;
WifiConnector* wifi;
Engine* engine;
ButtonController* buttons;

void setup() {

  Serial.begin(115200);
  Serial.print("Baureihe 101, Version ");
  Serial.println(VERSION_CODE);

  engine = new Engine();
  wifi = new WifiConnector();
  buttons = new ButtonController();  
  buttons->onButtonPressed([]() {
    if(buttons->isReversed()) {
      engine->drive_backward();
    }
    else {
      engine->drive_forward();
    }
  });
  bluetooth = new BluetoothConnector();
  bluetooth->on(BLUETOOTH_COMMAND_START, 0, [](byte command, byte* buffer, byte bufferSize){
    Serial.println("Received Drive forward command!");
    
    engine->drive_forward();
  });

    bluetooth->on(BLUETOOTH_COMMAND_STOP, 0, [](byte command, byte* buffer, byte bufferSize){
    Serial.println("Received stop command!");
    
    engine->stop();
  });
}

// the loop function runs over and over again forever
void loop() {  
  wifi->Loop();  

  if(!wifi->isInOTAUpdate()) {
    bluetooth->Loop();
    buttons->Loop();
  }
}