#include <Button.h>
#include "BluetoothSerial.h"

#define PIN_BUTTON 23
unsigned long currentTimeButton = 0;
#define TICK_START 1000

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
Button *start = new  Button(PIN_BUTTON);

void setup() 
{
  SerialBT.begin("RYO");
}

void loop() 
{if (millis() > currentTimeButton + TICK_START)
  {
    if(start->GetIsPress()) SerialBT.println("Press");
  }
}
