/*
  lights.h - Controls the headlights of the train
  Created by Marc Mendler, November 2023
*/
#ifndef lights_h
#define lights_h

#include <Arduino.h>
#include <config.h>

#define BRIGHTNESS_CORRECTION_RED 1
#define BRIGHTNESS_CORRECTION_GREEN 1
#define BRIGHTNESS_CORRECTION_BLUE 1

#define DUTY_CYCLE_LENGTH 9

class Lights
{

   private:
   byte brightness = 100;
   byte red[6];
   byte green[6];
   byte blue[6];

   byte nextLed = 0;
   long lastUpdate = 0;

   void activateLED(byte led, bool r, bool g, bool b) {
      // rgb01234
      byte data = 0b00011111;
      if(r)
        data = data | 0b10000000;
      if(g)
        data = data | 0b01000000;
      if(b)
        data = data | 0b00100000;
      
      if(led < 5) {
        data -= (0b10000 >> led);
        digitalWrite(PIN_LED_BACK_CENTER, HIGH);
      }
      else if(led == 5) {
        digitalWrite(PIN_LED_BACK_CENTER, LOW);
      }

//     byte data = 0b11000001;

    // Farben: 1 = an


    // In Fahrtrichtung // Zahlen: 1 = aus
    // 0: vorne rechts 
    // 1: vorne mitte
    // 2: vorne links
    // 3: hinten links
    // 4: hinten rechts

    // Blau ist vorne kaputt

     //


     digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
     shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, data); // Write data
     digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output
   }

   // returns: duty cycle iterations
   int activateLEDWithColor(byte led, byte r, byte g, byte b) {
     return 0;
   }

   public:

   Lights() {
      pinMode(PIN_LED_ST_CP, OUTPUT);
      pinMode(PIN_LED_SH_CP, OUTPUT);
      pinMode(PIN_LED_BACK_CENTER, OUTPUT);
      pinMode(PIN_LED_EN, OUTPUT);
      pinMode(PIN_LED_DS, OUTPUT);

      // Set initial brightness      
      //analogWrite(PIN_LED_EN, 255-brightness);
      digitalWrite(PIN_LED_EN, false); // false: enable

   }

   void setBrightness(byte _brightness) {
     brightness = _brightness;
     //analogWrite(PIN_LED_EN, 255-brightness);
   }

   void setGlobalColor(byte r, byte g, byte b) {
     for(byte i = 0; i < 6; i++) {
        red[i] = r;
        green[i] = g;
        blue[i] = b;
     }

   }

   void setDirectionColor(boolean forward) {                
        /*red[i] = r;
        green[i] = g;
        blue[i] = b;*/ // TODO!
   }

   // Go through all 6 LED positions
   // Set the right color(s) while each LED is displayed
   // control brightness via the length that each LED is active
   // TODO: activate leds with the same color simultaneously??

   
   void Loop() {
      /*nextLed++;

      if(nextLed == 256)
        nextLed = 0;*/

      activateLED(5, false, true, false);
      delay(5);

     digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
     shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, 0); // Write data
     digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output

     delay(5);
   }   


};

#endif