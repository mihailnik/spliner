#ifndef encoder_H
#define encoder_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

struct initEncoderStruct {
  int CLK;
  int DT;
};

class Encoder {
  initEncoderStruct _initEncoder;
 public:
  int counter = 0;
  int currentStateCLK;
  int lastStateCLK;
  unsigned long lastButtonPress = 0;
  void encoderRead(void);
  //void (Encoder::*send_msg)(void);
  Encoder(int CLK, int DT);
  boolean begin();
  int getValue();
  
};

#endif
