/*
  buttons.h - Watches the state of the buttons that are used to start/stop the train
  Created by Marc Mendler, August 2023
*/
#ifndef buttons_h
#define buttons_h

#include <Arduino.h>
#include <config.h>

#define DEBOUNCE_DELAY 600

bool interruptTriggered = false;
void IRAM_ATTR ISR() {
    interruptTriggered = true;
}

class ButtonController {
  private:
  bool lastButtonPressTick = 0;
  void (*buttonPressedCallback)() = nullptr;

  public:
  ButtonController()
  {
    // initialize pins as an input
    pinMode(PIN_PUSH_BUTTON, INPUT);
    pinMode(PIN_REVERSE, INPUT);
    
    attachInterrupt(PIN_PUSH_BUTTON, ISR, RISING);
  }

  void Loop()
  {
    if(interruptTriggered) {
      // Debounce Button Press
      if(xTaskGetTickCount() - lastButtonPressTick >  DEBOUNCE_DELAY / portTICK_PERIOD_MS) {
        lastButtonPressTick = xTaskGetTickCount();

        if(buttonPressedCallback != nullptr)
          buttonPressedCallback();
      }

      interruptTriggered = false;
    }
  }

  void onButtonPressed(void (*_callback) ())
  {		
    this->buttonPressedCallback = _callback;	
  }

  bool isReversed()
  {
    return !digitalRead(PIN_REVERSE);
  }
};
#endif