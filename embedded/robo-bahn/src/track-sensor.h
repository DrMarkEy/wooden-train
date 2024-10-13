/*
  track-sensor.h - Controls the track sensor of the train (color sensor and magnetic sensor)
  Created by Marc Mendler, Oktober 2023
*/
#ifndef tracksensor_h
#define tracksensor_h

#include <Arduino.h>

#include <color-sensor.h>
#include <http-logger.h>
#include <library.h>

class TrackSensor;
extern TrackSensor* trackSensor;

class TrackSensor {

  private:
  ColorSensor* colorSensor;
  void (*colorChangedCallback)(uint8_t color) = nullptr;  
  bool working = false;
  long nextBrokenUpdate = 0;

  uint8_t lastColor = COLOR_WOOD;

  void readMagneticField() {

    // Hall-Sensor-Test
    /*Serial.print("Left: ");
    Serial.println(analogRead(2));

    Serial.print("Right: ");
    Serial.println(analogRead(15));*/
  }

  void colorMeasurement(uint8_t classifiedColor) {
    if(classifiedColor != lastColor) {
      logger->Logf("Detected color %d", classifiedColor);

      // Only white followed by a color (other than black and wood) is a signal
      if(lastColor == COLOR_WHITE && classifiedColor != COLOR_BLACK && classifiedColor != COLOR_WOOD) {          
        logger->Logf("Detected color signal %d", classifiedColor);
        
        if(colorChangedCallback != nullptr) {
          colorChangedCallback(classifiedColor);
        }
      }
      
      lastColor = classifiedColor;        
    }
  }

  public:

  TrackSensor()
  {    
    colorSensor = new ColorSensor(PIN_COLOR_SENSOR_INTERRUPT, PIN_COLOR_SENSOR_LED);
    colorSensor->onMeasurement([](uint8_t color) {
      trackSensor->colorMeasurement(color);
    });
  }

  void Loop()
  {    
    colorSensor->Loop();
  }

  void onColorChangeDetected(void (*_callback) (uint8_t color))
  {		
    this->colorChangedCallback = _callback;	
  }
};
#endif