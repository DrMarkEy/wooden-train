#include <buttons.h>

void ButtonController::Setup() {
// initialize pins as an input
pinMode(PIN_PUSH_BUTTON, INPUT);
pinMode(PIN_REVERSE, INPUT);

attachInterrupt(PIN_PUSH_BUTTON, ISR, RISING);
}

void ButtonController::Loop()
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

void ButtonController::onButtonPressed(void (*_callback) ())
{		
this->buttonPressedCallback = _callback;	
}

bool ButtonController::isReversed()
{
return !digitalRead(PIN_REVERSE);
}