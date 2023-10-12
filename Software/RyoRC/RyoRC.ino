//Librerias
#include <EngineController.h>  //Motores
#include "BluetoothSerial.h"   //Bluetooh
#include <PS4Controller.h>


//Pines motores y canales PWM
#define PIN_MOTOR_IZQUIERDO_1 17
#define PIN_MOTOR_IZQUIERDO_2 16
#define PIN_MOTOR_DERECHO_1 26
#define PIN_MOTOR_DERECHO_2 27

#define CANAL_PWM_IZQUIERDO_1 1
#define CANAL_PWM_IZQUIERDO_2 2
#define CANAL_PWM_DERECHO_1 3
#define CANAL_PWM_DERECHO_2 4

// Velocidades Sumo Radiocontrolado
#define MAX_SPEED 250
#define AVERAGE_SPEED 200
#define AVERAGE_LOW_SPEED 150
#define LOW_SPEED 100
int RightSpeed;
int LeftSpeed;

unsigned long currentTimeState;
#define TICK_DEBUG_STATE 500
#define DEBUG_STATE 1

int deadPoint = 50;

//configuramos el Serial Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;


IEngine *rightEngine = new Driver_DRV8825(PIN_MOTOR_DERECHO_1, PIN_MOTOR_DERECHO_2, CANAL_PWM_DERECHO_1, CANAL_PWM_DERECHO_2);
IEngine *leftEngine = new Driver_DRV8825(PIN_MOTOR_IZQUIERDO_1, PIN_MOTOR_IZQUIERDO_2, CANAL_PWM_IZQUIERDO_1, CANAL_PWM_IZQUIERDO_2);
EngineController *Ryo = new EngineController(rightEngine, leftEngine);

void pruebaLectura()
{
  int R2 = PS4.R2Value();
  int L2 = PS4.L2Value(); 
  int X_axis_value = PS4.LStickX()
  if (millis() > currentTimeState + TICK_DEBUG_STATE) 
  {
    currentTimeState = millis();
    //Serial.print("R2: ");
    //Serial.println(R2);
    //Serial.print("L2: ");
    //Serial.println(L2);
    Serial.print("Xaxis: ");
    Serial.println(X_axis_value); 
  }
}

enum mode
{
  MAIN
  MODE_1
  MODE_2
}
int mode = MAIN;

void SumoRC()
{

  switch(mode)
  {
    case MAIN:
    {
      
      break;
    }

    case MODE_1:
    {
      int R2 = PS4.R2Value();
      int L2 = PS4.L2Value(); 
      int X_axis_value = PS4.LStickX();
  
      if(R2 > 0)
      {
        RightSpeed = r2
        LeftSpeed = r2 //Ajustar para solucionar el problema de la rueda
        Ryo->Forward(RightSpeed, LeftSpeed);
      }

      else if(L2 > 0)
      {
        RightSpeed = L2
        LeftSpeed = L2 //Ajustar para solucionar el problema de la rueda
        Ryo->Backward(RightSpeed, LeftSpeed);
      }

      else if(X_axis_value > 50 && X_axis_value < -50)
      {
        if(X_axis_value > 50)
        {
          RightSpeed = X_axis_value
          LeftSpeed = X_axis_value //Ajustar para solucionar el problema de la rueda
          Ryo->Right(RightSpeed, LeftSpeed);
        }

        if(X_axis_value < -50)
        {
          RightSpeed = (X_axis_value * (-1))
          LeftSpeed = (X_axis_value * (-1)) //Ajustar para solucionar el problema de la rueda
          Ryo->Left(RightSpeed, LeftSpeed);
        }
      }

      else
      {
        Ryo->Stop();
      }

      break;
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Ryo");
  PS4.begin();
  Ryo->Stop();

  while (!PS4.isConnected()) {
    if (DEBUG_STATE) {
      if (millis() > currentTimeState + TICK_DEBUG_STATE) {
        currentTimeState = millis();
        Serial.println("Esperando conexion..");
      }
    }
  }
  PS4.attach(Main);
}

void loop() {
}