#include <EngineController.h>

#define PIN_RIGHT_ENGINE_IN1 21
#define PIN_RIGHT_ENGINE_IN2 19
#define PIN_LEFT_ENGINE_IN1 22
#define PIN_LEFT_ENGINE_IN2 23
#define PWM_CHANNEL_RIGHT_IN1 1
#define PWM_CHANNEL_RIGHT_IN2 2
#define PWM_CHANNEL_LEFT_IN1 3
#define PWM_CHANNEL_LEFT_IN2 4
int speed = 200;

IEngine *rightEngine = new Driver_DRV8825(PIN_RIGHT_ENGINE_IN1, PIN_RIGHT_ENGINE_IN2, PWM_CHANNEL_RIGHT_IN1, PWM_CHANNEL_RIGHT_IN2);
IEngine *leftEngine = new Driver_DRV8825(PIN_LEFT_ENGINE_IN1, PIN_LEFT_ENGINE_IN2, PWM_CHANNEL_LEFT_IN1, PWM_CHANNEL_LEFT_IN2);
EngineController *Ryo = new EngineController(rightEngine, leftEngine);
void setup()
{
}

void loop()
{
    robot->Forward(speed);
    delay(3000);
    robot->Backward(speed);
    delay(3000);
    robot->Left(speed);
    delay(3000);
    robot->Right(speed);
    delay(3000);
    robot->Stop();
    delay(3000);
  
}
