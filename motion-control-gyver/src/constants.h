#define channel1_CLK 2      
#define channel1_DIO 3

#define channel2_CLK 2      
#define channel2_DIO 4

#define channel3_CLK 2      
#define channel3_DIO 5

#define channel4_CLK 2      
#define channel4_DIO 6

#define channel5_CLK 2      
#define channel5_DIO 7

#define time_CLK 2      
#define time_DIO 8

#define c_en 0
#define c_speed 1
#define c_dir 2
#define c_accel 3
#define c_target 4
#define c_old_target 5
#define c_dist 6
#define c_limit 7
#define c_mode 8

//chanel mode rows
#define m_off 0
#define m_en 1
#define m_active 2
#define m_live 3
#define m_key_speed 4
#define m_repeat 5

//data rows
#define c_clock 0
#define c_nod 1 // kivok
#define c_lift 2
#define c_carousel 3
#define c_rail 4
#define c_empty 5
#define c_format 6

//live rows
#define l_clock 8
#define l_nod 6 // kivok
#define l_carousel 7
#define l_rail 5

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

const uint8_t SEG_OFF[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_A | SEG_E | SEG_F | SEG_G,           // F
  SEG_A | SEG_E | SEG_F | SEG_G,          // F
  0
};

const uint8_t SEG_DIR[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,    // d
  SEG_E,                                    // i
  SEG_E | SEG_G,                            // r
};
  
const uint8_t SEG_GOO[] = {
  SEG_A | SEG_B | SEG_G | SEG_C | SEG_F,   // g
  SEG_A | SEG_B | SEG_G | SEG_F,           // o
  SEG_A | SEG_B | SEG_G | SEG_F,           // o
  0
};

const uint8_t SEG_RUN[] = {
  SEG_G | SEG_E,           // r
  SEG_E | SEG_D | SEG_C,   // u
  SEG_E | SEG_G | SEG_C,   // n
  0
};

  
const uint8_t SEG_LEFT[] = {
  SEG_F | SEG_E | SEG_D,
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
  SEG_A | SEG_E | SEG_F | SEG_G,
  SEG_F | SEG_E | SEG_D | SEG_G
};
  
const uint8_t SEG_RIGH[] = {
  SEG_G | SEG_E, 
  SEG_E, 
  SEG_F | SEG_E | SEG_G | SEG_C,
  SEG_F | SEG_E | SEG_D | SEG_G
};
  
const uint8_t SEG_STOP[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
  SEG_F | SEG_E | SEG_D | SEG_G, 
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  SEG_A | SEG_E | SEG_F | SEG_G | SEG_B
};

const uint8_t SEG_SPED[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
  SEG_A | SEG_E | SEG_F | SEG_G | SEG_B, 
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G
};

const uint8_t SEG_EDIT[] = {
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, 
  SEG_F | SEG_E,
  SEG_F | SEG_E | SEG_D | SEG_G
};

const uint8_t SEG_LIVE[] = {
  SEG_F | SEG_E | SEG_D,
  SEG_F | SEG_E, 
  SEG_F | SEG_E | SEG_D | SEG_B | SEG_C,
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
};

const uint8_t SEG_DIST[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G, 
  SEG_F | SEG_E,
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
  SEG_F | SEG_E | SEG_D | SEG_G
};

const uint8_t SEG_ACC[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, 
  SEG_A | SEG_E | SEG_F | SEG_D,
  SEG_A | SEG_E | SEG_F | SEG_D,
  0
};

const uint8_t SEG_L0VE[] = {
  SEG_F | SEG_E | SEG_D,
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
  SEG_F | SEG_E | SEG_D | SEG_B | SEG_C,
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
};

const uint8_t SEG_REPT[] = {
  SEG_G | SEG_E, 
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G, 
  SEG_A | SEG_E | SEG_F | SEG_G | SEG_B,
  SEG_F | SEG_E | SEG_D | SEG_G
};

#define FORMAT_EMPTY 0
#define FORMAT_RESET 1
#define FORMAT_LIVE 2
#define FORMAT_SPEED 3
#define FORMAT_TARGET 4
#define FORMAT_STOP 5
#define FORMAT_PAUSE 6
#define FORMAT_ACCEL 7
#define FORMAT_SET_CURRENT 8
#define FORMAT_REPEAT 9
#define FORMAT_DIR 10
#define FORMAT_EN 11

#define MODE_STOP 0
#define MODE_LIVE 1
#define MODE_SPEED 2
#define MODE_RUN 3
#define MODE_DIST 4
#define MODE_ACC 5
#define MODE_STOP_MOTION 6

#define BTN_TO_THE_LEFT 4
#define BTN_TO_THE_RIGHT 5
#define BTN_STOP_NOW 6

const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] =
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

#define _counter 0
#define _currentStateCLK 1
#define _lastStateCLK 2
#define _CLK 3
#define _DT 4

#define CLK_0 10
#define DT_0 11

#define CLK_1 12
#define DT_1 13

#define CLK_2 A10
#define DT_2 A11

#define CLK_3 A12
#define DT_3 A13

#define CLK_4 A14
#define DT_4 A15
