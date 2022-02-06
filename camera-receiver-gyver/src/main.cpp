#include "main.h"
// #include "Motor.h"
#include <AccelStepper.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define SERIAL_EN 1

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

int currentNod = 0;
int currentLift = 0;
int currentClock = 0;

AccelStepper MotorNod(1, 9, 34); 
AccelStepper MotorLift(1, 6, 37);
AccelStepper MotorClock(1, 8, 35);

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

void setup(){
  // For arduino speed up
  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  // For arduino speed up end
  
  DDRH = B11111111; // 
  DDRC = B11111111; // Dir
  DDRA = B11111111; // Enable | Brake
  
  PORTH = B11111111;
  PORTC = B11111111;

  PORTA = B11111111;

  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  
  // 0 PORTH|=(1<<6); NOD
  // 1 PORTH|=(1<<3)|(1<<4); LIFT
  // 2 PORTH|=(1<<5); Clock
  
  byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
   #if SERIAL_EN
  Serial.begin(9600);
   #endif
  radio.begin();
  delay(10);
  radio.setChannel(0x44); // data receiving channel (from 0 to 127), 5 - means data is being received at a frequency of 2.405 GHz (on one channel there can be only 1 receiver and up to 6 transmitters)
  radio.setDataRate(RF24_2MBPS); // set speed (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // set power (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe(1,address[5]);      //want to listen to the pipe 0
  //radio.openReadingPipe(1, 0x1234567800LL); // We open 1 pipe with identifier 0x1234567890 for receiving data (up to 6 different pipes can be opened on the channel, which should differ only in the last byte of the identifier)
  radio.startListening(); // Turn on the receiver, start listening to an open pipe

  MotorNod.setMaxSpeed(100000.0);
  MotorLift.setMaxSpeed(100000.0);  
  MotorClock.setMaxSpeed(100000.0);
  
  MotorNod.setSpeed(100.0);
  MotorLift.setSpeed(100.0);
  MotorClock.setSpeed(100.0);

  MotorNod.setAcceleration(1000);
  MotorLift.setAcceleration(1000);
  MotorClock.setAcceleration(1000);

  // set Dir for Lift
  PORTC|=(1<<0);
 // PORTC&=~(1<<1);  // реверс для второго лифта
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
  MotorClock.setCurrentPosition(0);
  MotorNod.setCurrentPosition(0);
  
  digitalWrite(ENBL_LIFT1, digitalRead(SW1));
  digitalWrite(ENBL_LIFT2, digitalRead(SW2));
  digitalWrite(ENBL_NOD, digitalRead(SW3));
  digitalWrite(ENBL_CLOCK, digitalRead(SW4));

  while (1){
    if(radio.available()){
       radio.read(&data, sizeof(data));
       #if SERIAL_EN
        logInput();
        #endif
    }

    if(data[c_format] == SPEED_FORMAT){
      MotorClock.setMaxSpeed(data[c_clock]);
      MotorLift.setMaxSpeed(data[c_lift]);
      MotorNod.setMaxSpeed(data[c_nod]);
    } else if (data[c_format] == STOP_FORMAT) {
      MotorClock.stop();
      MotorLift.stop();
      MotorNod.stop();
      return;
    } else if (data[c_format] == SET_CURRUNT_FORMAT) {
      MotorClock.setCurrentPosition(data[c_clock]);
      MotorLift.setCurrentPosition(data[c_lift]);
      MotorNod.setCurrentPosition(data[c_nod]);
      return;
    } else if (data[c_format] == ACCEL_FORMAT) {
      MotorClock.setAcceleration(data[c_clock]);
      MotorLift.setAcceleration(data[c_lift]);
      MotorNod.setAcceleration(data[c_nod]);
      return;
    }

    if(data[c_format] == DIST_FORMAT){
        MotorClock.moveTo(data[c_clock]);
        MotorLift.moveTo(data[c_lift]);
        MotorNod.moveTo(data[c_nod]);
    }

    if(data[c_format] == REPEAT_FORMAT){
        currentNod += data[c_nod];
        currentLift += data[c_lift];
        currentClock += data[c_clock];
        
        MotorClock.moveTo(currentClock);
        MotorLift.moveTo(currentLift);
        MotorNod.moveTo(currentNod);
    }

    if (MotorClock.distanceToGo() != 0){
      MotorClock.run();
    }
    if (MotorLift.distanceToGo() != 0){
      MotorLift.run();
    }
    if (MotorNod.distanceToGo() != 0){
      MotorNod.run();
    }
  }
 }

