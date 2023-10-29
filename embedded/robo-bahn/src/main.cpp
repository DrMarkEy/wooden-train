#include <Arduino.h>
#include <engine.h>
#include <soundplayer.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>
#include <http-logger.h>




BluetoothConnector* bluetooth;
WifiConnector* wifi;
Engine* engine;
ButtonController* buttons;
SoundPlayer* soundPlayer;


void setup() {
  logger = new Logger();
  logger->Log("Baureihe 101, Version ");
  logger->Log(VERSION_CODE);
  
  wifi = new WifiConnector();
  logger->setWifiConnector(wifi);
  logger->Log("Http Logger Ready!");

  engine = new Engine();

  soundPlayer = new SoundPlayer();

  buttons = new ButtonController();  
  buttons->onButtonPressed([]() {
    if(buttons->isReversed()) {
      engine->setDirection(false);
      engine->setSpeed(255);      
    }
    else {
      engine->setDirection(true);
      engine->setSpeed(255);
    }
  });
  bluetooth = new BluetoothConnector();
  bluetooth->onCommand(BLUETOOTH_COMMAND_START, 0, [](byte command, byte* buffer, byte bufferSize){
    logger->Log("Drive forward!");
    
    engine->setDirection(true);
    //engine->setSpeed(255);
  });

  bluetooth->onCommand(BLUETOOTH_COMMAND_STOP, 0, [](byte command, byte* buffer, byte bufferSize){
    logger->Log("Stop driving!");
    
    engine->stop();
  });

  bluetooth->onSpeedChanged([](byte speed){
    engine->setSpeed(speed);
    logger->Log("Set speed "+String(speed));    
  });



}


long nextSwitch = 0;
bool ledState = false;

// the loop function runs over and over again forever
void loop() {  
  wifi->Loop();  

  soundPlayer->Loop();

  if(!wifi->isInOTAUpdate()) {   
    buttons->Loop();
  }

  if(nextSwitch < millis()) {
    nextSwitch = millis() + 1000;
    
    // LED BLINKING
    //ledState = !ledState;
    //digitalWrite(4, ledState);
  }
}