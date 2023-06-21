#ifndef _ENGINE_CONTROLLER_H
#define _ENGINE_CONTROLLER_H
#include "arduino.h"

class IEngine
{
public:
    IEngine() = default;
    virtual void Forward();
    virtual void Backward();
    virtual void Stop();
    virtual void SetSpeed(int sp);
};

class Driver_DRV8825 : public IEngine
{

private:
    int speed = 200;
    const int frequency = 1000;
    int channelA;
    int channelB;
    const int resolution = 8;
    int pinPwmA;
    int pinPwmB;

public:
    Driver_DRV8825(int pinA, int pinB, int chA, int chB);
    void Forward();
    void Backward();
    void Stop();
    void SetSpeed(int sp);
};

class Driver_G2_18V17 : public IEngine
{
private:
    int speed = 200;
    const int frequency = 1000;
    int channel;
    const int resolution = 8;
    int pinDir;
    int pinPwm;

public:
    Driver_G2_18V17(int dir, int pwm, int ch);
    void Forward();
    void Backward();
    void Stop();
    void SetSpeed(int sp);
};

class Driver_LN298N : public IEngine
{
private:
    int pinA;
    int pinB;
    int pinPwm;
    const int frequency = 1000;
    int channel;
    const int resolution = 8;
    int speed = 200;

public:
    Driver_LN298N(int in1, int in2, int ena, int ch);
    void Forward();
    void Backward();
    void Stop();
    void SetSpeed(int sp);
};

class EngineController
{
private:
    IEngine *rightEngine;
    IEngine *leftEngine;
    int rightEngineSpeed;
    int leftEngineSpeed;

public:
    EngineController(IEngine *rightEng, IEngine *leftEng);
    void Forward(int rightSpeed, int leftSpeed = 0);
    void Backward(int rightSpeed, int leftSpeed = 0);
    void Left(int rightSpeed, int leftSpeed = 0);
    void Right(int rightSpeed, int leftSpeed = 0);
    void Stop();
};

#endif