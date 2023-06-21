#include <EngineController.h>

#define PIN_RIGHT_ENGINE_DIR 19
#define PIN_RIGHT_ENGINE_PWM 18
#define PIN_LEFT_ENGINE_DIR 22
#define PIN_LEFT_ENGINE_PWM 21
#define PWM_CHANNEL_ENGINE_RIGHT 11
#define PWM_CHANNEL_ENGINE_LEFT 12

IEngine *rightEngine = new Driver_G2_18V17(PIN_RIGHT_ENGINE_DIR, PIN_RIGHT_ENGINE_PWM, PWM_CHANNEL_ENGINE_RIGHT);
IEngine *leftEngine = new Driver_G2_18V17(PIN_LEFT_ENGINE_DIR, PIN_LEFT_ENGINE_PWM, PWM_CHANNEL_ENGINE_LEFT);
EngineController *robot = new EngineController(rightEngine, leftEngine);
void setup() {
}

void loop() {
    robot->Forward(200, 100);
    delay(3000);
    robot->Backward(100, 200);
    delay(3000);
    robot->Left(100, 100);
    delay(3000);
    robot->Right(70);
    delay(3000);
    robot->Stop();
    delay(3000);
}
