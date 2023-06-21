#ifndef _SHARP_H
#define _SHARP_H
#include "Arduino.h"

class Isensor
{
public:
    Isensor() = default;
    virtual double SensorRead();
    float AnalogReading(int pin);
};

class Sharp_GP2Y0A60S : public Isensor
{
private:
    int sensorPin;

public:
    Sharp_GP2Y0A60S(int pin);
    double SensorRead();
};

class Sharp_GP2Y0A21 : public Isensor
{
private:
    int sensorPin;

public:
    Sharp_GP2Y0A21(int pin);
    double SensorRead();
};

class Sharp_GP2Y0A02 : public Isensor
{
private:
    int sensorPin;

public:
    Sharp_GP2Y0A02(int pin);
    double SensorRead();
};

class Ultrasound : public Isensor
{
private:
    int pinTrig;
    int pinEcho;

public:
    Ultrasound(int trig, int echo);
    double SensorRead();
};

#endif