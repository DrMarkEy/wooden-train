//#include <Arduino.h>
#include <engine.h>
#include <soundplayer.h>
//#include <lights.h>
#include <buttons.h>
//#include <connectivity/wifi.h>
//#include <connectivity/bluetooth.h>
//#include <http-logger.h>
//#include <track-sensor.h>


//BluetoothConnector* bluetooth;
//WifiConnector* wifi;

Engine* engine;
ButtonController buttonController;
SoundPlayer soundPlayer;
//Lights* lights;
//TrackSensor* trackSensor;

void setup() {
  delay(500);
    Serial.begin(SERIAL_BAUD_RATE);
    delay(500);
Serial.println("Running!");
  /*logger = new Logger();
  logger->Log("Baureihe 101, Version ");
  logger->Log(VERSION_CODE);
  
  wifi = new WifiConnector();

  wifi->onWifiConnected([]() {
    logger->setWifiConnector(wifi);
    logger->Log("Http Logger Ready!");
  });*/

  engine = new Engine();

  buttonController.Setup();
  buttonController.onButtonPressed([]() {

    if(engine->getSpeed() == 0) {
      if(buttonController.isReversed()) {
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
  /*
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
  trackSensor->onColorSignalDetected([](uint8_t color) {
    // TODO: Replace with gradually slowing down?!?
    if(color == SIGNAL_STOP) {
      engine->setSpeed(0);
    }

    /*sensorColor[0] = color;
    sensorColor[1] = 10;
    sensorColor[2] = 255;
    sensorColor[3] = 50;
    bluetooth->setSensorColor(sensorColor);* /
  });  

  lights = new Lights();
  lights->setGlobalColor(255, 0, 0);*/

  soundPlayer.playSound(1);
}


// the loop function runs over and over again forever
long countTimer = millis();
void loop() {
  /*LOG_DURATION(logger, "Wifi Loop", wifi->Loop());

  LOG_DURATION(logger, "OTA Update check", 
    if(wifi->isInOTAUpdate()) 
      return;
  );
  */


  //LOG_DURATION(logger, "Buttons", );
  /*LOG_DURATION(logger, "Lights", lights->Loop());
  */

 /*
  LOG_DURATION(logger, "TrackSensor", trackSensor->Loop());

  if(millis() - countTimer > 1000) {
    countTimer = millis();
    uint8_t measurements = trackSensor->resetMeasurementCount();
    logger->Logf("Measured %d colors", measurements);
  }*/
}