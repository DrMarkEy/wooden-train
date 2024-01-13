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

#define DUTY_CYCLE_LENGTH 45

class Lights
{

   private:
     byte dutyCyclePosition = 0;
     byte red = 4;
     byte green = 2;     
     byte blue = 9;
     byte dutyCycle[DUTY_CYCLE_LENGTH] = {red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue, red, green, blue}; // 15 entries for each color, Format: rgbrgbrgbrgbrgb... 
     // Only 6 Bit of every entry are used to determine which LEDs should be lit during this duty cycle

   //byte brightness = 100;
  // byte red[6];
  // byte green[6];
  // byte blue[6];

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

    // Farben: 1 = an
    // Zahlen: 0 = an

    // In Fahrtrichtung // Zahlen: 1 = aus
    // 0: vorne rechts 
    // 1: vorne mitte
    // 2: vorne links
    // 3: hinten links
    // 4: hinten rechts

    // Blau ist vorne kaputt (Hardware issue!)

     digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
     shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, data); // Write data
     digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output
   }

   void activateLEDs(bool r, bool g, bool b, byte ledMask) {
      // rgb01234
      byte data = 0b00011111;
      if(r)
        data = data | 0b10000000;
      if(g)
        data = data | 0b01000000;
      if(b)
        data = data | 0b00100000;
            
      // ledMask: 012345
      data ^= (ledMask >> 1);      
      
      if((ledMask & 0b1) == 0b1) {
        digitalWrite(PIN_LED_BACK_CENTER, LOW);
      }
      else
      {
        digitalWrite(PIN_LED_BACK_CENTER, HIGH);
      }

    // Farben: 1 = an
    // Zahlen: 0 = an

    // In Fahrtrichtung // Zahlen: 1 = aus
    // 0: vorne rechts 
    // 1: vorne mitte
    // 2: vorne links
    // 3: hinten links
    // 4: hinten rechts

    // Blau ist vorne kaputt (Hardware issue!)

     digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
     shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, data); // Write data
     digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output
   }

  
  const byte pattern[15][15] = {{0}, {0, 7}, {0, 5, 10}, {0, 3, 7, 10}, {0, 3, 6, 9, 12}, {0, 2, 5, 7, 10, 12}, {0, 2, 4, 6, 8, 10, 12}, {0, 2, 4, 5, 6, 8, 10, 12}, {0, 1, 2, 4, 5, 6, 8, 10, 11, 12}, {0, 1, 2, 4, 5, 6, 7, 8, 10, 11, 12}, {0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12}, {0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}};

  // idea: Always call this function once when any color is changed
  // build a byte array of the complete duty cycle to light all leds
  // alternate the current duty cycle value with 0 to reduce brightness globally,

  // Currently: Simplest version: go through all leds in order and set their color
  // TODO: Better version: Group leds with the exact same color
  // TODO: Even better version: Reuse duty cycle entries of the same color between all effected leds
   void buildDutyCycle(byte r[6], byte g[6], byte b[6]) {
    
    // Initialize all leds as off
    for(byte i = 0; i < DUTY_CYCLE_LENGTH; i++) {
      dutyCycle[i] = 0;
    }

    for(byte i = 0; i < 6; i++) {
      setDutyCycleLevel(r[i], 0);
      setDutyCycleLevel(g[i], 1);
      setDutyCycleLevel(b[i], 2);
    }
    
     // TODO: Apply color correction!
   }

   void setDutyCycleLevel(byte colorValue, byte colorOffset) {
      byte rLevel = colorValue / 17;
      
      for(byte s = 0; s < rLevel; s++) {
        dutyCycle[pattern[rLevel][s] * colorOffset] = 1;
      }
   }

   void dutyCycleStep(byte i) {
      byte ledMask = dutyCycle[i];
      activateLEDs(i % 3 == 0, i % 3 == 1, i % 3 == 2, ledMask);
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
     //brightness = _brightness;
     //analogWrite(PIN_LED_EN, 255-brightness);
   }

   void setGlobalColor(byte red, byte green, byte blue) {
     byte r[6];    
     byte g[6];
     byte b[6];
     for(byte i = 0; i < 6; i++) {
      r[i] = red;
      g[i] = green;
      b[i] = blue;
     }
     //buildDutyCycle(r, g, b);
   }

   void setDirectionColor(boolean forward) {                
        /*red[i] = r;
        green[i] = g;
        blue[i] = b;*/ // TODO!
   }
   
   void Loop() {
      
      dutyCycleStep(dutyCyclePosition);
      dutyCyclePosition ++;
      if(dutyCyclePosition == DUTY_CYCLE_LENGTH) {
        dutyCyclePosition = 0;
      }

      //activateLEDs(true, true, false, 0b111111);
      
      

     digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
     shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, 0); // Write data
     digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output

     
   }   


};

#endif