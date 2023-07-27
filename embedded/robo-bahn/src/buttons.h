#include <Arduino.h>
#include <config.h>
#include <library.h>

void initialize_buttons()
{
    // initialize pins as an input
    pinMode(PIN_PUSH_BUTTON, INPUT);
    pinMode(PIN_REVERSE, INPUT);
    
}

bool is_reversed()
{
    return digitalRead(PIN_REVERSE);
}

bool start_button_pressed = false;
void on_button_pressed() 
{
    if(is_reversed())
    {
        engine_drive_backward();
    }    
    else
    {
        engine_drive_forward();
    }
}

void update_button_states()
{
   track_button_state(PIN_PUSH_BUTTON, &start_button_pressed, &on_button_pressed, nullptr);
}