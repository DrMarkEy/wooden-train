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

  // Automatically update engine direction when vehicle state changes
  vehicleState.addOnStateChangedCallback([](uint8_t changedState) {
    if(changedState == VEHICLE_STATE_OPERATION_MODE || changedState == VEHICLE_STATE_DRIVING_DIRECTION || changedState == VEHICLE_STATE_ENGINE_SPEED) {

      // Update engine speed and direction
      engine->setDirection(vehicleState.getDrivingDirection() == DRIVING_DIRECTION_FORWARD);
      if(vehicleState.getOperationMode() == OPERATION_MODE_STOPPED) {
        engine->setSpeed(0);
      }
      else {
        engine->setSpeed(vehicleState.getEngineSpeed());
      }
    }

    // Activate / deactivate color sensor
    if(changedState == VEHICLE_STATE_COLOR_SENSOR_ENABLED || changedState == VEHICLE_STATE_OPERATION_MODE) {
      if(vehicleState.isColorSensorEnabled() && vehicleState.getOperationMode() != OPERATION_MODE_STOPPED) {
        trackSensor->enableColorSensor();
        logger.Log("Color sensor enabled");
      }
      else {
        trackSensor->disableColorSensor();
        logger.Log("Color sensor disabled");
      }
    }
  });

  buttonController.Setup();
  buttonController.onButtonPressed([]() {
    // Always switch between stopped and driving mode
    if(vehicleState.getOperationMode() == OPERATION_MODE_STOPPED) {
      vehicleState.setOperationMode(OPERATION_MODE_DRIVING);
    }
    else {
      vehicleState.setOperationMode(OPERATION_MODE_STOPPED);
    }
  });

  bluetooth.Setup(&vehicleState);
  bluetooth.onCommand([](uint8_t* buffer, size_t bufferSize)
  {

    if(buffer[0] == BLUETOOTH_COMMAND_WHISTLE)
    {
      logger.Log("Whistle!");
      soundPlayer.playSound(1);
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_REVERSE)
    {
      engine->stop();
      delay(100);

      // Switch operation mode
      if(vehicleState.getDrivingDirection() == DRIVING_DIRECTION_FORWARD) {
        vehicleState.setDrivingDirection(DRIVING_DIRECTION_REVERSE);
      }
      else {
        vehicleState.setDrivingDirection(DRIVING_DIRECTION_FORWARD);
      }
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_START) {
      vehicleState.setOperationMode(OPERATION_MODE_DRIVING);
    }
    else if(buffer[0] == BLUETOOTH_COMMAND_STOP) {
      vehicleState.setOperationMode(OPERATION_MODE_STOPPED);
    }
    else if (buffer[0] == BLUETOOTH_COMMAND_TOGGLE_COLOR_SENSOR) {
      bool enabled = vehicleState.isColorSensorEnabled();
      vehicleState.setColorSensorEnabled(!enabled);
    }
  });

  bluetooth.onColorChanged([](uint8_t* array, size_t len){
    logger.Log("Set color " + String(array[0]) + "," + String(array[1]) + "," + String(array[2]));
    //lights->setDirectionColorWithColoredBacklights(true, array[0], array[1], array[2]);
  });


  trackSensor = new TrackSensor();
  trackSensor->onColorChangeDetected([](uint8_t color) {
    vehicleState.setLastReadColor(color);
  });

  trackSensor->onColorSignalDetected([](uint8_t signal) {
    // TODO: Replace with gradually slowing down?!?
    if(signal == SIGNAL_STOP) {
      vehicleState.setOperationMode(OPERATION_MODE_STOPPED);
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
      vehicleState.setDrivingDirection(1 - vehicleState.getDrivingDirection());
      /*byte oldSpeed = engine->getSpeed();
      engine->stop();
      delay(100);
      engine->setDirection(!engine->getDirection());
      delay(100);
      engine->setSpeed(oldSpeed);*/
    }

  });
  /*
  lights = new Lights();
  lights->setGlobalColor(255, 0, 0);*/

  vehicleState.setEngineSpeed(255);
  vehicleState.setOperationMode(OPERATION_MODE_STOPPED);

  if(buttonController.isReversed()) {
    vehicleState.setDrivingDirection(DRIVING_DIRECTION_REVERSE);
  }
  else {
    vehicleState.setDrivingDirection(DRIVING_DIRECTION_FORWARD);
  }

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
    //logger.Logf("Measured %d colors in 1s", measurements);
  }
}