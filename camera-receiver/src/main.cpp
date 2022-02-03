
// приемник на лифте
#include <AccelStepperMR.h>
#include "Motor.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define EMPTY_FORMAT -10
#define SPEED_FORMAT -20
#define DIST_FORMAT -30
#define STOP_FORMAT -40
#define ACCEL_FORMAT -50
#define SET_CURRUNT_FORMAT -60

#define SW1 14
#define SW2 15
#define SW3 16
#define SW4 17

#define ENBL_LIFT1 22
#define ENBL_LIFT2 23
#define ENBL_NOD 25
#define ENBL_CLOCK 24

// For arduino speed up

#define FASTADC 1
// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
// For arduino speed up end

// Define some steppers and the pins the will use
RF24 radio(48, 53); // nRF24L01+ (CE, CSN)

int data[7] = {512, 512, 512, 512, 0, 512, 512};

AccelStepperMR MotorKivok(1, 0, 0); 
AccelStepperMR MotorLift(1, 1, 1);
AccelStepperMR MotorClock(1, 2, 2);

float correction1 = 1.0; // Kivok
float correction2 = 4.0; // Karusel
float correction3 = 5.0;
int focusCorrection = 0;
long prevReadTime = 0;
int currentFocusStep = 0;
int focusSteps = 20000;
long currTime = 0;
bool prevDir[3];
//#define speedPinRead1 A0
//#define speedPinRead2 A2

void setup()
{  
  // For arduino speed up
  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  // For arduino speed up end

  data[0] = 512; // Kivok  //
  data[1] = 512; // Lift
  data[2] = 512; // Clock
  data[3] = 0; // Karusel
  data[4] = -10; // Focus
  data[5] = 0; // Rail 
  data[6] = 512; // 
  
  DDRH = B11111111; // 
  DDRC = B11111111; // Dir
  DDRA = B11111111; // Enable | Brake
  
  PORTH = B11111111;
  PORTC = B11111111;

  PORTA = B11111111;
  
  // 0 PORTH|=(1<<6); NOD
  // 1 PORTH|=(1<<3)|(1<<4); LIFT
  // 2 PORTH|=(1<<5); Clock
  
  byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
  Serial.begin(9600);
  radio.begin();
  delay(10);
  radio.setChannel(0x44); // data receiving channel (from 0 to 127), 5 - means data is being received at a frequency of 2.405 GHz (on one channel there can be only 1 receiver and up to 6 transmitters)
  radio.setDataRate(RF24_2MBPS); // set speed (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // set power (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe(1,address[5]);      //want to listen to the pipe 0
  //radio.openReadingPipe(1, 0x1234567800LL); // We open 1 pipe with identifier 0x1234567890 for receiving data (up to 6 different pipes can be opened on the channel, which should differ only in the last byte of the identifier)
  radio.startListening(); // Turn on the receiver, start listening to an open pipe

  MotorKivok.setMaxSpeed(100000.0);
  MotorLift.setMaxSpeed(100000.0);  
  MotorClock.setMaxSpeed(100000.0);
  
  MotorKivok.setSpeed(0.0);
  MotorLift.setSpeed(0.0);
  MotorClock.setSpeed(0.0);

  MotorLift.setAcceleration(1000);

  // set Dir for Lift
  PORTC|=(1<<0);
  PORTC&=~(1<<1);
}
int currPosFocus = 0;
long prevBuffTime = 0;
int posBuff(int newPos){
  int pos = currPosFocus;
  if(currTime > prevBuffTime + 50){
    pos = newPos;
    prevBuffTime = currTime;
  }
  return pos;
}
void logInput(){
  Serial.print("Data: ");
  Serial.print(data[0]);
  Serial.print(" | ");
  Serial.print(data[1]);
  Serial.print(" | ");
  Serial.print(data[2]);
  Serial.print(" | ");
  Serial.print(data[3]);
  Serial.print(" | ");
  Serial.print(data[4]);
  Serial.print(" | ");
  Serial.print(data[5]);
  Serial.print(" | ");
  Serial.println(data[6]);
  delay(100);
}    
void loop()
{
  MotorLift.setCurrentPosition(0);
  digitalWrite(ENBL_LIFT1, digitalRead(SW1));
  digitalWrite(ENBL_LIFT2, digitalRead(SW2));
  digitalWrite(ENBL_NOD, digitalRead(SW3));
  digitalWrite(ENBL_CLOCK, digitalRead(SW4));

  while (1){
    //currTime = millis();
    //if(currTime > prevReadTime + 100){
    //  prevReadTime = currTime;
      if(radio.available()){
         radio.read(&data, sizeof(data));
         logInput();
      }

//      if(data[6] == SPEED_FORMAT){
//        MotorLift.setMaxSpeed(data[1]);
//      } else if (data[6] == STOP_FORMAT) {
//        MotorLift.stop();
//        return;
//      } else if (data[6] == SET_CURRUNT_FORMAT) {
//        MotorLift.setCurrentPosition(data[1]);
//        return;
//      }
      if(data[6] == EMPTY_FORMAT || data[6] == STOP_FORMAT){
      
        if(data[0] > -1){
          float round_speed0 = 0.0;
          int res0 = (data[0] / 10) * 10;
          if(res0 > 520.0){
            round_speed0 = (res0 - 512.0) * correction1; 
            MotorKivok.setSpeed(round_speed0);
            if(prevDir[0]){
              MotorKivok.setDirection(false);
              prevDir[0] = false;
            }
          }else if(res0 < 510){
            round_speed0 = (512.0 - res0) * correction1;
            MotorKivok.setSpeed(round_speed0);
            if(!prevDir[0]){
              MotorKivok.setDirection(true);
              prevDir[0] = true;
            }
          }else{
            round_speed0 = 0.0;
            MotorKivok.setSpeed(round_speed0);
          }
        }
        
        
  //      if(data[6] == DIST_FORMAT){
  //          float round_speed4 = 0.0;
  //          int res4 = (data[1] / 10) * 10;
  //          round_speed4 = res4 * correction2;
  //          MotorLift.moveTo(round_speed4);
  //          
  //        if (MotorLift.distanceToGo() != 0)
  //        {
  //          MotorLift.run();
  //        }
        // } else {
          if(data[1] > -10){
            float round_speed1 = 0.0;
            int res1 = (data[1] / 10) * 10;
            if(res1 > 520.0){
              round_speed1 = (res1 - 512.0) * correction3; 
              MotorLift.setSpeed(round_speed1);
              if(prevDir[1]){
                MotorLift.setDirection(false);
                prevDir[1] = false;
              }
            }else if(res1 < 510){
              round_speed1 = (512.0 - res1) * correction3;
              MotorLift.setSpeed(round_speed1);
              if(!prevDir[1]){
                MotorLift.setDirection(true);
                prevDir[1] = true;
              }
            }else{
              round_speed1 = 0.0;
              MotorLift.setSpeed(round_speed1);
            }
          }
        // }
        
        if(data[2] > -10){
          float round_speed2 = 0.0;
          int res2 = (data[2] / 10) * 10;
          if(res2 > 520.0){
            round_speed2 = (res2 - 512.0) * correction3;
            MotorClock.setSpeed(round_speed2);
            if(prevDir[2]){
              MotorClock.setDirection(false);
              prevDir[2] = false;
            }
          }else if(res2 < 510){
            round_speed2 = (512.0 - res2) * correction3;
            MotorClock.setSpeed(round_speed2);
            if(!prevDir[2]){
              MotorClock.setDirection(true);
              prevDir[2] = true;
            }
          }else{
            round_speed2 = 0.0;
            MotorClock.setSpeed(round_speed2);
          }
        }
      }
    //}
    MotorKivok.runSpeed();
    
    MotorLift.runSpeed();
    
    MotorClock.runSpeed();
      
    PORTH = B11111111;
    
  }
}
