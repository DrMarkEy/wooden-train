#include <Arduino.h>
#include <engine.h>
#include <soundplayer.h>
#include <lights.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>
#include <http-logger.h>
#include <track-sensor.h>


BluetoothConnector* bluetooth;
WifiConnector* wifi;

Engine* engine;
ButtonController* buttons;
SoundPlayer* soundPlayer;
Lights* lights;
TrackSensor* trackSensor;

void setup() {

  logger = new Logger();
  logger->Log("Baureihe 101, Version ");
  logger->Log(VERSION_CODE);
  
  wifi = new WifiConnector();

  wifi->onWifiConnected([]() {
    logger->setWifiConnector(wifi);
    logger->Log("Http Logger Ready!");
  });

  engine = new Engine();

  buttons = new ButtonController();  
  buttons->onButtonPressed([]() {
    if(engine->getSpeed() == 0) {
      if(buttons->isReversed()) {
        engine->setDirection(false);
        engine->setSpeed(255);      
      }
      else {
        engine->setDirection(true);
        engine->setSpeed(255);
      }
    }
    else {      
      engine->setSpeed(0);
    }
  });
  
  bluetooth = new BluetoothConnector();

  bluetooth->onSpeedChanged([](byte* array, byte len){
    byte speed = array[0];
    engine->setSpeed(speed);
    logger->Log("Set speed "+String(speed));    
  });

  bluetooth->onColorChanged([](byte* array, byte len){
    logger->Log("Set color " + String(array[0]) + "," + String(array[1]) + "," + String(array[2]));    
    lights->setDirectionColorWithColoredBacklights(true, array[0], array[1], array[2]);
  });

  bluetooth->onCommand([](byte* buffer, byte bufferSize)
  {
    
    if(buffer[0] == BLUETOOTH_COMMAND_WHISTLE) 
    {
      logger->Log("Whistle!");
      soundPlayer->playSound(1);
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_REVERSE) 
    {
      byte oldSpeed = engine->getSpeed();
      engine->stop();
      delay(100);
      engine->setDirection(!engine->getDirection());
      delay(100);
      engine->setSpeed(oldSpeed);
    }
  });

  trackSensor = new TrackSensor();
  trackSensor->onColorChangeDetected([](uint8_t color) {
    // TODO: Replace with gradually slowing down?!?
    if(color == COLOR_RED) {
      engine->setSpeed(0);
    }

    /*sensorColor[0] = color;
    sensorColor[1] = 10;
    sensorColor[2] = 255;
    sensorColor[3] = 50;
    bluetooth->setSensorColor(sensorColor);*/
  });  

  lights = new Lights();
  lights->setGlobalColor(255, 0, 0);

  soundPlayer = new SoundPlayer();
  soundPlayer->playSound(1);
}


// the loop function runs over and over again forever
void loop() {    
  logger->LogDuration("Wifi Loop", []() {
    wifi->Loop(); 
  }); 
  
  logger->LogDuration("OTA Update Check", []() {
    if(wifi->isInOTAUpdate()) 
      return;
  });

  logger->LogDuration("Buttons", []() {
    buttons->Loop();
  });

  logger->LogDuration("Lights", []() {
    lights->Loop();
  });

  logger->LogDuration("SoundPlayer", []() {
    soundPlayer->Loop();
  });

  logger->LogDuration("TrackSensor", []() {
    trackSensor->Loop();
  });
}