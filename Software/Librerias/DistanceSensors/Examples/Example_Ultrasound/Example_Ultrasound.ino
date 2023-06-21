#include <DistanceSensors.h>

#define PIN_ECHO  11
#define PIN_TRIGG 12
double distance;

unsigned long currentTime = 0;
#define TICK_DEBUG 500

Isensor *ultrasound = new Ultrasound(PIN_TRIGG, PIN_ECHO);
void setup() {
  Serial.begin(9600);
}

void loop() {
  distance = ultrasound->SensorRead();

  if (millis() > currentTime + TICK_DEBUG)
  {
    Serial.print("Distance: ");
    Serial.println(distance);
  }
}
