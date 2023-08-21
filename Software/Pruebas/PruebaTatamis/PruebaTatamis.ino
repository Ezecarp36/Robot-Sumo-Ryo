#include <AnalogSensor.h>
#include "BluetoothSerial.h"


#define PIN_SENSOR_TATAMI_RIGHT 34
#define PIN_SENSOR_TATAMI_LEFT 13

int LeftTatamiReading;
int RightTatamiReading;

unsigned long currentTime = 0;
#define TICK_DEBUG 100

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

AnalogSensor *rightTatami = new AnalogSensor(PIN_SENSOR_TATAMI_RIGHT);
AnalogSensor *leftTatami = new AnalogSensor(PIN_SENSOR_TATAMI_LEFT);

void setup()
{
    SerialBT.begin("Sami");
}

void loop()
{
    LeftTatamiReading = leftTatami->SensorRead();
    RightTatamiReading = rightTatami->SensorRead();

    if (millis() > currentTime + TICK_DEBUG)
    {
        //SerialBT.print("RightTatami: ");
        //SerialBT.print(RightTatamiReading);
        SerialBT.print(" // LeftTatami: "); 
        SerialBT.println(LeftTatamiReading);
    }
}