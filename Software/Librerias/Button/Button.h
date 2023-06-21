#ifndef _BUTTON_H
#define _BUTTON_H
#include "Arduino.h"

class Button
{
private:
    int pin;
    bool flank = HIGH;
    bool previousState = !flank;

public:
    Button(int p);
    void SetFlank(bool f);
    bool GetIsPress();
};

#endif