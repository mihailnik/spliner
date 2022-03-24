#ifndef Main_H_
#define Main_H_
#include <Arduino.h>
#include <Servo.h>
#include <Si446x.h>
// пример настроек сна
#include <GyverPower.h>

#define RP_NOP      0
#define RP_WEKUP    1
#define RP_FIER     2
#define RP_SLEP     3
#define RP_AUTO     4

void RP_SLEEP(void);
void RX_CMD_EXEC(void);

#endif