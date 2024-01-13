#include <Arduino.h>
#include <engine.h>
#include <soundplayer.h>
#include <lights.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>
#include <http-logger.h>

BluetoothConnector* bluetooth;
WifiConnector* wifi;
Engine* engine;
ButtonController* buttons;
SoundPlayer* soundPlayer;
Lights* lights;

void setup() {
  logger = new Logger();
  logger->Log("Baureihe 101, Version ");
  logger->Log(VERSION_CODE);
  
  wifi = new WifiConnector();
  logger->setWifiConnector(wifi);
  logger->Log("Http Logger Ready!");

  engine = new Engine();

  lights = new Lights();
  soundPlayer = new SoundPlayer();

  // TODO: Nur zum Test:
  soundPlayer->playSound(1);

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
  /*bluetooth->onCommand(BLUETOOTH_COMMAND_START, 0, [](byte command, byte* buffer, byte bufferSize){
    logger->Log("Drive forward!");
    
    engine->setDirection(true);
    //engine->setSpeed(255);
  });*/

/*  bluetooth->onCommand(BLUETOOTH_COMMAND_STOP, 0, [](byte command, byte* buffer, byte bufferSize){
    logger->Log("Stop driving!");
    
    engine->stop();
  });*/

  bluetooth->onSpeedChanged([](byte* array, byte len){
    byte speed = array[0];
    engine->setSpeed(speed);
    logger->Log("Set speed "+String(speed));    
  });

  bluetooth->onColorChanged([](byte* array, byte len){
    //byte speed = array[0];
    //engine->setSpeed(speed);
    logger->Log("Set color " + String(array[0]) + "," + String(array[1]) + "," + String(array[2]));    
  });

  bluetooth->onCommand([](byte* buffer, byte bufferSize){
    
    if(buffer[0] == BLUETOOTH_COMMAND_WHISTLE) {
      logger->Log("Whistle!");
      soundPlayer->playSound(1);
    }
  });

  lights->setGlobalColor(255, 128, 0);
}


long nextSwitch = 0;
bool ledState = false;

// the loop function runs over and over again forever
void loop() {  
  wifi->Loop();  

  lights->Loop();
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