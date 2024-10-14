/*
  track-sensor.h - Controls the track sensor of the train (color sensor and magnetic sensor)
  Created by Marc Mendler, Oktober 2023
*/
#ifndef tracksensor_h
#define tracksensor_h

#include <Arduino.h>

#include <color-sensor.h>
#include <cyclic-buffer.h>
#include <http-logger.h>
#include <library.h>

class TrackSensor;
extern TrackSensor* trackSensor;

#define COLOR_IRRELEVANT 255
static const uint8_t SIGNAL_STOP_COLORS[3] = {COLOR_WHITE, COLOR_RED, COLOR_IRRELEVANT};
static const uint8_t SIGNAL_STOP_COLORS_2[3] = {COLOR_WHITE, COLOR_IRRELEVANT, COLOR_RED};

#define SIGNAL_STOP 1

class TrackSensor {

  private:
  ColorSensor* colorSensor;
  void (*colorSignalCallback)(uint8_t color) = nullptr;
  CyclicBuffer<uint8_t, 80> lastMeasurements = CyclicBuffer<uint8_t, 80>(COLOR_WOOD);
  uint8_t measurementCounter = 0;

  void readMagneticField() {

    // Hall-Sensor-Test
    /*Serial.print("Left: ");
    Serial.println(analogRead(2));

    Serial.print("Right: ");
    Serial.println(analogRead(15));*/
  }

  void colorMeasurement(uint8_t classifiedColor) {
    lastMeasurements.add(classifiedColor);
    measurementCounter ++;

    if(checkSignalMatch(SIGNAL_STOP_COLORS) || checkSignalMatch(SIGNAL_STOP_COLORS_2)) {
      logger->Logf("Detected stop signal!");
      colorSignalCallback(SIGNAL_STOP);
    }
  }

  bool checkSignalMatch(const uint8_t* signal) {
    for(uint8_t i = 0; i < 3; i++) {
      if(signal[i] != COLOR_IRRELEVANT) {
        if(lastMeasurements[i] != signal[i]) {
          return false;
        }
      }
    }
    return true;
  }

  public:

  TrackSensor()
  {    
    colorSensor = new ColorSensor(PIN_COLOR_SENSOR_LED);
    colorSensor->onMeasurement([](uint8_t color) {
      trackSensor->colorMeasurement(color);
    });
  }

  void Loop()
  {    
    colorSensor->Loop();
  }

  uint8_t resetMeasurementCount() {    
    lastMeasurements.print();

    uint8_t result = measurementCounter;
    measurementCounter = 0;
    return result;
  }

  void onColorSignalDetected(void (*_callback) (uint8_t color))
  {		
    this->colorSignalCallback = _callback;	
  }
};
#endif