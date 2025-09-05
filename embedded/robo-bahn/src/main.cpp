#include <Arduino.h>
#include <engine.h>
#include <soundplayer.h>
//#include <lights.h>
#include <buttons.h>
#include <connectivity/wifi.h>
#include <connectivity/bluetooth.h>
#include <http-logger.h>
#include <track-sensor.h>
#include <vehicle-state.hpp>
//#define OPERATION_MODE_STOPPED_AT_SIGNAL 5 -> Use STOPPED

Logger logger;

BluetoothConnector bluetooth;
WifiConnector wifi;

Engine* engine;
ButtonController buttonController;
SoundPlayer soundPlayer;
//Lights* lights;
TrackSensor* trackSensor;
VehicleState vehicleState;

void setup() {
  logger.Setup();
  logger.Log("Baureihe 101, Version ");
  logger.Log(VERSION_CODE);

  wifi.Run();

  wifi.onWifiConnected([]() {
    logger.setWifiConnector(&wifi);
    logger.Log("Http Logger Ready!");
  });

  engine = new Engine();

  buttonController.Setup();
  buttonController.onButtonPressed([]() {

    if(engine->getSpeed() == 0) {
      if(buttonController.isReversed()) {
        engine->setSpeed(255);

        vehicleState.setOperationMode(OPERATION_MODE_DRIVING);
        vehicleState.setDrivingDirection(DRIVING_DIRECTION_REVERSE);
        engine->setDirection(false);
      }
      else {
        engine->setSpeed(255);

        vehicleState.setOperationMode(OPERATION_MODE_DRIVING);
        vehicleState.setDrivingDirection(DRIVING_DIRECTION_FORWARD);
        engine->setDirection(true);
      }
    }
    else {
      engine->setSpeed(0);
      vehicleState.setOperationMode(OPERATION_MODE_STOPPED);
    }
  });

  bluetooth.Setup(&vehicleState);
  bluetooth.onSpeedChanged([](uint8_t* array, size_t len){
    uint8_t speed = array[0];
    engine->setSpeed(speed);
    logger.Log("Set speed "+String(speed));
  });

  bluetooth.onColorChanged([](uint8_t* array, size_t len){
    logger.Log("Set color " + String(array[0]) + "," + String(array[1]) + "," + String(array[2]));
    //lights->setDirectionColorWithColoredBacklights(true, array[0], array[1], array[2]);
  });

  bluetooth.onCommand([](uint8_t* buffer, size_t bufferSize)
  {

    if(buffer[0] == BLUETOOTH_COMMAND_WHISTLE)
    {
      logger.Log("Whistle!");
      soundPlayer.playSound(1);
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_REVERSE)
    {
      uint8_t oldSpeed = engine->getSpeed();
      engine->stop();
      delay(100);
      engine->setDirection(!engine->getDirection());
      delay(100);
      engine->setSpeed(oldSpeed);

      // Switch operation mode
      if(vehicleState.getDrivingDirection() == DRIVING_DIRECTION_FORWARD) {
        vehicleState.setDrivingDirection(DRIVING_DIRECTION_REVERSE);
      }
      else {
        vehicleState.setDrivingDirection(DRIVING_DIRECTION_FORWARD);
      }
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_START) {
      if(engine->getDirection()) {
        vehicleState.setOperationMode(OPERATION_MODE_DRIVING);
      }
      else {
        vehicleState.setOperationMode(OPERATION_MODE_DRIVING);
      }

      engine->setSpeed(bluetooth.getSpeed());
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_STOP) {
      vehicleState.setOperationMode(OPERATION_MODE_STOPPED);

      engine->setSpeed(0);
    }

  });


  trackSensor = new TrackSensor();
  trackSensor->onColorChangeDetected([](uint8_t color) {
    vehicleState.setLastReadColor(color);
  });

  trackSensor->onColorSignalDetected([](uint8_t signal) {
    // TODO: Replace with gradually slowing down?!?
    if(signal == SIGNAL_STOP) {
      engine->setSpeed(0);
    }
    else if(signal == SIGNAL_WAIT) {
      byte oldSpeed = engine->getSpeed();
      engine->stop();
      delay(2500);
      engine->setSpeed(oldSpeed);
    }
    else if(signal == SIGNAL_HONK) {
      soundPlayer.playSound(1);
    }
    else if(signal == SIGNAL_REVERSE) {
      byte oldSpeed = engine->getSpeed();
      engine->stop();
      delay(100);
      engine->setDirection(!engine->getDirection());
      delay(100);
      engine->setSpeed(oldSpeed);
    }

  });
  /*
  lights = new Lights();
  lights->setGlobalColor(255, 0, 0);*/

  vehicleState.setOperationMode(OPERATION_MODE_STOPPED);
  vehicleState.setDrivingDirection(DRIVING_DIRECTION_FORWARD);
  vehicleState.setColorSensorEnabled(true);
  soundPlayer.playSound(1);
}


// the loop function runs over and over again forever
long countTimer = millis();
void loop() {
  /*LOG_DURATION(logger, "Lights", lights->Loop());
  */


  LOG_DURATION(logger, "TrackSensor", trackSensor->Loop());

  if(millis() - countTimer > 1000) {
    countTimer = millis();
    uint8_t measurements = trackSensor->resetMeasurementCount();
    logger.Logf("Measured %d colors", measurements);
  }
}