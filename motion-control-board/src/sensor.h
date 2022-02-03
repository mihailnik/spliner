#ifndef sensor_H
#define sensor_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

struct initSensorStruct {
  int V_PIN;
  int zMin; 
  int zMax;
  bool buffer_value;
  bool isFullRange;
  bool invert;
};

class Sensor {
  initSensorStruct _initSensor;
   
 public:
  int value = 512;
  bool isChanged;
  bool invert = false;
  bool isFullRange = false;
  Sensor(int v_PIN, int vzMin, int vzMax);
  Sensor(int v_PIN, int buffer_value);
  Sensor(int v_PIN, int buffer_value, bool invert);
  boolean begin();
  void readValue();
  void setValue(int val);
};

#endif
