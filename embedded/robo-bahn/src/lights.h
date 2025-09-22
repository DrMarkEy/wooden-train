/*
  lights.h - Controls the headlights of the train
  Created by Marc Mendler, November 2023
*/
#ifndef lights_h
#define lights_h

#include <Arduino.h>
#include <config.h>
#include <http-logger.h>

#define BRIGHTNESS_CORRECTION_RED 1
#define BRIGHTNESS_CORRECTION_GREEN 1
#define BRIGHTNESS_CORRECTION_BLUE 1

#define DUTY_CYCLE_ITERATIONS 4
#define DUTY_CYCLE_LENGTH 3 * DUTY_CYCLE_ITERATIONS

#define BRIGHTNESS_OFF 0    // LED off
#define BRIGHTNESS_DIM 1    // LED on for 1 / 4 of the duty cycle
#define BRIGHTNESS_NORMAL 2 // LED on for 2 / 4 of the duty cycle
#define BRIGHTNESS_BRIGHT 3 // LED on for 4 / 4 of the duty cycle

// TODO: ADJUST PIN NUMBERS
#define PIN_R 1
#define PIN_G 2
#define PIN_B 3
#define PIN_LED1 4
#define PIN_LED2 5
#define PIN_LED3 6
#define PIN_LED4 7
#define PIN_LED5 8
#define PIN_LED6 9

// In Fahrtrichtung
// 1: vorne rechts
// 2: vorne mitte
// 3: vorne links
// 4: hinten links
// 5: hinten rechts
// 6: hinten mitte (separate pin)

class Lights
{
  /*
  Moves through a duty cycle of 12 steps (4 per color).
  Each LED can be set to on or off in each step.

  */


   private:
    byte dutyCyclePosition = 0;

    bool led1[DUTY_CYCLE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 12 entries (4 per color), Format: rgbrgbrgbrgb
    bool led2[DUTY_CYCLE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool led3[DUTY_CYCLE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool led4[DUTY_CYCLE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool led5[DUTY_CYCLE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool led6[DUTY_CYCLE_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    void setColor(bool* ledDutyCycle, byte brightnessR, byte brightnessG, byte brightnessB) {
      setColorComponent(ledDutyCycle, brightnessR, 0);
      setColorComponent(ledDutyCycle, brightnessG, 1);
      setColorComponent(ledDutyCycle, brightnessB, 2);
    }

    void setColorComponent(bool* ledDutyCycle, byte brightness, byte colorOffset) {
      // Initialize all entries for this color to off
      for(byte i = 0; i < DUTY_CYCLE_ITERATIONS; i++) {
        ledDutyCycle[colorOffset + i * 3] = false;
      }

      switch(brightness) {
        case BRIGHTNESS_OFF:
          // Nothing to do
          break;

        case BRIGHTNESS_DIM:
          ledDutyCycle[colorOffset] = true;
          break;

        case BRIGHTNESS_NORMAL:
          ledDutyCycle[colorOffset] = true;
          ledDutyCycle[2 * DUTY_CYCLE_ITERATIONS + colorOffset] = true;
          break;

        case BRIGHTNESS_BRIGHT:
          for(byte i = 0; i < DUTY_CYCLE_ITERATIONS; i++) {
            ledDutyCycle[i * DUTY_CYCLE_ITERATIONS + colorOffset] = true;
          }
          break;
      }
    }

    void updateLED(bool* ledDutyCycle, byte ledPin) {
      if(ledDutyCycle[dutyCyclePosition]) {
        setPin(ledPin, false); // Inverted logic
      }
      else
      {
        setPin(ledPin, true); // Inverted logic
      }
    }

    void setPin(byte pin, bool high) {
      // TODO: Set specific pin high or low
    }

    void iterateDutyCycle() {
      // Set LED colors
      if(dutyCyclePosition % 3 == 0) {
        // Light red LEDs
        setPin(PIN_R, true);
        setPin(PIN_G, false);
        setPin(PIN_B, false);
      }
      else if(dutyCyclePosition % 3 == 1) {
        // Light green LEDs
        setPin(PIN_R, false);
        setPin(PIN_G, true);
        setPin(PIN_B, false);
      }
      else {
        // Light blue LEDs
        setPin(PIN_R, false);
        setPin(PIN_G, false);
        setPin(PIN_B, true);
      }

      // Set LED positions
      updateLED(led1, 1);
      updateLED(led2, 2);
      updateLED(led3, 3);
      updateLED(led4, 4);
      updateLED(led5, 5);
      updateLED(led6, 6);

      dutyCyclePosition ++;
    }

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
      /*logger.Log("L: ");

      if(r)
      logger.Log("r");

      if(g)
      logger.Log("g");

      if(b)
      logger.Log("b");

      logger.Log(":");
      logger.Log(ledMask);

*/

      // rgb01234
      byte data = 0b00011111;
      if(r)
        data = data | 0b10000000;
      if(g)
        data = data | 0b01000000;
      if(b)
        data = data | 0b00100000;

      // ledMask: 012345
      // data: rgb01234
      // Letztes Bit abschneiden
      data ^= ledMask >> 1;

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
     /*byte r[6];
     byte g[6];
     byte b[6];
     for(byte i = 0; i < 6; i++) {
      r[i] = red;
      g[i] = green;
      b[i] = blue;
     }
     buildDutyCycle(r, g, b);*/
   }

   void setRailwayColorScheme(boolean forward) {
     if(forward) {
       setColor(led1, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColor(led2, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColor(led3, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColor(led4, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColor(led5, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColor(led6, BRIGHTNESS_OFF, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
     }
     else
     {
       setColor(led1, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_BRIGHT);
       setColor(led2, BRIGHTNESS_OFF, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColor(led3, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColor(led4, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColor(led5, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColor(led6, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
     }

   }

   void Loop() {

      dutyCycleStep(dutyCyclePosition);
      dutyCyclePosition ++;
      if(dutyCyclePosition >= DUTY_CYCLE_LENGTH) {
        dutyCyclePosition = 0;
      }

      delayMicroseconds(50);
//      activateLEDs(true, true, false, 0b1);


     if(dutyCyclePosition % 2 == 0) {
       digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
       shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, 0); // Write data
       digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output
     }

   }


};

#endif