//librerias
#include "BluetoothSerial.h"
#include <EngineController.h>
#include <AnalogSensor.h> //libreria para sensores analogicos( sensores tatami)
#include <DistanceSensors.h>
#include <Button_pullup.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//debug
#define DEBUG_SHARP 0
#define DEBUG_TATAMI 0
#define DEBUG_STATE 0
#define DEBUG_LDR 0
#define TICK_DEBUG 500
#define TICK_DEBUG_STRATEGY 500
#define TICK_DEBUG_SHARP 500
#define TICK_DEBUG_TATAMI 500
#define TICK_DEBUG_LDR 1000
unsigned long currentTimeSharp = 0;
unsigned long currentTimeTatami = 0;
unsigned long currentTimeEstrategy = 0;
unsigned long currentTimeLdr = 0;

//configuramos el Serial Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

//Oled
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

//Variables y constantes para los sensores de tatami
#define PIN_SENSOR_TATAMI_IZQ 35
#define PIN_SENSOR_TATAMI_DER 32
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300

//Variables y constantes LDR
#define PIN_SENSOR_LDR 23
#define MONTADO 100
int ldr;

//Variables y constantes para los sensores de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 27
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 35
#define RIVAL 40
int distSharpRigh;
int distSharpLeft;

// Variables y constantes para los motores
#define PIN_RIGHT_ENGINE_IN1 26
#define PIN_RIGHT_ENGINE_IN2 17
#define PIN_LEFT_ENGINE_IN1 17
#define PIN_LEFT_ENGINE_IN2 16
#define PWM_CHANNEL_RIGHT_IN1 1
#define PWM_CHANNEL_RIGHT_IN2 2
#define PWM_CHANNEL_LEFT_IN1 3
#define PWM_CHANNEL_LEFT_IN2 4
#define SEARCH_SPEED 65// 12 volt 170
#define ATTACK_SPEED_LDR 255// 12 volt 255
#define ATTACK_SPEED 200// 12 volt 220
#define STRONG_ATTACK_SPEED 210
#define ATTACK_SPEED_AGGRESSIVE 240// 12 volt 235
#define AVERAGE_SPEED 100// 12 volt 200
int slowAttack = 45; // 12 volt 120
int lowAttackCont;
unsigned long currentTimeAttack = 0;
int tickTurn;
#define TICK_LOW_ATTACK 1200
#define TICK_ATTACK_SEARCH 1500
#define TICK_TURN_FRONT 29// 45g
#define TICK_TURN_SIDE 46// 90g
#define TICK_SHORT_BACK_TURN 67// 110g
#define TICK_LONG_BACK_TURN 83// 135g


//Pines para los botones y buzzer
#define PIN_BUTTON 23
bool lec;
bool flank;
unsigned long currentTimeButton = 0;
#define TICK_START 1000
#define BUZZER 18
//<------------------------------------------------------------------------------------------------------------->//
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

IEngine *rightEngine = new Driver_DRV8825(PIN_RIGHT_ENGINE_IN1, PIN_RIGHT_ENGINE_IN2, PWM_CHANNEL_RIGHT_IN1, PWM_CHANNEL_RIGHT_IN2);
IEngine *leftEngine = new Driver_DRV8825(PIN_LEFT_ENGINE_IN1, PIN_LEFT_ENGINE_IN2, PWM_CHANNEL_LEFT_IN1, PWM_CHANNEL_LEFT_IN2);
EngineController *Ryo = new EngineController(rightEngine, leftEngine);

AnalogSensor *rightTatami = new AnalogSensor(PIN_SENSOR_TATAMI_DER);
AnalogSensor *LeftTatami = new AnalogSensor(PIN_SENSOR_TATAMI_IZQ);

Isensor *sharpRight = new Sharp_GP2Y0A02(PIN_SENSOR_DISTANCIA_DERECHO);
Isensor *sharpLeft = new Sharp_GP2Y0A02(PIN_SENSOR_DISTANCIA_IZQUIERDO);

Button_pullup *start = new  Button(PIN_BUTTON);

AnalogSensor *ldrSensor = new AnalogSensor(PIN_SENSOR_LDR);

Adafruit_NeoPixel leds(NUM_LEDS, PIN_LEDS, NEO_RGB + NEO_KHZ800);
//<------------------------------------------------------------------------------------------------------------->//
void printSharp()
{
  if (millis() > currentTimeSharp + TICK_DEBUG_SHARP)
  {
    currentTimeSharp = millis();
    SerialBT.print("Right dist: ");
    SerialBT.print(distSharpRigh);
    SerialBT.print("  //  ");
    SerialBT.print("Left dist: ");
    SerialBT.println(distSharpLeft);
  }
}

//Funciones para imprimir las lecturas de los sensores por el serial Bluetooth
void printLdr()
{
  if (millis() > currentTimeLdr + TICK_DEBUG_LDR)
  {
    currentTimeLdr = millis();
    SerialBT.print("Ldr: ");
    SerialBT.println(ldr);
  }
}

//Funcion para imprimir la lectura de los sensores de tatami en el puerto Bluetooth
void printTatami()
{
  if (millis() > currentTimeTatami + TICK_DEBUG_TATAMI)
  {
    currentTimeTatami = millis();
    SerialBT.print("Right tatami: ");
    SerialBT.print(righTatamiRead);
    SerialBT.print("  //  ");
    SerialBT.print("Left tatami: ");
    SerialBT.println(leftTatamiRead);
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para la lectura de los sensores
void sensorsReading()
  {
    distSharpRigh = sharpRight->SensorRead();
    distSharpLeft = sharpLeft->SensorRead();
    righTatamiRead = rightTatami->SensorRead();
    leftTatamiRead = LeftTatami->SensorRead();
    ldr = ldrSensor->SensorRead();
  }
//<------------------------------------------------------------------------------------------------------------->//
//Con el enum reemplazamos los casos de la maquina de estado por palabras descriptivas para mejor interpretacion del codigo
enum strategy
{
  REPOSITIONING_MENU,
  STRATEGIES_MENU,
  PASSIVE,
  SEMI_PASSIVE,
  SEMI_AGGRESSIVE,
  AGGRESSIVE
};
int strategy = REPOSITIONING_MENU;
//<------------------------------------------------------------------------------------------------------------->//
enum passive
{
  STANDBY_PASSIVE,
  SEARCH_PASSIVE,
  TURN_RIGHT_PASSIVE,
  TURN_LEFT_PASSIVE,
  TATAMI_LIMIT_PASSIVE,
  ATTACK_PASSIVE
}; 
int passive = STANDBY_PASSIVE;
//Estrategia que espera al rival y cuando este se monta ataca
void Passive()
{
  switch (passive)
  {
    case STANDBY_PASSIVE:
    {
    oled.clearDisplay(); 
    leds.setPixelColor(1, leds.Color(150,150,150));
    leds.show();
    Ryo->Stop();
    if (start->GetIsPress())
    {
      leds.clear();
      leds.show();
      Ryo->Stop();
      delay(4900);
      Ryo->Right(ATTACK_SPEED, ATTACK_SPEED);
      delay(tickTurn);
      passive = SEARCH_PASSIVE;
    } 
    break;
    }

    case SEARCH_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) passive = TURN_RIGHT_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) passive = TURN_LEFT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) passive = ATTACK_PASSIVE;
      break;    
    }

    case TURN_RIGHT_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) passive = SEARCH_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) passive = TURN_LEFT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) passive = ATTACK_PASSIVE;
      break;
    }

    case TURN_LEFT_PASSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) passive = SEARCH_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) passive = TURN_RIGHT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) passive = ATTACK_PASSIVE;
      break;
    }

    case ATTACK_PASSIVE:
    {
      if(distSharpRigh <= 10 && distSharpLeft <= 10)
      {
        Ryo->Forward(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
        if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      }

      else 
      {
        Ryo->Stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) passive = SEARCH_PASSIVE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) passive = TURN_RIGHT_PASSIVE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) passive = TURN_LEFT_PASSIVE;
      }
      break;
    }

    case TATAMI_LIMIT_PASSIVE: 
    {
    Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
    delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) passive = SEARCH_PASSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum semiPassive
{
  STANDBY_SEMI_PASSIVE,
  SEARCH_SEMI_PASSIVE,
  TURN_RIGHT_SEMI_PASSIVE,
  TURN_LEFT_SEMI_PASSIVE,
  LOW_ATTACK_SEMI_PASSIVE,
  TATAMI_LIMIT_SEMI_PASSIVE,
  ATTACK_SEMI_PASSIVE
}; 
int semiPassive = STANDBY_SEMI_PASSIVE;
//Estrategia que espera al rival y avanza de a poco buscando el rival
void SemiPassive()
{
  switch (semiPassive)
  {
    case STANDBY_SEMI_PASSIVE:
    {
    leds.clear();
    leds.setPixelColor(1, leds.Color(150,150,150));      
    leds.setPixelColor(2, leds.Color(150,150,150));
    leds.show();
    Ryo->Stop();
    Ryo->Stop();
    if (start->GetIsPress())
    {
      leds.clear();
      leds.show();
      Ryo->Stop();
      delay(5000);
      Ryo->Right(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
      delay(tickTurn);
      semiPassive = SEARCH_SEMI_PASSIVE;
    } 
    break;
    }

    case SEARCH_SEMI_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiPassive = TURN_RIGHT_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiPassive = TURN_LEFT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiPassive = ATTACK_SEMI_PASSIVE;
      if (millis() > currentTimeAttack + TICK_ATTACK_SEARCH)
      {
        semiPassive = LOW_ATTACK_SEMI_PASSIVE;
      }
      if(distSharpRigh <= 10 && distSharpLeft <= 10)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }
      break;    
    }

    case TURN_RIGHT_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiPassive = SEARCH_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiPassive = TURN_LEFT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiPassive = ATTACK_SEMI_PASSIVE;
      if(distSharpRigh <= 10 && distSharpLeft <= 10)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }
      break;
    }

    case TURN_LEFT_PASSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiPassive = SEARCH_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiPassive = TURN_RIGHT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiPassive = ATTACK_SEMI_PASSIVE;
      if(distSharpRigh <= 10 && distSharpLeft <= 10)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }
      break;
    }

    case ATTACK_SEMI_PASSIVE:
    {
      if(distSharpRigh <= 10 && distSharpLeft <= 10)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }

      else 
      {
        Ryo->Stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiPassive = SEARCH_SEMI_PASSIVE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiPassive = TURN_RIGHT_SEMI_PASSIVE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiPassive = TURN_LEFT_SEMI_PASSIVE;
        if (millis() > currentTimeAttack + TICK_LOW_ATTACK)
        {
          semiPassive = LOW_ATTACK_SEMI_PASSIVE;
        }
      }
      break;
    }

    case LOW_ATTACK_SEMI_PASSIVE:
    {
      lowAttackCont++;
      slowAttack = slowAttack + (lowAttackCont*7);
      Ryo->Forward(slowAttack, slowAttack);
      delay(388);
      currentTimeAttack = millis();
      semiPassive = ATTACK_SEMI_PASSIVE;
      break;
    }

    case TATAMI_LIMIT_SEMI_PASSIVE: 
    {
        Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
        delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) semiPassive = SEARCH_SEMI_PASSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum semiAggressive
{
  STANDBY_SEMI_AGGRESSIVE,
  SEARCH_SEMI_AGGRESSIVE,
  TURN_RIGHT_SEMI_AGGRESSIVE,
  TURN_LEFT_SEMI_AGGRESSIVE,
  TATAMI_LIMIT_SEMI_AGGRESSIVE,
  ATTACK_SEMI_AGGRESSIVE
}; 
int semiAggressive = STANDBY_SEMI_AGGRESSIVE;
//Estrategia que va a buscar al rival de forma moderada
void SemiAggressive()
{
  switch (semiAggressive)
  {
    case STANDBY_SEMI_AGGRESSIVE:
    {
    leds.clear();
    leds.setPixelColor(1, leds.Color(150,150,150));      
    leds.setPixelColor(2, leds.Color(150,150,150));
    leds.setPixelColor(3, leds.Color(150,150,150));
    leds.show();
    Ryo->Stop();
    if (start->GetIsPress())
    {
      leds.clear();
      leds.show();
      delay(5000);
      Ryo->Right(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
      delay(tickTurn);
      semiAggressive = SEARCH_SEMI_AGGRESSIVE;
    } 
    break;
    }

    case SEARCH_SEMI_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiAggressive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiAggressive = TURN_RIGHT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiAggressive = TURN_LEFT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiAggressive = ATTACK_SEMI_AGGRESSIVE; 
    }

    case TURN_RIGHT_SEMI_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiAggressive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiAggressive = TURN_LEFT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiAggressive = ATTACK_SEMI_AGGRESSIVE;
      break;
    }

    case TURN_LEFT_SEMI_AGGRESSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiAggressive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiAggressive = TURN_RIGHT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiAggressive = ATTACK_SEMI_AGGRESSIVE;
      break;
    }

    case ATTACK_SEMI_AGGRESSIVE:
    {
      Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiAggressive = TURN_RIGHT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiAggressive = TURN_LEFT_SEMI_AGGRESSIVE;
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh > 15 && distSharpLeft > 15)
      {
        Ryo->Forward(STRONG_ATTACK_SPEED, STRONG_ATTACK_SPEED);
      }
      break;
    }

    case TATAMI_LIMIT_SEMI_AGGRESSIVE: 
    {
      Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
      delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum aggressive
{
  STANDBY_AGGRESSIVE,
  SEARCH_AGGRESSIVE,
  TURN_RIGHT_AGGRESSIVE,
  TURN_LEFT_AGGRESSIVE,
  TATAMI_LIMIT_AGGRESSIVE,
  ATTACK_AGGRESSIVE
}; 
int aggressive = STANDBY_AGGRESSIVE;
//Estrategia que va a buscar al rival de forma moderada
void Aggressive()
{
  switch (aggressive)
  {
    case STANDBY_AGGRESSIVE:
    {
    leds.clear();
    leds.setPixelColor(1, leds.Color(150,150,150));      
    leds.setPixelColor(2, leds.Color(150,150,150));
    leds.setPixelColor(3, leds.Color(150,150,150));
    leds.setPixelColor(4, leds.Color(150,150,150));
    leds.show();
    Ryo->Stop();
    if (start->GetIsPress())
    {
      leds.clear();     
      leds.show();
      delay(5000);
      Ryo->Right(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
      delay(tickTurn);
      aggressive = SEARCH_AGGRESSIVE;
    } 
    break;
    }

    case SEARCH_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) aggressive = TURN_RIGHT_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) aggressive = TURN_LEFT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) aggressive = ATTACK_AGGRESSIVE; 
    }

    case TURN_RIGHT_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) aggressive = SEARCH_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) aggressive = TURN_LEFT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) aggressive = ATTACK_AGGRESSIVE;
      break;
    }

    TURN_LEFT_AGGRESSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) aggressive = SEARCH_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) aggressive = TURN_RIGHT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) aggressive = ATTACK_AGGRESSIVE;
      break;
    }


    case ATTACK_AGGRESSIVE:
    {
        Ryo->Forward(ATTACK_SPEED_AGGRESSIVE, ATTACK_SPEED_AGGRESSIVE);
        if(distSharpRigh > RIVAL || distSharpLeft > RIVAL) aggressive = SEARCH_AGGRESSIVE;
        if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
        if(distSharpRigh <= 10 && distSharpLeft <= 10) 
        {
          Ryo->Forward(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
          if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
        }
        break;
    }

    case TATAMI_LIMIT_AGGRESSIVE: 
    {
    Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
    delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) aggressive = SEARCH_AGGRESSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Maquina de estados para el menu de la pantalla oled
//Maquina de estados para seleccionar el angulo de giro de reposicionamiento
enum repositioningMenu
{
  TURN_MAIN_MENU,
  TURN_FRONT,
  TURN_SIDE,
  SHORT_BACK_TURN,
  LONG_BACK_TURN
};
int repositioningMenu = TURN_MAIN_MENU;

void RepositioningMenu()
{
  switch (repositioningMenu)
  {
  case TURN_MAIN_MENU:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,150,0));
    leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          repositioningMenu = TURN_FRONT;
        }
      }
      repositioningMenu = TURN_FRONT;
    }
    break;
  }
  case TURN_FRONT:
  {
      leds.clear();
      leds.setPixelColor(1, leds.Color(150,0,0));
      leds.setPixelColor(2, leds.Color(150,0,0));
      leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_TURN_FRONT;
          strategy = STRATEGIES_MENU;
        }
      }
      repositioningMenu = TURN_SIDE;
    }
    break;
  }
  case TURN_SIDE:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,150,0));
    leds.setPixelColor(2, leds.Color(0,150,0));
    leds.setPixelColor(3, leds.Color(0,150,0));
    leds.setPixelColor(4, leds.Color(0,150,0));
    leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_TURN_SIDE;
          strategy = STRATEGIES_MENU;
        }
      }
      repositioningMenu = SHORT_BACK_TURN;
    }
    break;
  }
  case SHORT_BACK_TURN:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,0,150));
    leds.setPixelColor(2, leds.Color(0,0,150));
    leds.setPixelColor(3, leds.Color(0,0,150));
    leds.setPixelColor(4, leds.Color(0,0,150));
    leds.setPixelColor(5, leds.Color(0,0,150));
    leds.setPixelColor(6, leds.Color(0,0,150));
    leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_SHORT_BACK_TURN;
          strategy = STRATEGIES_MENU;
        }
      }
      repositioningMenu = LONG_BACK_TURN;
    }
    break;
  }
  case LONG_BACK_TURN:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(150,150,0));
    leds.setPixelColor(2, leds.Color(150,150,0));
    leds.setPixelColor(3, leds.Color(150,150,0));
    leds.setPixelColor(4, leds.Color(150,150,0));
    leds.setPixelColor(5, leds.Color(150,150,0));
    leds.setPixelColor(6, leds.Color(150,150,0));
    leds.setPixelColor(7, leds.Color(150,150,0));
    leds.setPixelColor(8, leds.Color(150,150,0));
    leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          tickTurn = TICK_LONG_BACK_TURN;
          strategy = STRATEGIES_MENU;
        }
      }
      repositioningMenu = TURN_FRONT;
    }
    break;
  }
  }
}

enum strategiesMenu
{
  MAIN_MENU,
  PASSIVE_MENU,
  SEMI_PASSIVE_MENU,
  SEMI_AGGRESSIVE_MENU,
  AGGRESSIVE_MENU,
};
int menu = MAIN_MENU;
//Maquina de estados para navegar dentro del menu y seleccionar la estrategia
void StrategiesMenu()
{
  switch (menu)
  {
  case MAIN_MENU:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,0,200));
    leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          menu = PASSIVE_MENU;
        }
      }
      menu = PASSIVE_MENU;
    }
    break;
  }
  
  case PASSIVE_MENU:
  { 
    leds.clear();
    leds.setPixelColor(1, leds.Color(150,0,0));
    leds.setPixelColor(2, leds.Color(150,0,0));
    leds.setPixelColor(3, leds.Color(150,0,0));
    leds.setPixelColor(4, leds.Color(150,0,0));
    leds.setPixelColor(5, leds.Color(150,0,0));
    leds.setPixelColor(6, leds.Color(150,0,0));
    leds.setPixelColor(7, leds.Color(150,0,0));
    leds.setPixelColor(8, leds.Color(150,0,0));
    leds.show();
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          strategy = PASSIVE;
        }
      }
      menu = SEMI_PASSIVE_MENU;
    }
    break;
  }

  case SEMI_PASSIVE_MENU:
  { 
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,0,150));
    leds.setPixelColor(2, leds.Color(0,0,150));
    leds.setPixelColor(3, leds.Color(0,0,150));
    leds.setPixelColor(4, leds.Color(0,0,150));
    leds.setPixelColor(5, leds.Color(0,0,150));
    leds.setPixelColor(6, leds.Color(0,0,150));
    leds.setPixelColor(7, leds.Color(0,0,150));
    leds.setPixelColor(8, leds.Color(0,0,150));
    leds.show(); 
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          strategy = SEMI_PASSIVE;
        }
      }
      menu = menu = SEMI_AGGRESSIVE_MENU;
    }
    break;
  }

  case SEMI_AGGRESSIVE_MENU:
  { 
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,150,0));
    leds.setPixelColor(2, leds.Color(0,150,0));
    leds.setPixelColor(3, leds.Color(0,150,0));
    leds.setPixelColor(4, leds.Color(0,150,0));
    leds.show(); 
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          strategy = SEMI_AGGRESSIVE;
        }
      }
      menu = menu = AGGRESSIVE_MENU;
    }
    break;
  }

  case AGGRESSIVE_MENU:
  {
    leds.clear();
    leds.setPixelColor(1, leds.Color(0,150,0));
    leds.setPixelColor(2, leds.Color(0,150,0));
    leds.setPixelColor(3, leds.Color(0,150,0));
    leds.setPixelColor(4, leds.Color(0,150,0));
    leds.setPixelColor(5, leds.Color(0,150,0));
    leds.setPixelColor(6, leds.Color(0,150,0));
    leds.setPixelColor(7, leds.Color(0,150,0));
    leds.setPixelColor(8, leds.Color(0,150,0));
    flank = start->GetIsPress();
    if(flank)
    {
      currentTimeButton = millis();
      lec = digitalRead(PIN_BUTTON);
      while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          strategy = AGGRESSIVE;
        }
      }
      menu = menu = PASSIVE_MENU;
    }
    break;
  }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
void logicMovement()
{
  switch (strategy)
  {
    case REPOSITIONING_MENU:
    {
      RepositioningMenu();
      break;
    }
    case STRATEGIES_MENU:
    {
      StrategiesMenu();
      break;
    }
    case PASSIVE:
    {
      Passive();
      break;
    }

    case SEMI_PASSIVE:
    {
      SemiPassive();
      break;
    }

    case SEMI_AGGRESSIVE:
    {
      SemiAggressive();
      break;
    }

    case AGGRESSIVE:
    {
      Aggressive();
      break;
    }   
  }
}
//<------------------------------------------------------------------------------------------------------------->//

void setup()
{
  leds.begin();
  SerialBT.begin("Ryo");
}

void loop(){ 
  sensorsReading();
  logicMovement();
  if(DEBUG_SHARP) printSharp();
  if(DEBUG_TATAMI) printTatami();
}
