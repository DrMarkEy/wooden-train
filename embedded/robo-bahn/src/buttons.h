/*
  buttons.h - Watches the state of the buttons that are used to start/stop the train
  Created by Marc Mendler, August 2023
*/
#ifndef buttons_h
#define buttons_h

#include <Arduino.h>
#include <config.h>

#define DEBOUNCE_DELAY 600

static bool interruptTriggered = false;
static void IRAM_ATTR ISR() {
    interruptTriggered = true;
}

class ButtonController {
  private:
  bool lastButtonPressTick = 0;
  void (*buttonPressedCallback)() = nullptr;

  public:
  ButtonController(){}
  void Setup();
  void Loop();
  void onButtonPressed(void (*_callback) ());
  bool isReversed();

} extern buttonController;
#endif