/*
  track-sensor.h - Controls the track sensor of the train (color sensor and magnetic sensor)
  Created by Marc Mendler, Oktober 2023
*/
#ifndef tracksensor_h
#define tracksensor_h

#include <Arduino.h>
#include "Adafruit_TCS34725.h"

#include <http-logger.h>
#include <library.h>

#define UPDATE_INTERVAL 1000

// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/colorview/colorview.ino
// https://de.wikipedia.org/wiki/Lab-Farbraum

#define COLOR_NONE 0
#define COLOR_BLACK 1
#define COLOR_RED 2
#define COLOR_YELLOW 3
#define COLOR_GREEN 4
#define COLOR_BLUE 5

class TrackSensor {

  private:
  Adafruit_TCS34725* colorSensor;
  long nextUpdate = 0;
  void (*colorChangedCallback)(uint8_t color) = nullptr;

  void readColor() {
    
 uint16_t r, g, b, c;//, colorTemp, lux;

  colorSensor->getRawData(&r, &g, &b, &c);  
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  //lux = tcs.calculateLux(r, g, b);
  
  logger->Logf("red %d", r);
  logger->Logf("green %d", g);
  logger->Logf("blue %d", b);
  }

  void readMagneticField() {

    // Hall-Sensor-Test
    /*Serial.print("Left: ");
    Serial.println(analogRead(2));

    Serial.print("Right: ");
    Serial.println(analogRead(15));*/
  }

  public:

  TrackSensor()
  {    
    colorSensor = new Adafruit_TCS34725();
  }

  void init() {
    // COLOR-SENSOR LED
    pinMode(4, OUTPUT);
    digitalWrite(4, true);

    // Initialize color sensor
    if (colorSensor->begin()) {
      logger->Log("Color Sensor initialized!");
      
    } else {
      logger->Log("No TCS34725 found ... check your connections");      
    }
  }

  void Loop()
  {
    if(millis() > nextUpdate) {

      // TODO: Only trigger if color actually changed!
      if(colorChangedCallback != nullptr) {
        //colorChangedCallback(COLOR_BLUE);
        readColor();
      }

      nextUpdate = millis() + UPDATE_INTERVAL;
    }
  }

  void onColorChangeDetected(void (*_callback) (uint8_t color))
  {		
    this->colorChangedCallback = _callback;	
  }

};
#endif