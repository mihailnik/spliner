// крутим мотор туда-сюда плавно с ускорением

#include "GyverStepper2.h"
// подключим три мотора
// у первого и второго управление EN не подключаем
GStepper2<STEPPER2WIRE> stepper1(100, 9, 3);
GStepper2<STEPPER2WIRE> stepper2(100, 6, 5);
GStepper2<STEPPER2WIRE> stepper3(100, 8, 7, 4);

void setup() {
  // мотор 1 просто вращается
  //stepper1.setRunMode(KEEP_SPEED);
  stepper1.setSpeed(3000);
  stepper1.enable();

  // мотор 2 будет делать sweep по проверке tick
  //stepper2.setRunMode(FOLLOW_POS);
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(600);
  stepper2.enable();

  // мотор 3 будет перемещаться на случайную позицию
  //stepper3.setRunMode(FOLLOW_POS);
  stepper3.setMaxSpeed(1000);
  stepper3.setAcceleration(300);
  //stepper3.autoPower(true);
  stepper3.enable();
}

void loop() {
  // первый мотор
  stepper1.tick();
  
  // второй крутим туды-сюды (-1000, 1000)
  if (!stepper2.tick()) {
    static bool dir;
    dir = !dir;
    stepper2.setTarget(dir ? -1000 : 1000);
  }

  // третий по таймеру
  // будет отключаться при остановке
  stepper3.tick();
  static uint32_t tmr;
  if (millis() - tmr > 5000) {   // каждые 5 секунд
    tmr = millis();
    stepper3.setTarget(random(0, 2000));  // рандом 0-2000
  }
}