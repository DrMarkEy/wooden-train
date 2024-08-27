/*
  buttons.h - Watches the state of the buttons that are used to start/stop the train
  Created by Marc Mendler, August 2023
*/
#ifndef buttons_h
#define buttons_h

#include <Arduino.h>
#include <config.h>

class ButtonController {

  public:

  ButtonController()
  {
    // initialize pins as an input
    pinMode(PIN_PUSH_BUTTON, INPUT);
    pinMode(PIN_REVERSE, INPUT);
    
  }

  void Loop()
  {
    track_button_state(PIN_PUSH_BUTTON, &start_button_pressed, this->buttonPressedCallback, this->buttonReleasedCallback);
  }

  void onButtonPressed(void (*_callback) ())
  {		
    this->buttonPressedCallback = _callback;	
  }

  void onButtonReleased(void (*_callback) ())
  {		
    this->buttonReleasedCallback = _callback;	
  }

  bool isReversed()
  {
    return !digitalRead(PIN_REVERSE);
  }

  private:

  bool start_button_pressed = false;
  void (*buttonPressedCallback)() = nullptr;
  void (*buttonReleasedCallback)() = nullptr;

  void track_button_state(unsigned char buttonPin, bool* buttonStateVariable, void (*onPressed) (), void (*onReleased) ()) {
   if(!*buttonStateVariable && digitalRead(buttonPin))
   {
      *buttonStateVariable = true;

      if(onPressed != nullptr)
        onPressed();
   }
   else if(*buttonStateVariable && !digitalRead(buttonPin))
   {
      *buttonStateVariable = false;

      if(onReleased != nullptr)
        onReleased();
   }
  }
};
#endif