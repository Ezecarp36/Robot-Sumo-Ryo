#include <DistanceSensors.h>

#define SENSOR_PIN 13
double distance;

unsigned long currentTime = 0;
#define TICK_DEBUG 500

Isensor *sharp_GP2Y0A21 = new Sharp_GP2Y0A21(SENSOR_PIN);
void setup() {
  Serial.begin(9600);
}

void loop() {
  distance = sharp_GP2Y0A21->SensorRead();

  if (millis() > currentTime + TICK_DEBUG)
  {
    Serial.print("Distance: ");
    Serial.println(distance);
  }
}
