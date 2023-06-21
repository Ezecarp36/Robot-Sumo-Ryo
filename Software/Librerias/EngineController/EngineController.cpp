#include "EngineController.h"

Driver_DRV8825::Driver_DRV8825(int pinA, int pinB, int chA, int chB)
{
    pinPwmA = pinA;
    pinPwmB = pinB;
    channelA = chA;
    channelB = chB;
    pinMode(pinPwmA, OUTPUT);
    pinMode(pinPwmB, OUTPUT);
    ledcSetup(channelA, frequency, resolution);
    ledcSetup(channelB, frequency, resolution);
    ledcAttachPin(pinPwmA, channelA);
    ledcAttachPin(pinPwmB, channelB);
}

void Driver_DRV8825::Forward()
{
    ledcWrite(channelA, speed);
    ledcWrite(channelB, 0);
}

void Driver_DRV8825::Backward()
{
    ledcWrite(channelA, 0);
    ledcWrite(channelB, speed);
}

void Driver_DRV8825::Stop()
{
    ledcWrite(channelA, 0);
    ledcWrite(channelB, 0);
}

void Driver_DRV8825::SetSpeed(int sp)
{
    speed = sp;
}

Driver_G2_18V17::Driver_G2_18V17(int dir, int pwm, int ch)
{
    pinDir = dir;
    pinPwm = pwm;
    channel = ch;
    pinMode(pinDir, OUTPUT);
    pinMode(pinPwm, OUTPUT);
    ledcSetup(channel, frequency, resolution);
    ledcAttachPin(pinPwm, channel);
}

void Driver_G2_18V17::Forward()
{
    digitalWrite(pinDir, 1);
    ledcWrite(channel, speed);
}

void Driver_G2_18V17::Backward()
{
    digitalWrite(pinDir, 0);
    ledcWrite(channel, speed);
}

void Driver_G2_18V17::Stop()
{
    digitalWrite(pinDir, 0);
    ledcWrite(channel, 0);
}

void Driver_G2_18V17::SetSpeed(int sp)
{
    speed = sp;
}

Driver_LN298N::Driver_LN298N(int in1, int in2, int ena, int ch)
{
    pinA = in1;
    pinB = in2;
    pinPwm = ena;
    channel = ch;
    ledcSetup(channel, frequency, resolution);
    ledcAttachPin(pinPwm, channel);
}

void Driver_LN298N::Forward()
{
    digitalWrite(pinA, 1);
    digitalWrite(pinA, 0);
    ledcWrite(channel, speed);
}

void Driver_LN298N::Backward()
{
    digitalWrite(pinA, 0);
    digitalWrite(pinA, 1);
    ledcWrite(channel, speed);
}

void Driver_LN298N::Stop()
{
    digitalWrite(pinA, 0);
    digitalWrite(pinA, 0);
    ledcWrite(channel, 0);
}

void Driver_LN298N::SetSpeed(int sp)
{
    speed = sp;
}

EngineController::EngineController(IEngine *rightEng, IEngine *leftEng)
{
    this->rightEngine = rightEng;
    this->leftEngine = leftEng;
}

void EngineController::Forward(int rightSpeed, int leftSpeed)
{
    rightEngineSpeed = rightSpeed;
    leftSpeed == 0 ? leftEngineSpeed = rightEngineSpeed : leftEngineSpeed = leftSpeed;

    rightEngine->SetSpeed(rightEngineSpeed);
    leftEngine->SetSpeed(leftEngineSpeed);
    rightEngine->Forward();
    leftEngine->Forward();
}

void EngineController::Backward(int rightSpeed, int leftSpeed)
{
    rightEngineSpeed = rightSpeed;
    leftSpeed == 0 ? leftEngineSpeed = rightEngineSpeed : leftEngineSpeed = leftSpeed;

    rightEngine->SetSpeed(rightEngineSpeed);
    leftEngine->SetSpeed(leftEngineSpeed);
    rightEngine->Backward();
    leftEngine->Backward();
}

void EngineController::Right(int rightSpeed, int leftSpeed)
{
    rightEngineSpeed = rightSpeed;
    leftSpeed == 0 ? leftEngineSpeed = rightEngineSpeed : leftEngineSpeed = leftSpeed;

    rightEngine->SetSpeed(rightEngineSpeed);
    leftEngine->SetSpeed(leftEngineSpeed);
    rightEngine->Backward();
    leftEngine->Forward();
}

void EngineController::Left(int rightSpeed, int leftSpeed)
{
    rightEngineSpeed = rightSpeed;
    leftSpeed == 0 ? leftEngineSpeed = rightEngineSpeed : leftEngineSpeed = leftSpeed;

    rightEngine->SetSpeed(rightEngineSpeed);
    leftEngine->SetSpeed(leftEngineSpeed);
    rightEngine->Forward();
    leftEngine->Backward();
}

void EngineController::Stop()
{
    rightEngine->Stop();
    leftEngine->Stop();
}
