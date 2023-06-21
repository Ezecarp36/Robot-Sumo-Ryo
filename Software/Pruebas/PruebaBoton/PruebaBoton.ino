#include <Button.h>
#include <Adafruit_NeoPixel.h>

#define PIN_LEDS 17
#define NUM_LEDS 8

#define PIN_BUTTON 18
unsigned long currentTimeButton = 0;
#define TICK_START 1000


Button *start = new  Button(PIN_BUTTON);
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LEDS, NEO_RGB + NEO_KHZ800);

enum menu
{
  MAIN_MENU,
  OPCION_1,
  OPCION_2,
  OPCION_3
};
int menu = MAIN_MENU;

void Menu()
{
  switch(menu)
  {
    case MAIN_MENU:
    {
      leds.clear();
      for(int i=0; i<NUM_LEDS; i++)
      {
        leds.setPixelColor(i, leds.Color(150,0,0));
        leds.show();
      }
      leds.show();
      if(start->GetIsPress()) menu = OPCION_1;
      break;
    }

    case OPCION_1:
    {
      leds.clear();
      for(int i=0; i<3; i++)
      {
        leds.setPixelColor(i, leds.Color(0,150,0));
        leds.show();
      }
      leds.show();
      if(start->GetIsPress())
      {
        currentTimeButton = millis();
        while(digitalRead(PIN_BUTTON));
        {
          if(millis() > currentTimeButton + TICK_START)
          {
            for(int i=0; i<3; i++)
            {
              leds.setPixelColor(i, leds.Color(0,0,150));
              leds.show();
            }
          menu = MAIN_MENU; 
          }
        }
        menu = OPCION_2;
      }
      break;
    }

    case OPCION_2:
    {
      leds.clear();
      for(int i=0; i<6; i++)
      {
        leds.setPixelColor(i, leds.Color(0,150,0));
        leds.show();
      }
      leds.show();
      if(start->GetIsPress())
      {
        currentTimeButton = millis();
      
        while(digitalRead(PIN_BUTTON));
        {
          if(millis() > currentTimeButton + TICK_START)
          {
            for(int i=0; i<6; i++)
            {
              leds.setPixelColor(i, leds.Color(0,0,150));
              leds.show();
            }
          menu = MAIN_MENU; 
          }
        }
        menu = OPCION_3;
      }
      break;
    }

    case OPCION_3:
    {
      leds.clear();
      for(int i=0; i<9; i++)
      {
        leds.setPixelColor(i, leds.Color(0,150,0));
        leds.show();
      }
      leds.show();
      if(start->GetIsPress())
      {
        currentTimeButton = millis();
      
        while(digitalRead(PIN_BUTTON));
        {
          if(millis() > currentTimeButton + TICK_START)
          {
            for(int i=0; i<9; i++)
            {
              leds.setPixelColor(i, leds.Color(0,0,150));
              leds.show();
            }
          menu = MAIN_MENU;  
          }
        }
        menu = OPCION_1;
      }
      break;
    }
  }
}

void setup() 
{
  leds.begin();
}

void loop() 
{
  Menu();
}
