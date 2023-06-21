#include <DistanceSensors.h>
#include "BluetoothSerial.h"

#define PIN_SENSOR_RIGHT 27
#define PIN_SENSOR_LEFT 35
int leftDistance;
int rightDistance;

unsigned long currentTime = 0;
#define TICK_DEBUG 1000

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

Isensor *rightSensor = new Sharp_GP2Y0A60S(PIN_SENSOR_RIGHT);
Isensor *leftSensor = new Sharp_GP2Y0A60S(PIN_SENSOR_LEFT);

void setup()
{
    SerialBT.begin("Sami");
}

void loop()
{
    rightDistance = rightSensor->SensorRead();
    leftDistance = leftSensor->SensorRead();

    if (millis() > currentTime + TICK_DEBUG)
    {
      SerialBT.print("RightDistance: ");
      SerialBT.print(rightDistance);
      SerialBT.print(" // LeftDistance: ");
      SerialBT.println(leftDistance);
    }
}
