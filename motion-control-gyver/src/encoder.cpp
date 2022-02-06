#include "encoder.h"
#include "PinChangeInterrupt.h"

Encoder::Encoder(int CLK, int DT) {
  _initEncoder.CLK = CLK;
  _initEncoder.DT = DT;
}

void Encoder::encoderRead(void) {
  currentStateCLK = digitalRead(_initEncoder.CLK);
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    digitalRead(_initEncoder.DT) != currentStateCLK ? counter ++ : counter --;
  }
  lastStateCLK = currentStateCLK;
}
//Encoder::Encoder(int CLK, int DT) : encoderRead(&Encoder::encoderRead)
//{
// 
//}
boolean Encoder::begin() {
  pinMode(_initEncoder.CLK,INPUT);
  pinMode(_initEncoder.DT,INPUT);
  uint8_t CLK = _initEncoder.CLK;
  lastStateCLK = digitalRead(_initEncoder.CLK);
  //send_msg = &Encoder::encoderRead;
  //attachPCINT(digitalPinToPCINT(10), &Encoder::encoderRead, CHANGE);
  return true;
}

int Encoder::getValue() {
    return counter;
}
