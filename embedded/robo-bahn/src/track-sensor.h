/*
  track-sensor.h - Controls the track sensor of the train (color sensor and magnetic sensor)
  Created by Marc Mendler, Oktober 2023
*/
#ifndef tracksensor_h
#define tracksensor_h

#include <Arduino.h>

#include <color-sensor.h>
#include "CircularBuffer.hpp"
#include <http-logger.h>
#include <library.h>

class TrackSensor;
extern TrackSensor* trackSensor;

#define COLOR_BUFFER_SIZE 80

#define COLOR_IRRELEVANT 255
#define SIGNAL_CODE_LENGTH 5
static const uint8_t SIGNAL_STOP_COLORS[SIGNAL_CODE_LENGTH]   = {COLOR_IRRELEVANT, COLOR_IRRELEVANT, COLOR_IRRELEVANT, COLOR_WHITE, COLOR_RED};
static const uint8_t SIGNAL_STOP_COLORS_2[SIGNAL_CODE_LENGTH] = {COLOR_IRRELEVANT, COLOR_IRRELEVANT, COLOR_WHITE, COLOR_IRRELEVANT, COLOR_RED};
static const uint8_t SIGNAL_STOP_COLORS_3[SIGNAL_CODE_LENGTH] = {COLOR_IRRELEVANT, COLOR_WHITE, COLOR_IRRELEVANT, COLOR_IRRELEVANT, COLOR_RED};
static const uint8_t SIGNAL_STOP_COLORS_4[SIGNAL_CODE_LENGTH] = {COLOR_WHITE, COLOR_IRRELEVANT, COLOR_IRRELEVANT, COLOR_IRRELEVANT, COLOR_RED};

#define SIGNAL_STOP 1

class TrackSensor {

  private:
  ColorSensor* colorSensor;
  void (*colorSignalCallback)(uint8_t color) = nullptr;
  CircularBuffer<uint8_t, COLOR_BUFFER_SIZE> lastColorMeasurements;
  uint8_t measurementCounter = 0;

  void readMagneticField() {

    // Hall-Sensor-Test
    /*logger.Log("Left: ");
    logger.Log(analogRead(2));

    logger.Log("Right: ");
    logger.Log(analogRead(15));*/
  }

  void colorMeasurement(uint8_t classifiedColor) {
    lastColorMeasurements.push(classifiedColor);
    measurementCounter ++;

    if(checkSignalMatch(SIGNAL_STOP_COLORS) || checkSignalMatch(SIGNAL_STOP_COLORS_2) || checkSignalMatch(SIGNAL_STOP_COLORS_3) || checkSignalMatch(SIGNAL_STOP_COLORS_4)) {
      logger.Logf("Detected stop signal!");
      colorSignalCallback(SIGNAL_STOP);
    }
  }

  bool checkSignalMatch(const uint8_t* signal) {
    for(uint8_t i = 0; i < SIGNAL_CODE_LENGTH; i++) {
      if(signal[i] != COLOR_IRRELEVANT) {
        if(lastColorMeasurements[COLOR_BUFFER_SIZE - SIGNAL_CODE_LENGTH + i] != signal[i]) {
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

    // Initialize buffer with COLOR_WOOD
    for(uint8_t i = 0; i < COLOR_BUFFER_SIZE; i++) {
      lastColorMeasurements.push(COLOR_WOOD);
    }    
  }

  void Loop()
  {    
    colorSensor->Loop();
  }

  uint8_t resetMeasurementCount() {
    String str = "";
    for(uint8_t i = 0; i < COLOR_BUFFER_SIZE; i++) {
      str += String(lastColorMeasurements[i]);
      str += " ";
    }
    logger.Log(str);

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