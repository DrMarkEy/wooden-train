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

#define DUTY_CYCLE_ITERATIONS 4
#define DUTY_CYCLE_LENGTH 3 * DUTY_CYCLE_ITERATIONS

#define BRIGHTNESS_OFF 0    // LED off
#define BRIGHTNESS_DIM 1    // LED on for 1 / 4 of the duty cycle
#define BRIGHTNESS_NORMAL 2 // LED on for 2 / 4 of the duty cycle
#define BRIGHTNESS_BRIGHT 3 // LED on for 4 / 4 of the duty cycle

//0bBGR12300
#define PIN_B 1
#define PIN_G 2
#define PIN_R 3
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

    byte shiftRegisterData;
    bool LED6State;

    void setColorForLED(bool* ledDutyCycle, byte brightnessR, byte brightnessG, byte brightnessB) {
      setColorComponent(ledDutyCycle, brightnessR, 0);
      setColorComponent(ledDutyCycle, brightnessG, 1);
      setColorComponent(ledDutyCycle, brightnessB, 2);
    }

    void setColorComponent(bool* ledDutyCycle, byte brightness, byte colorOffset) {
      // Initialize all entries for this color to off
      for(byte i = 0; i < DUTY_CYCLE_ITERATIONS; i++) {
        byte actualIndex = i * 3 + colorOffset;
        ledDutyCycle[actualIndex] = false;
      }

      switch(brightness) {
        case BRIGHTNESS_OFF:
          Serial.println("Setting off for color offset " + String(colorOffset));
          // Nothing to do
          break;

        case BRIGHTNESS_DIM:
          Serial.println("Setting dim for color offset " + String(colorOffset));

          ledDutyCycle[colorOffset] = true;
          break;

        case BRIGHTNESS_NORMAL:
          Serial.println("Setting normal for color offset " + String(colorOffset));

          ledDutyCycle[colorOffset] = true;
          ledDutyCycle[2 * DUTY_CYCLE_ITERATIONS + colorOffset] = true;
          break;

        case BRIGHTNESS_BRIGHT:
          Serial.println("Setting bright for color offset " + String(colorOffset));

          for(byte i = 0; i < DUTY_CYCLE_ITERATIONS; i++) {
            ledDutyCycle[i * 3 + colorOffset] = true;
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

    void setPin(byte pin, bool state) {
      if(pin == PIN_LED6) {
        LED6State = state;
      }
      else {
        // Set specific byte in shift register data
        if(state) {
          shiftRegisterData = shiftRegisterData | (0b1 << (pin - 1));
        }
        else
        {
          shiftRegisterData = shiftRegisterData & ~(0b1 << (pin - 1));
        }
      }
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

      // Set individual LEDs
      updateLED(led1, 1);
      updateLED(led2, 2);
      updateLED(led3, 3);
      updateLED(led4, 4);
      updateLED(led5, 5);
      updateLED(led6, 6);

      shiftOutData();

      dutyCyclePosition ++;
      if(dutyCyclePosition >= DUTY_CYCLE_LENGTH) {
        dutyCyclePosition = 0;
      }
    }

    void printDutyCycle(bool led[DUTY_CYCLE_LENGTH]) {
      for(byte i = 0; i < DUTY_CYCLE_LENGTH; i++) {
        Serial.print(led[i] ? "1" : "0");
      }
      Serial.println();
    }

    void printDutyCycles() {
      Serial.println("Duty cycles:");
      Serial.print("LED1: ");
      printDutyCycle(led1);
      Serial.print("LED2: ");
      printDutyCycle(led2);
      Serial.print("LED3: ");
      printDutyCycle(led3);
      Serial.print("LED4: ");
      printDutyCycle(led4);
      Serial.print("LED5: ");
      printDutyCycle(led5);
      Serial.print("LED6: ");
      printDutyCycle(led6);
    }

    void shiftOutData() {
      digitalWrite(PIN_LED_ST_CP, LOW);  // drop latch pin to GND
      shiftOut(PIN_LED_DS, PIN_LED_SH_CP, LSBFIRST, shiftRegisterData); // Write data
      digitalWrite(PIN_LED_ST_CP, HIGH); // Push data to output
    }

   public:

   Lights() {
      pinMode(PIN_LED_ST_CP, OUTPUT);
      pinMode(PIN_LED_SH_CP, OUTPUT);
      pinMode(PIN_LED_BACK_CENTER, OUTPUT);
      pinMode(PIN_LED_EN, OUTPUT);
      pinMode(PIN_LED_DS, OUTPUT);

      // Enable shift register
      digitalWrite(PIN_LED_EN, false);

   }

   /**
    * Sets a global color for all LEDs. Each color component is given on a brightness scale from 0-4.
    */
   void setGlobalColor(byte red, byte green, byte blue) {

      Serial.println("Shift-Register test ready!");

      setColorForLED(led1, red, green, blue);
      setColorForLED(led2, red, green, blue);
      setColorForLED(led3, red, green, blue);
      setColorForLED(led4, red, green, blue);
      setColorForLED(led5, red, green, blue);
      setColorForLED(led6, red, green, blue);

      printDutyCycles();
   }

   void setRailwayColorScheme(boolean forward) {
     if(forward) {
       setColorForLED(led1, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColorForLED(led2, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColorForLED(led3, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColorForLED(led4, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColorForLED(led5, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColorForLED(led6, BRIGHTNESS_OFF, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
     }
     else
     {
       setColorForLED(led1, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_BRIGHT);
       setColorForLED(led2, BRIGHTNESS_OFF, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColorForLED(led3, BRIGHTNESS_BRIGHT, BRIGHTNESS_OFF, BRIGHTNESS_OFF);
       setColorForLED(led4, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColorForLED(led5, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
       setColorForLED(led6, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT, BRIGHTNESS_BRIGHT);
     }
   }

   void Loop() {
    iterateDutyCycle();
    delayMicroseconds(50);
   }
};

#endif