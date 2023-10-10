#include "Button_pullup.h"

Button::Button(int p)
{
    pin = p;

    pinMode(pin, INPUT_PULLUP);
}

void Button::SetFlank(bool f)
{
    flank = f;
    previousState = !flank;
}

bool Button::GetIsPress()
{
    bool actualState = digitalRead(pin);
    bool state = (previousState != actualState) && actualState == flank;
    previousState = actualState;
    delay(100);
    return state;
}
