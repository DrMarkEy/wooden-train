#include <Arduino.h>
#include <engine.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>
#include <http-logger.h>

BluetoothConnector* bluetooth;
WifiConnector* wifi;
Engine* engine;
ButtonController* buttons;

void setup() {
  logger = new Logger();
  logger->Log("Baureihe 101, Version ");
  logger->Log(VERSION_CODE);
  
  wifi = new WifiConnector();
  logger->setWifiConnector(wifi);
  logger->Log("Http Logger Ready!");

  engine = new Engine();
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
    logger->Log("Drive forward!");
    
    engine->drive_forward();
  });

    bluetooth->on(BLUETOOTH_COMMAND_STOP, 0, [](byte command, byte* buffer, byte bufferSize){
    logger->Log("Stop driving!");
    
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