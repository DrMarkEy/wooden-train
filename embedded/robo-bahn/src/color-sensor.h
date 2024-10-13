/*
  color-sensor.h - A bridge to the TCS34725 that has some sensible default settings for our use-case
  Created by Marc Mendler, Oktober 2023
*/
#ifndef colorsensor_h
#define colorsensor_h

#include <Arduino.h>
#include "Adafruit_TCS34725.h"

#include <http-logger.h>
#include <lab-utils.h>

// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/colorview/colorview.ino
// https://de.wikipedia.org/wiki/Lab-Farbraum

// Wood is only detected if the distance to it is half as much as to all other colors
// This is to prevent detecting white as wood accidentally
#define WOOD_EXTRA_DISTANCE_FACTOR 2

#define COLOR_WOOD 0
#define COLOR_BLACK 1
#define COLOR_WHITE 2
#define COLOR_RED 3
//#define COLOR_YELLOW 4 // Deactivated for now, because produces to many false positives...
//#define COLOR_GREEN 5  // Deactivated for now, because produces to many false positives...
#define COLOR_BLUE 6

const float LAB_WOOD[] = {27.67, 1.21, 14.61};
const float LAB_BLACK[] = {0, 0, 0};
const float LAB_WHITE[] = {68.10, -14.61, 11.95};
const float LAB_RED[] = {11.52, 21.14, 11.27};
//const float LAB_YELLOW[] = {38.97, 2.35, 34.24};
const float LAB_BLUE[] = {19.5, -6.74, -11.59}; 
//const float LAB_GREEN[] = {8.89, -10.81, 6.09};

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

  void readColor(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {  
    // Don't use the commented out function, as it uses a delay internally
    //colorSensor->getRawData(r, g, b, c);    

    *c = colorSensor->read16(TCS34725_CDATAL);
    *r = colorSensor->read16(TCS34725_RDATAL);
    *g = colorSensor->read16(TCS34725_GDATAL);
    *b = colorSensor->read16(TCS34725_BDATAL);  
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

    delta = deltaE(lab, LAB_BLACK);  
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_BLACK;
    }

    delta = deltaE(lab, LAB_WHITE);  
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_WHITE;
    }

    delta = deltaE(lab, LAB_RED);  
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_RED;
    }

    /*delta = deltaE(lab, LAB_YELLOW);  
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_YELLOW;
    }*/

    delta = deltaE(lab, LAB_BLUE);  
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_BLUE;
    }

    /*delta = deltaE(lab, LAB_GREEN);  
    if(delta < deltaMin) {
      deltaMin = delta;
      color = COLOR_GREEN;
    }*/

    // ...

    return color;
  }

  public:

  ColorSensor(uint8_t interruptPin, uint8_t ledPin)
  {           
    colorSensor = new Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);

    // Setup interrupt pin
    pinMode(interruptPin, INPUT); //TCS interrupt output is Active-LOW and Open-Drain. Pin has a soldered on pullup resistor
    attachInterrupt(digitalPinToInterrupt(interruptPin), colorSensorInterrupt, FALLING);

    // Initialize color sensor
    if (colorSensor->begin()) {
      logger->Log("Color Sensor initialized!");

      // Turn on COLOR-SENSOR LED
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin, true);

      working = true;
      
    } else {
      logger->Log("No TCS34725 found ... check your connections");      
      working = false;
    }
  }

  void Loop()
  {    
    if (interrupt) {
      //logger->Log("Interrupted!");

      if(!working) {
        if(millis() > nextBrokenUpdate) {
          logger->Log("ColorSensor broken!");
          nextBrokenUpdate = millis() + 1000;
          return;
        }
      }

      uint16_t r, g, b, c;          
      readColor(&r, &g, &b, &c);
      
      /*
      logger->Log("Color Value:");
      logger->Logf("red %d", r);
      logger->Logf("green %d", g);
      logger->Logf("blue %d", b);
      */

      uint8_t rgb[3];
      rgb[0] = clamp(r, 0, 255);
      rgb[1] = clamp(g, 0, 255);
      rgb[2] = clamp(b, 0, 255);

      /*
      logger->Log("Adjusted Value:");        
      logger->Logf("red %d", rgb[0]);
      logger->Logf("green %d", rgb[1]);
      logger->Logf("blue %d", rgb[2]);
      */
      
      float labConverted[3];
      rgb2lab(rgb, labConverted);

      /*
      logger->Log("Lab Values:");
      logger->Logf("%f", labConverted[0]);
      logger->Logf("%f", labConverted[1]);
      logger->Logf("%f", labConverted[2]);
      */

      uint8_t classifiedColor = classifyColor(labConverted);
      //logger->Logf("Detected color %d", classifiedColor);

      if(measurementCallback != nullptr) {
        measurementCallback(classifiedColor);
      }
        
      colorSensor->clearInterrupt();
      interrupt = false;
     }
  }

  void onMeasurement(void (*_callback) (uint8_t color))
  {		
    this->measurementCallback = _callback;	
  }
};

#endif