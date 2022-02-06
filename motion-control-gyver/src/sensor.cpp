#include "sensor.h"

Sensor::Sensor(int v_PIN, int vzMin, int vzMax) {
  _initSensor.V_PIN = v_PIN;
  _initSensor.zMin = vzMin;
  _initSensor.zMax = vzMax;
  _initSensor.isFullRange = false;
};

Sensor::Sensor(int v_PIN, int buffer_value) {
  _initSensor.V_PIN = v_PIN;
  _initSensor.buffer_value = buffer_value;
  _initSensor.isFullRange = true;
};

Sensor::Sensor(int v_PIN, int buffer_value, bool invert) {
  _initSensor.V_PIN = v_PIN;
  _initSensor.buffer_value = buffer_value;
  _initSensor.isFullRange = true;
  _initSensor.invert = invert;
};


boolean Sensor::begin() {
  pinMode(_initSensor.V_PIN, INPUT);
  analogWrite(_initSensor.V_PIN, LOW);
  return true;
}

void Sensor::setValue(int val){
  if(_initSensor.isFullRange){
      if(value + 2 < val || value - 2 > val){
        value = val;
      }
  } else {
    if (val > _initSensor.zMax || val < _initSensor.zMin){
      value = val;
    } else {
      value = 512;
    }
  }
}

void Sensor::readValue() {
  int valueRead;
  if(_initSensor.invert){
    valueRead = analogRead(_initSensor.V_PIN);
  } else {
    valueRead = 1023 - analogRead(_initSensor.V_PIN);
  }
  return setValue(valueRead);
}
