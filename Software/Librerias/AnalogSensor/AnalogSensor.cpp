#include "AnalogSensor.h"

AnalogSensor::AnalogSensor(int p)
{
    pin = p;
    pinMode(pin, INPUT);
}

float AnalogSensor::SensorRead()
{
    long sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum = sum + analogRead(pin);
    }
    float read = sum / n;
    return (read);
}
