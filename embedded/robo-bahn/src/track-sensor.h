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

#define SIGNAL_STOP 1
#define SIGNAL_WAIT 2
#define SIGNAL_HONK 3
#define SIGNAL_REVERSE 4

class TrackSensor {

  private:
  ColorSensor* colorSensor;
  void (*colorChangedCallback)(uint8_t color) = nullptr;
  void (*colorSignalCallback)(uint8_t signal) = nullptr;

  // For debugging
  CircularBuffer<uint8_t, COLOR_BUFFER_SIZE> lastColorMeasurements;
  uint8_t measurementCounter = 0;

  // The least MEASURED color value. The color sensor measures a color in every step. Only colors that are measured at least 2 times count as detected.
  uint8_t lastMeasuredColor = COLOR_IRRELEVANT;
  // The currently detected color value. Is only updated when a different color is detected (i.e. measured two times in a row)
  uint8_t currentDetectedColor = COLOR_IRRELEVANT;
  // The previously detected color. Changes only if currentDetectedColor is updated to a new color.
  uint8_t previouslyDetectedColor = COLOR_IRRELEVANT;
  // Note: previouslyDetectedColor is always different from currentDetectedColor, as these values only update when different.

  void readMagneticField() {

    // Hall-Sensor-Test
    /*logger.Log("Left: ");
    logger.Log(analogRead(2));

    logger.Log("Right: ");
    logger.Log(analogRead(15));*/
  }

  void colorMeasurement(uint8_t classifiedColor) {
    // Only for debugging
    lastColorMeasurements.push(classifiedColor);
    measurementCounter ++;

    // ---- Actual detection logic ----
    // If the color was already measured in the previous step => count it as detected
    if(lastMeasuredColor == classifiedColor) {
      // The color is not already the currently detected one
      if(classifiedColor != currentDetectedColor) {
        previouslyDetectedColor = currentDetectedColor;
        currentDetectedColor = classifiedColor;

        onNewColorDetected();
      }
    }
    else // The color is different from the one in the previous step
    {
      lastMeasuredColor = classifiedColor;
    }
  }

  void onNewColorDetected() {
    if(previouslyDetectedColor == COLOR_WHITE) {
      if(currentDetectedColor == COLOR_RED) {
        logger.Logf("Detected stop signal!");
        colorSignalCallback(SIGNAL_STOP);
      }

      if(currentDetectedColor == COLOR_YELLOW) {
        logger.Logf("Detected wait signal!");
        colorSignalCallback(SIGNAL_WAIT);
      }

      if(currentDetectedColor == COLOR_GREEN) {
        logger.Logf("Detected honk signal!");
        colorSignalCallback(SIGNAL_HONK);
      }

      if(currentDetectedColor == COLOR_BLUE) {
        logger.Logf("Detected reverse signal!");
        colorSignalCallback(SIGNAL_REVERSE);
      }
    }

    colorChangedCallback(currentDetectedColor);
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
    // TODO: Use this for debugging the detected color sequence
    /*String str = "";
    for(uint8_t i = 0; i < COLOR_BUFFER_SIZE; i++) {
      str += String(lastColorMeasurements[i]);
      str += " ";
    }
    logger.Log(str);*/

    uint8_t result = measurementCounter;
    measurementCounter = 0;
    return result;
  }

  void onColorChangeDetected(void (*_callback) (uint8_t color))
  {
    this->colorChangedCallback = _callback;
  }

  void onColorSignalDetected(void (*_callback) (uint8_t signal))
  {
    this->colorSignalCallback = _callback;
  }
};
#endif