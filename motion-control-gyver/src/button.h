#ifndef button_H
#define button_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

struct initButtonStruct {
  int V_PIN;
  bool invert;
};

class Button {
  initButtonStruct _initButton;
   
 public:
  boolean value = false;
  boolean invert = false;
  long pressTime = 0;
  long prevTime = 0;
  long delayBtn = 0;
  Button(int v_PIN);
  Button(int v_PIN, bool isInvert);
  boolean begin();
  boolean getValue();
  boolean getState();
  long getDelay();
};

#endif
