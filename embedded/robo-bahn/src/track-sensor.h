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

#define WAIT_INTERVAL 100
#define LED_SWITCH_WAIT_INTERVAL 50
#define MEASURE_WAIT_INTERVAL 20

#define OPERATION_MEASURE_BACKGROUND 1
#define OPERATION_LED_ON 2
#define OPERATION_MEASURE_COLOR 3
#define OPERATION_LED_OFF 4

#define BRIGHTNESS_MULTIPLIER 10

// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/colorview/colorview.ino
// https://de.wikipedia.org/wiki/Lab-Farbraum

#define COLOR_WOOD 0
#define COLOR_BLACK 1
#define COLOR_RED 2
#define COLOR_YELLOW 3
#define COLOR_GREEN 4
#define COLOR_BLUE 5

const float LAB_WOOD[] = {37.71, -3.81, 11.59};
const float LAB_RED[] = {20.81, 27.61, 14.62};
const float LAB_YELLOW[] = {87.59, -24.43, 59.86};
const float LAB_BLUE[] = {23.54, -6.45, -16.21};
const float LAB_GREEN[] = {18.03, -16.85, 9.40};

// TODO: LAB_BLACK

class TrackSensor {

  private:
  Adafruit_TCS34725* colorSensor;
  long nextUpdate = 0;
  uint8_t nextOperation = OPERATION_LED_ON;
  void (*colorChangedCallback)(uint8_t color) = nullptr;  
  uint16_t bg_r, bg_g, bg_b, bg_c;
  bool working = false;

  uint8_t lastColor = COLOR_WOOD;

  void readColor(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {  
    colorSensor->getRawData(r, g, b, c);    
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

    // COLOR-SENSOR LED
    pinMode(PIN_COLOR_SENSOR_LED, OUTPUT);
    digitalWrite(PIN_COLOR_SENSOR_LED, true);

    // Initialize color sensor
    if (colorSensor->begin()) {
      logger->Log("Color Sensor initialized!");
      working = true;
      
    } else {
      logger->Log("No TCS34725 found ... check your connections");      
      working = false;
    }
  }

  void Loop()
  {    

    if(millis() > nextUpdate) {
      if(!working) {
        logger->Log("ColorSensor broken!");
        nextUpdate = millis() + 1000;
        return;
      }
   

      switch(nextOperation) {
        case OPERATION_MEASURE_BACKGROUND:          
          readColor(&bg_r, &bg_g, &bg_b, &bg_c);
          
          /*
          logger->Log("Background:");
          logger->Logf("red %d", bg_r);
          logger->Logf("green %d", bg_g);
          logger->Logf("blue %d", bg_b);
          */

          nextOperation = OPERATION_LED_ON;
          nextUpdate = millis() + MEASURE_WAIT_INTERVAL;
        break;

        case OPERATION_LED_ON:
          digitalWrite(PIN_COLOR_SENSOR_LED, true);
          nextOperation = OPERATION_MEASURE_COLOR;
          nextUpdate = millis() + LED_SWITCH_WAIT_INTERVAL;
        break;

        case OPERATION_MEASURE_COLOR:
          uint16_t r, g, b, c;          
          readColor(&r, &g, &b, &c);
          
          /*
          logger->Log("Color Value:");
          logger->Logf("red %d", r);
          logger->Logf("green %d", g);
          logger->Logf("blue %d", b);
          */

          uint8_t rgb[3];
          rgb[0] = clamp((r - bg_r) * BRIGHTNESS_MULTIPLIER, 0, 255);
          rgb[1] = clamp((g - bg_g) * BRIGHTNESS_MULTIPLIER, 0, 255);
          rgb[2] = clamp((b - bg_b) * BRIGHTNESS_MULTIPLIER, 0, 255);

          /*logger->Log("Adjusted Value:");        
          logger->Logf("red %d", rgb[0]);
          logger->Logf("green %d", rgb[1]);
          logger->Logf("blue %d", rgb[2]);
          */
          float labConverted[3];
          rgb2lab(rgb, labConverted);
    
          /*logger->Log("Lab Values:");
          
          logger->Logf("%f", labConverted[0]);
          logger->Logf("%f", labConverted[1]);
          logger->Logf("%f", labConverted[2]);*/

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

          nextOperation = OPERATION_LED_OFF;
          nextUpdate = millis() + MEASURE_WAIT_INTERVAL;
        break;

        case OPERATION_LED_OFF:
          digitalWrite(PIN_COLOR_SENSOR_LED, false);
          nextOperation = OPERATION_MEASURE_BACKGROUND;
          nextUpdate = millis() + WAIT_INTERVAL;
        break;
      }
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

  delta = deltaE(lab, LAB_WOOD);  
  if(delta < deltaMin) {
    deltaMin = delta;
    color = COLOR_WOOD;
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