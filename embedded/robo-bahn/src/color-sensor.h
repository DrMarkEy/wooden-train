/*
  color-sensor.h - A bridge to the TCS34725 that has some sensible default settings for our use-case
  Created by Marc Mendler, Oktober 2023
*/
#ifndef colorsensor_h
#define colorsensor_h

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_TCS34725.h"

#include <http-logger.h>
#include <lab-utils.h>

// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/colorview/colorview.ino
// https://de.wikipedia.org/wiki/Lab-Farbraum

#define INTEGRATION_CYCLES 2

// Wood is only detected if the distance to it is half as much as to all other colors
// This is to prevent detecting white as wood accidentally
#define WOOD_EXTRA_DISTANCE_FACTOR 1
#define BLACK_EXTRA_DISTANCE_FACTOR 1
#define WHITE_EXTRA_DISTANCE_FACTOR 1
#define RED_EXTRA_DISTANCE_FACTOR 1
#define YELLOW_EXTRA_DISTANCE_FACTOR 1
#define GREEN_EXTRA_DISTANCE_FACTOR 1
#define BLUE_EXTRA_DISTANCE_FACTOR 1
#define MAGENTA_EXTRA_DISTANCE_FACTOR 1

#define COLOR_WOOD 0
#define COLOR_BLACK 1
#define COLOR_WHITE 2
#define COLOR_RED 3
#define COLOR_YELLOW 4
#define COLOR_GREEN 5
#define COLOR_BLUE 6
#define COLOR_MAGENTA 7

const float LAB_WOOD[] = {4.0, 0.1, 1.9};
const float LAB_BLACK[] = {1.1, 0.0, 0.0};
const float LAB_WHITE[] = {19.3, -6.0, 3.6};
const float LAB_RED[] = {2.4, 3.4, 1.3};
const float LAB_YELLOW[] = {15.8, -8.6, 17.0};
const float LAB_GREEN[] = {4.6, -4.5, 2.7};
const float LAB_BLUE[] = {2.8, -0.8, -3.7};
const float LAB_MAGENTA[] = {4.4, 4.9, -0.4};

volatile boolean interrupt = false;
void colorSensorInterrupt()
{
  interrupt = true;
}

class ColorSensor {

  private:
  Adafruit_TCS34725* colorSensor;
  void (*measurementCallback)(uint8_t color) = nullptr;
  bool working = false;
  long nextBrokenUpdate = 0;
  uint16_t r_acc, g_acc, b_acc;
  uint8_t integrationCycle = 0;

  void readColor(uint16_t* r, uint16_t* g, uint16_t* b) {
    uint16_t c;
    colorSensor->getRawData(r, g, b, &c);
  }

  uint8_t classifyColor(const float lab[3]) {
    float deltaMin = 1000000;
    float delta;
    uint8_t color;

    delta = deltaE(lab, LAB_WOOD) * WOOD_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_WOOD;
    }

    delta = deltaE(lab, LAB_BLACK) * BLACK_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_BLACK;
    }

    delta = deltaE(lab, LAB_WHITE) * WHITE_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_WHITE;
    }

    delta = deltaE(lab, LAB_RED) * RED_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_RED;
    }

    delta = deltaE(lab, LAB_YELLOW) * YELLOW_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_YELLOW;
    }

    delta = deltaE(lab, LAB_GREEN) * GREEN_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_GREEN;
    }

    delta = deltaE(lab, LAB_BLUE) * BLUE_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_BLUE;
    }

    delta = deltaE(lab, LAB_MAGENTA) * MAGENTA_EXTRA_DISTANCE_FACTOR;
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_MAGENTA;
    }

    return color;
  }

  public:

  ColorSensor(uint8_t ledPin)
  {
    colorSensor = new Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);
    r_acc = 0;
    g_acc = 0;
    b_acc = 0;

    // Initialize color sensor
    if (colorSensor->begin()) {
      logger.Log("Color Sensor initialized!");

      // Turn on COLOR-SENSOR LED
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin, true);

      working = true;


    } else {
      logger.Log("No TCS34725 found ... check your connections");
      working = false;
    }
  }

  void Loop() {

    if(!working) {
      if(millis() > nextBrokenUpdate) {
        logger.Log("ColorSensor broken!");
        nextBrokenUpdate = millis() + 1000;
      }

      return;
    }

    // Measure
    uint16_t r, g, b;
    readColor(&r, &g, &b);

    r_acc += r;
    g_acc += g;
    b_acc += b;
    integrationCycle++;

    if(integrationCycle == INTEGRATION_CYCLES) {
      /*
      logger->Log("Color Value:");
      logger->Logf("red %d", r);
      logger->Logf("green %d", g);
      logger->Logf("blue %d", b);
      */

      uint8_t rgb[3];
      rgb[0] = clamp(r_acc, 0, 255);
      rgb[1] = clamp(g_acc, 0, 255);
      rgb[2] = clamp(b_acc, 0, 255);

      /*
      logger->Log("Adjusted Value:");
      logger->Logf("red %d", rgb[0]);
      logger->Logf("green %d", rgb[1]);
      logger->Logf("blue %d", rgb[2]);
      */

      float labConverted[3];
      rgb2lab(rgb, labConverted);

      //logger.Logf("Lab Values:\n%f \n%f \n %f\n", labConverted[0], labConverted[1], labConverted[2]);

      uint8_t classifiedColor = classifyColor(labConverted);

      //if(classifiedColor != 0)
       // logger->Logf("Detected color %d", classifiedColor);

      if(measurementCallback != nullptr) {
        measurementCallback(classifiedColor);
      }

      r_acc = 0;
      g_acc = 0;
      b_acc = 0;
      integrationCycle = 0;
    }
  }

  void onMeasurement(void (*_callback) (uint8_t color))
  {
    this->measurementCallback = _callback;
  }
};

#endif