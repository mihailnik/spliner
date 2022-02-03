#ifndef motor_H
#define motor_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class Motor {
  
 public:
  int mSpeed = 0;
  int maxRange = 30000;
  int minRange = 10;
  int currentSpeed = 0;
  int goalSpeed = 0;
  int goalSpeedTemp = 0;
  int accel = 2;
  bool useSmooth = false;
  long prevTime = 0;
  long smoothDelay = 10;
  bool upTrueOrDownFalse = true;
  bool _direction = true;
  bool _directionTemp = true;
  bool isInitDirChange = false;
  int stepPin;
  int speedRead;
  int data[6];
  long _lastStepTime = 0;
  long prevReadTime = 0;
  long _stepInterval = maxRange; // min 50 - 30.000 (1600)
  
  Motor(int stepPin);
  
  void changeSpeed(bool isUp);
  int getSensorValue();
  void setDirection(bool dir);
  void setSpeedAndDirection(int filtredSensor);
  void applySpeedCorrection();
  void setSpeed(int sp);
  void makeStep();
  bool tryToMove();
  void run();
};

#endif


