#ifndef _ANALOG_SENSOR_H
#define _ANALOG_SENSOR_H
#include "Arduino.h"

class AnalogSensor
{
private:
    int pin;
    int n = 10;

public:
    AnalogSensor(int p);
    float SensorRead();
};
#endif