#include "button.h"

Button::Button(int v_PIN) {
  _initButton.V_PIN = v_PIN;
}

Button::Button(int v_PIN, bool isInvert) {
  _initButton.V_PIN = v_PIN;
  _initButton.invert = isInvert;
}

boolean Button::begin() {
  pinMode(_initButton.V_PIN, INPUT);
  digitalWrite(_initButton.V_PIN, LOW);
  return true;
}

boolean Button::getValue() {
    return value;
}

boolean Button::getState() {
  boolean state = digitalRead(_initButton.V_PIN);
  bool _tempValue = _initButton.invert ? !state : state;
  
  pressTime = millis();
  if(_tempValue == value){
    delayBtn = pressTime - prevTime;
  } else {
    prevTime = pressTime;
    delayBtn = 0;
  }
  value = _tempValue;
  return value;
}

long Button::getDelay() {
  return delayBtn;
}
