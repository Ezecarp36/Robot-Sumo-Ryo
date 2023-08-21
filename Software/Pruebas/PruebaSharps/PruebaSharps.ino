
#include <DistanceSensors.h>
#include "BluetoothSerial.h"

#define PIN_SENSOR_DERECHO 25
#define PIN_SENSOR_IZQUIERDO 35
float distancia_derecha;
float distancia_izquierda;

unsigned long currentTime = 0;
#define TICK_DEBUG 1000

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
Isensor *sharp_derecha = new Sharp_GP2Y0A02(PIN_SENSOR_DERECHO);
Isensor *sharp_izquierda = new Sharp_GP2Y0A02(PIN_SENSOR_IZQUIERDO);
void setup() {
  Serial.begin(9600);
  SerialBT.begin("Sami");
}

void loop() {
  
  distancia_derecha = sharp_derecha->SensorRead();
  distancia_izquierda = sharp_izquierda->SensorRead();

  if (millis() > currentTime + TICK_DEBUG)
  {
    SerialBT.print("Distancia derecha: ");
    SerialBT.println(distancia_derecha);
    SerialBT.print("Distancia Izquierda: ");
    SerialBT.println(distancia_izquierda);
  }
}
