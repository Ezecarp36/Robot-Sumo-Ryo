#include "DistanceSensors.h"

float Isensor::AnalogReading(int pin)
{
    int n = 3;
    long sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum = sum + analogRead(pin);
    }
    float adc = sum / n;
    return adc;
}

Sharp_GP2Y0A60S::Sharp_GP2Y0A60S(int pin)
{
    sensorPin = pin;
    pinMode(sensorPin, INPUT);
}

double Sharp_GP2Y0A60S::SensorRead()
{
    float adc = AnalogReading(sensorPin);
    double distance = 187754 * pow(adc, -1.183);
    delay(100);
    return distance;
}

Sharp_GP2Y0A21::Sharp_GP2Y0A21(int pin)
{
    sensorPin = pin;
    pinMode(sensorPin, INPUT);
}

double Sharp_GP2Y0A21::SensorRead()
{
    float adc = AnalogReading(sensorPin);
    float distance = 11945.0 / (adc - 11.0);
    delay(100);
    return distance;
}

Sharp_GP2Y0A02::Sharp_GP2Y0A02(int pin)
{
    sensorPin = pin;
    pinMode(sensorPin, INPUT);
}

double Sharp_GP2Y0A02::SensorRead()
{
    float adc = AnalogReading(sensorPin);
    if (adc > 2700 ) adc = 2700;
    if(adc < 500 ) adc = 500;
    float distance = 10650.08*(pow(adc , -0.74999))+1; //254000 *(pow( adc , -1.2134));
    delay(100);
    return distance;
}

Ultrasound::Ultrasound(int trig, int echo)
{
    pinTrig = trig;
    pinEcho = echo;
    pinMode(pinEcho, INPUT);
    pinMode(pinTrig, OUTPUT);
    digitalWrite(pinTrig, LOW);
}

double Ultrasound::SensorRead()
{
    long distance;
    long pulse;
    // SENSOR
    digitalWrite(pinTrig, HIGH);
    delayMicroseconds(10); // Enviamos un pulso de 10us
    digitalWrite(pinTrig, LOW);
    pulse = pulseIn(pinEcho, HIGH);
    distance = pulse / 58.2;
    return distance;
}
