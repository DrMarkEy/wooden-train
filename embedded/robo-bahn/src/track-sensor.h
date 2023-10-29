/*
  track-sensor.h - Controls the track sensor of the train (color sensor and magnetic sensor)
  Created by Marc Mendler, Oktober 2023
*/
#ifdef tracksensor_h  // TODO: ifndef
#define tracksensor_h

#include <Arduino.h>
#include "Adafruit_TCS34725.h"


#include <http-logger.h>
#include <library.h>

// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/colorview/colorview.ino
// https://de.wikipedia.org/wiki/Lab-Farbraum

class TrackSensor {

  private:
  //Adafruit_TCS34725* colorSensor;

  void readColor() {
    
/* uint16_t r, g, b, c;//, colorTemp, lux;

  colorSensor.getRawData(&r, &g, &b, &c);  
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  //lux = tcs.calculateLux(r, g, b);
  
  Serial.print("red ");
  Serial.println(r);

  Serial.print("green ");
  Serial.println(g);
  
  Serial.print("blue ");
  Serial.println(b);*/

  }

  void readMagneticField() {

    // Hall-Sensor-Test
    /*Serial.print("Left: ");
    Serial.println(analogRead(2));

    Serial.print("Right: ");
    Serial.println(analogRead(15));*/
  }

  public:

  TrackSensor(Logger* logger)
  {    
    //colorSensor = new Adafruit_TCS34725();


     // COLOR-SENSOR LED
    pinMode(4, OUTPUT);
    digitalWrite(4, true);

    // Initialize color sensor
    /*if (colorSensor->begin()) {
      logger->Log("Color Sensor initialized!");
      
    } else {
      logger->Log("No TCS34725 found ... check your connections");      
    }*/
  }

  void Loop()
  {
    //EVERY_X_MS(500, readColor)
    //EVERY_X_MS(250, readMagneticField)
  }

};
#endif