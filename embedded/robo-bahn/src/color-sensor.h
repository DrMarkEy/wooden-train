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

// Manually integrate over 3 measurements, as the automatic integration of the sensor would only allow integration 10 measurements, which is too much
#define INTEGRATION_STEPS 3

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

const float LAB_WOOD[] = {76.2, 8.4, 30.0};
const float LAB_BLACK[] = {6.4, 0.6, 0.9};
const float LAB_WHITE[] = {100.0, 0.0, 0.0};
const float LAB_RED[] = {44.9, 53.0, 32.7};
//const float LAB_YELLOW[] = {97.8, -16.7, 60.9};
const float LAB_BLUE[] = {51.6, -11.0, -20.4}; 
//const float LAB_GREEN[] = {49.9, -28.0, 18.4};

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
  uint8_t integrationStep = 0;
  uint16_t r_acc, g_acc, b_acc, c_acc;     

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

      // Measure
      uint16_t r, g, b, c;          
      readColor(&r, &g, &b, &c);
      
      // Integrate
      r_acc += r;
      g_acc += g;
      b_acc += b;
      c_acc += c;
      integrationStep++;

      if(integrationStep == INTEGRATION_STEPS) {
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

        integrationStep = 0;
        r_acc = 0;
        g_acc = 0;
        b_acc = 0;
        c_acc = 0;
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