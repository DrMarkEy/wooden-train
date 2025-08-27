/*
  buttons.h - Watches the state of the buttons that are used to start/stop the train
  Created by Marc Mendler, August 2023
*/
#ifndef buttons_h
#define buttons_h

#include <Arduino.h>
#include <config.h>

#define DEBOUNCE_DELAY 600

static void ButtonTaskFunction (void* parameter);

static volatile bool interruptTriggered = false;
static void IRAM_ATTR ISR() {
    interruptTriggered = true;
}

class ButtonController {
  private:
  TickType_t lastButtonPressTick = 0;
  void (*buttonPressedCallback)() = nullptr;

  public:
  ButtonController(){}

  void Setup() {
  // initialize pins as an input
  pinMode(PIN_PUSH_BUTTON, INPUT);
  pinMode(PIN_REVERSE, INPUT);

  attachInterrupt(PIN_PUSH_BUTTON, ISR, RISING);

  xTaskCreatePinnedToCore(
      ButtonTaskFunction,
      "Check Button State",
      1024,
      NULL,//( void * ) &buttonController,
      1, // Priority
      NULL,
      1 // Core affinity
    );
}

  void checkButtonState()
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

} extern buttonController;

static void ButtonTaskFunction (void* parameter) {
  while(1) {
    buttonController.checkButtonState();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

#endif