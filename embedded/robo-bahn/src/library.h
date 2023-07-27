#include <Arduino.h>

void track_button_state(unsigned char buttonPin, bool* buttonStateVariable, void (*onPressed) (), void (*onReleased) ()) {
    if(!*buttonStateVariable && digitalRead(buttonPin))
   {
      *buttonStateVariable = true;

      if(onPressed != nullptr)
        onPressed();
   }
   else if(*buttonStateVariable)
   {
      *buttonStateVariable = false;

      if(onReleased != nullptr)
        onReleased();
   }
}