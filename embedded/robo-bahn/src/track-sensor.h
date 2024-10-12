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

// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/colorview/colorview.ino
// https://de.wikipedia.org/wiki/Lab-Farbraum

// Wood is only detected if the distance to it is half as much as to all other colors
// This is to prevent detecting white as wood accidentally
#define WOOD_EXTRA_DISTANCE_FACTOR 2

#define COLOR_WOOD 0
#define COLOR_BLACK 1
#define COLOR_WHITE 2
#define COLOR_RED 3
#define COLOR_YELLOW 4
#define COLOR_GREEN 5
#define COLOR_BLUE 6

const float LAB_WOOD[] = {27.67, 1.21, 14.61};
const float LAB_BLACK[] = {0, 0, 0};
const float LAB_WHITE[] = {91.73, -20.98, 15.95};
const float LAB_RED[] = {17.35, 28.18, 17.46};
const float LAB_YELLOW[] = {56.13, 2.43, 46.49};
const float LAB_BLUE[] = {19.5, -6.74, -11.59}; 
const float LAB_GREEN[] = {8.89, -10.81, 6.09};

volatile boolean interrupt = false;
void colorSensorInterrupt() 
{
  interrupt = true;
}

class TrackSensor {

  private:
  Adafruit_TCS34725* colorSensor;
  void (*colorChangedCallback)(uint8_t color) = nullptr;  
  bool working = false;
  long nextBrokenUpdate = 0;

  uint8_t lastColor = COLOR_WOOD;

  void readColor(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {  
    // Don't use the commented out function, as it uses a delay internally
    //colorSensor->getRawData(r, g, b, c);    

    *c = colorSensor->read16(TCS34725_CDATAL);
    *r = colorSensor->read16(TCS34725_RDATAL);
    *g = colorSensor->read16(TCS34725_GDATAL);
    *b = colorSensor->read16(TCS34725_BDATAL);  
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
    // TODO: Try different integration times and gains
    //colorSensor = new Adafruit_TCS34725();
    colorSensor = new Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);

    // Setup interrupt pin
    pinMode(PIN_COLOR_SENSOR_INTERRUPT, INPUT); //TCS interrupt output is Active-LOW and Open-Drain. Pin has a soldered on pullup resistor
    attachInterrupt(digitalPinToInterrupt(PIN_COLOR_SENSOR_INTERRUPT), colorSensorInterrupt, FALLING);

    // Initialize color sensor
    if (colorSensor->begin()) {
      logger->Log("Color Sensor initialized!");

      // Turn on COLOR-SENSOR LED
      pinMode(PIN_COLOR_SENSOR_LED, OUTPUT);
      digitalWrite(PIN_COLOR_SENSOR_LED, true);

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

      uint8_t classifiedColor;
      classifiedColor = classifyColor(labConverted);
      //logger->Logf("Detected color %d", classifiedColor);

      if(classifiedColor != lastColor) {
        lastColor = classifiedColor;
        
        logger->Logf("Detected new color %d", classifiedColor);
        if(colorChangedCallback != nullptr) {
          colorChangedCallback(classifiedColor);
        }
      }
        
      colorSensor->clearInterrupt();
      interrupt = false;
     }
  }

void onColorChangeDetected(void (*_callback) (uint8_t color))
{		
  this->colorChangedCallback = _callback;	
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

  delta = deltaE(lab, LAB_YELLOW);  
  if(delta < deltaMin) {
    deltaMin = delta;
    color = COLOR_YELLOW;
  }

  delta = deltaE(lab, LAB_BLUE);  
  if(delta < deltaMin) {
    deltaMin = delta;
    color = COLOR_BLUE;
  }

  delta = deltaE(lab, LAB_GREEN);  
  if(delta < deltaMin) {
    deltaMin = delta;
    color = COLOR_GREEN;
  }

  // ...

  return color;
}

void lab2rgb(const float lab[3], uint8_t rgb[3]) {
    float y = (lab[0] + 16) / 116;
    float x = lab[1] / 500 + y;
    float z = y - lab[2] / 200;
    float r, g, b;

    x = 0.95047f * ((x * x * x > 0.008856f) ? x * x * x : (x - 16.0f / 116) / 7.787f);
    y = 1.00000f * ((y * y * y > 0.008856f) ? y * y * y : (y - 16.0f / 116) / 7.787f);
    z = 1.08883f * ((z * z * z > 0.008856f) ? z * z * z : (z - 16.0f / 116) / 7.787f);

    r = x * 3.2406f + y * -1.5372f + z * -0.4986f;
    g = x * -0.9689f + y * 1.8758f + z * 0.0415f;
    b = x * 0.0557f + y * -0.2040f + z * 1.0570f;

    r = (r > 0.0031308f) ? (1.055f * pow(r, 1.0f / 2.4f) - 0.055f) : 12.92f * r;
    g = (g > 0.0031308f) ? (1.055f * pow(g, 1.0f / 2.4f) - 0.055f) : 12.92f * g;
    b = (b > 0.0031308f) ? (1.055f * pow(b, 1.0f / 2.4f) - 0.055f) : 12.92f * b;

    rgb[0] = static_cast<uint8_t>(clamp(r * 255.0f, 0.0f, 255.0f));
    rgb[1] = static_cast<uint8_t>(clamp(g * 255.0f, 0.0f, 255.0f));
    rgb[2] = static_cast<uint8_t>(clamp(b * 255.0f, 0.0f, 255.0f));
}

// Function to convert RGB to LAB
void rgb2lab(const uint8_t rgb[3], float lab[3]) {
    float r = rgb[0] / 255.0f;
    float g = rgb[1] / 255.0f;
    float b = rgb[2] / 255.0f;

    r = (r > 0.04045f) ? pow((r + 0.055f) / 1.055f, 2.4f) : r / 12.92f;
    g = (g > 0.04045f) ? pow((g + 0.055f) / 1.055f, 2.4f) : g / 12.92f;
    b = (b > 0.04045f) ? pow((b + 0.055f) / 1.055f, 2.4f) : b / 12.92f;

    float x = (r * 0.4124f + g * 0.3576f + b * 0.1805f) / 0.95047f;
    float y = (r * 0.2126f + g * 0.7152f + b * 0.0722f) / 1.00000f;
    float z = (r * 0.0193f + g * 0.1192f + b * 0.9505f) / 1.08883f;

    x = (x > 0.008856f) ? pow(x, 1.0f / 3.0f) : (7.787f * x) + 16.0f / 116;
    y = (y > 0.008856f) ? pow(y, 1.0f / 3.0f) : (7.787f * y) + 16.0f / 116;
    z = (z > 0.008856f) ? pow(z, 1.0f / 3.0f) : (7.787f * z) + 16.0f / 116;

    lab[0] = (116 * y) - 16;
    lab[1] = 500 * (x - y);
    lab[2] = 200 * (y - z);
}

// Function to compute perceptual distance between colors in CIELAB
float deltaE(const float labA[3], const float labB[3]) {
    float deltaL = labA[0] - labB[0];
    float deltaA = labA[1] - labB[1];
    float deltaB = labA[2] - labB[2];

    float c1 = sqrt(labA[1] * labA[1] + labA[2] * labA[2]);
    float c2 = sqrt(labB[1] * labB[1] + labB[2] * labB[2]);
    float deltaC = c1 - c2;
    float deltaH = deltaA * deltaA + deltaB * deltaB - deltaC * deltaC;
    deltaH = (deltaH < 0) ? 0 : sqrt(deltaH);

    float sc = 1.0f + 0.045f * c1;
    float sh = 1.0f + 0.015f * c1;

    float deltaLKlsl = deltaL / 1.0f;
    float deltaCkcsc = deltaC / sc;
    float deltaHkhsh = deltaH / sh;

    float i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh * deltaHkhsh;
    return (i < 0) ? 0 : sqrt(i);
}

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
  }
};
#endif