#ifndef Main_H_
#define Main_H_
#include <Si446x.h>
#include <Keypad.h>

#define RP_NOP      0
#define RP_WEKUP    1
#define RP_FIER     2
#define RP_SLEP     3
#define RP_AUTO     4

uint8_t RadioTX(si446x_state_t state_after_tx);
#endif