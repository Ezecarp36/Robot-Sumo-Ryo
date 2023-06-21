#include <Adafruit_NeoPixel.h>
 
#define PIN_LEDS 17
#define NUM_LEDS 8
#define DELAY_VAL 500   

Adafruit_NeoPixel leds(NUM_LEDS, PIN_LEDS, NEO_RGB + NEO_KHZ800);

void setup()
{
    leds.begin();
}

void loop()
{
    leds.clear();

    for(int i=0; i<NUM_LEDS; i++)
    {
      leds.setPixelColor(i, leds.Color(0,150,0));
      leds.show();
      delay(DELAY_VAL);
    }
}
