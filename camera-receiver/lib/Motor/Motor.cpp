#include "motor.h"

Motor::Motor(int stepPin) {
  stepPin = stepPin;
};

void Motor::changeSpeed(bool isUp)
{
  if(currentSpeed != goalSpeed){ // is we steel need apply smooth
    if(isUp){
      currentSpeed += accel;
    }else{
      currentSpeed -= accel;
    }
    setSpeed(currentSpeed);
  }else if(currentSpeed == goalSpeed){ // when we arrive goal speed turn off smoothing and next time we will keep the same speed
    useSmooth = false;
  }
}

int Motor::getSensorValue()
{
//  if(radio.available()){
//    radio.read(&data, sizeof(data));
//  } 
  speedRead = data[stepPin];
  return speedRead / 10 * 10;
}

void Motor::setDirection(bool dir)
{
  if(_direction != dir){
    if(currentSpeed > 0 && !isInitDirChange){
      isInitDirChange = true;
      goalSpeedTemp = mSpeed;
      mSpeed = 0;
    }else if(currentSpeed == 0){
      _direction = dir;
    }
  }

  if(isInitDirChange && currentSpeed == 0){
    mSpeed = goalSpeedTemp;
    goalSpeedTemp = 0;
    isInitDirChange = false;
  }
}

void Motor::setSpeedAndDirection(int filtredSensor)
{
  if(filtredSensor > 515){
    setDirection(true);
    if(!isInitDirChange){
      mSpeed = filtredSensor - 510;
    }
  }else if(filtredSensor < 510){
    setDirection(false);
    if(!isInitDirChange){
      mSpeed = 510 - filtredSensor;
    }
  }else{
    mSpeed = 0;
  }
}

void Motor::applySpeedCorrection()
{
  if(mSpeed != goalSpeed){
    useSmooth = true;
    goalSpeed = mSpeed;
  }
  if(useSmooth){
    long currTime = millis();
    if(currTime > prevTime + smoothDelay){ // check time for one step speed up
      prevTime = currTime;
      if(mSpeed > currentSpeed){
        upTrueOrDownFalse = true;
      }else{
        upTrueOrDownFalse = false;
      }
      changeSpeed(upTrueOrDownFalse);
    }
  }
}

void Motor::setSpeed(int sp)
{
  float _speed = 510.00 - (float)sp;
  
  //float quotientSpeed = (float)sp / 1000.00;// variant 2
  //long _interval = map(1 / quotientSpeed, 0, 510, minRange, maxRange);// variant 2
  
  long _interval = pow(_speed/37, 4); // variant 1
  
  //float quotientSpeed = map(_speed, 0, 510, minRange, maxRange); //variant 3
  //long _interval = log(quotientSpeed); // variant 3
  
  _stepInterval = constrain(_interval, minRange, maxRange);
  //if(_stepInterval > -30000)
    //Serial.println(_stepInterval);
}

bool Motor::tryToMove()
{
  unsigned long time = micros();   
  if (time - _lastStepTime >= _stepInterval){
    makeStep();
    _lastStepTime = time;
  }
}

void Motor::run(){
  long currTime = millis();
  if(currTime > prevReadTime + 100){ // check time for read sensor
      prevReadTime = currTime;
      int filtredSensor = getSensorValue();
      setSpeedAndDirection(filtredSensor);
      //Serial.println("Read");
      //Serial.println(filtredSensor);
      //delay(500);
  }
  applySpeedCorrection();
  tryToMove();
}

void Motor::makeStep()
{
  // PORTA - Step pins
  // PORTC - Dir pins
  
  if(_direction)
    PORTC = PORTC|(1<<stepPin);
  else
    PORTC = PORTC&(~(1<<stepPin));

  switch (stepPin){
    case 0: PORTA = B00000001; // Focus
      break;
    case 1: PORTA = B00000010; // Clock
      break;
    case 2: PORTA = B00000100; // Rotar
      break;
    case 3: PORTA = B00001000; // Kivok
      break;
    case 4: PORTA = B00010000; // Lift
      break;
    case 5: PORTA = B00100000; // Move
      break;
  }
}

