#include <EngineController.h>
#include <Button.h>
#include <Adafruit_NeoPixel.h>

#define PIN_RIGHT_ENGINE_IN1 21
#define PIN_RIGHT_ENGINE_IN2 19
#define PIN_LEFT_ENGINE_IN1 22
#define PIN_LEFT_ENGINE_IN2 23
#define PWM_CHANNEL_RIGHT_IN1 1
#define PWM_CHANNEL_RIGHT_IN2 2
#define PWM_CHANNEL_LEFT_IN1 3
#define PWM_CHANNEL_LEFT_IN2 4

int tickTurn;
#define TICK_TURN_FRONT 29
#define TICK_TURN_SIDE 46
#define TICK_SHORT_BACK_TURN 67
#define TICK_LONG_BACK_TURN 83

#define PIN_LEDS 17
#define NUM_LEDS 8

#define PIN_BUTTON 18
unsigned long currentTimeButton = 0;
#define TICK_START 1000


IEngine *rightEngine = new Driver_DRV8825(PIN_RIGHT_ENGINE_IN1, PIN_RIGHT_ENGINE_IN2, PWM_CHANNEL_RIGHT_IN1, PWM_CHANNEL_RIGHT_IN2);
IEngine *leftEngine = new Driver_DRV8825(PIN_LEFT_ENGINE_IN1, PIN_LEFT_ENGINE_IN2, PWM_CHANNEL_LEFT_IN1, PWM_CHANNEL_LEFT_IN2);
EngineController *Ryo = new EngineController(rightEngine, leftEngine);

Button *start = new  Button(PIN_BUTTON);
Adafruit_NeoPixel leds(NUM_LEDS, PIN_LEDS, NEO_RGB + NEO_KHZ800);

enum repositioningMenu
{
  TURN_MAIN_MENU,
  TURN_FRONT,
  TURN_SIDE,
  SHORT_BACK_TURN,
  LONG_BACK_TURN,
  TEST
};
int repositioningMenu = TURN_MAIN_MENU;

void RepositioningMenu()
{
  switch (repositioningMenu)
  {
  case TURN_MAIN_MENU:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(255,0,0));
    leds.show();
    if(start->GetIsPress())
    {
      currentTimeButton = millis();
      while(digitalRead(PIN_BUTTON))
      {
        if(millis() > currentTimeButton + TICK_START)
        {
          leds.setPixelColor(1, leds.Color(0,255,0));
          delay(2000);
          repositioningMenu = TEST;
        }
      }
      repositioningMenu = TURN_FRONT;
    }
    break;
  }
  case TURN_FRONT:
  {
      leds.clear();
      for(int i=0; i<2; i++)
      {
        leds.setPixelColor(i, leds.Color(255,0,0));
        leds.show();
      }
      leds.show();
    if(start->GetIsPress())
    {
      currentTimeButton = millis();
      while(digitalRead(PIN_BUTTON))
      {
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_TURN_FRONT;
          repositioningMenu = TEST;
        }
      }
      repositioningMenu = TURN_SIDE;
    }
    break;
  }
  case TURN_SIDE:
  {
    leds.clear();
      for(int i=0; i<4; i++)
      {
        leds.setPixelColor(i, leds.Color(255,0,0));
        leds.show();
      }
      leds.show();
    if(start->GetIsPress())
    {
      currentTimeButton = millis();
      while(digitalRead(PIN_BUTTON))
      {
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_TURN_SIDE;
          repositioningMenu = TEST;
        }
      }
      repositioningMenu = SHORT_BACK_TURN;
    }
    break;
  }
  case SHORT_BACK_TURN:
  {
    leds.clear();
      for(int i=0; i<6; i++)
      {
        leds.setPixelColor(i, leds.Color(255,0,0));
        leds.show();
      }
      leds.show();
    if(start->GetIsPress())
    {
      currentTimeButton = millis();
      while(digitalRead(PIN_BUTTON))
      {
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_SHORT_BACK_TURN;
          repositioningMenu = TEST;
        }
      }
      repositioningMenu = LONG_BACK_TURN;
    }
    break;
  }
  case LONG_BACK_TURN:
  {
    leds.clear();
      for(int i=0; i<8; i++)
      {
        leds.setPixelColor(i, leds.Color(255,0,0));
        leds.show();
      }
      leds.show();
    if(start->GetIsPress())
    {
      currentTimeButton = millis();
      while(digitalRead(PIN_BUTTON))
      {
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_LONG_BACK_TURN;
          repositioningMenu = TEST;
        }
      }
      repositioningMenu = TURN_FRONT;
    }
    break;
  }

  case TEST:
  {
    leds.clear();
    leds.show();
    Ryo->Right(255);
    delay(tickTurn);
    repositioningMenu = TURN_MAIN_MENU;
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
  RepositioningMenu();
}
