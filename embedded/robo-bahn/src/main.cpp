#include <Arduino.h>
#include <engine.h>
#include <soundplayer.h>
#include <lights.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>
#include <http-logger.h>
#include <track-sensor.h>

#include "esp_system.h"
#include "esp_ota_ops.h"


BluetoothConnector* bluetooth;
WifiConnector* wifi;
Engine* engine;
ButtonController* buttons;
SoundPlayer* soundPlayer;
Lights* lights;
TrackSensor* trackSensor;

uint8_t sensorColor[4];



void setup() {


  logger = new Logger();
  logger->Log("Baureihe 101, Version ");
  logger->Log(VERSION_CODE);
  
  wifi = new WifiConnector();

  wifi->onWifiConnected([]() {
    logger->setWifiConnector(wifi);
    logger->Log("Http Logger Ready!");

    trackSensor->init();
  });

    const esp_partition_t* runningPartition = esp_ota_get_running_partition();
   if (runningPartition != NULL) {
        logger->Logf("Boot-Partition Label: %s", runningPartition->label);
        logger->Logf("Boot-Partition Typ: %d", runningPartition->type);
        logger->Logf("Boot-Partition Adresse: 0x%X", runningPartition->address);
        logger->Logf("Boot-Partition Größe: %d Bytes", runningPartition->size);
    } else {
        logger->Log("Fehler beim Abrufen der Boot-Partition-Informationen");
    }

/*  if( running_partition != ota0 ) {
    // copy partition ota1 to ota0
    // set boot partition to ota0
    //restart
  }*/




  engine = new Engine();

  lights = new Lights();
  soundPlayer = new SoundPlayer();
  soundPlayer->playSound(1);

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
    //byte speed = array[0];
    //engine->setSpeed(speed);
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
    sensorColor[0] = color;
    sensorColor[1] = 10;
    sensorColor[2] = 255;
    sensorColor[3] = 50;
    bluetooth->setSensorColor(sensorColor);
  });

  lights->setGlobalColor(255, 0, 0);
}


// the loop function runs over and over again forever
void loop() {  
  wifi->Loop();  

  if(wifi->isInOTAUpdate()) 
    return;
  
  buttons->Loop();
  lights->Loop();
  soundPlayer->Loop();
  trackSensor->Loop();
}