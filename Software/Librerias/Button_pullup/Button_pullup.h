#ifndef _BUTTON_PULLUP_H
#define _BUTTON_PULLUP_H
#include "Arduino.h"

class Button_pullup
{
private:
    int pin;
    bool flank = HIGH;
    bool previousState = !flank;

public:
    Button_pullup(int p);
    void SetFlank(bool f);
    bool GetIsPress();
};

#endif