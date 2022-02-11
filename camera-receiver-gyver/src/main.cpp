#include "main.h"
// #include "Motor.h"
#include "GyverStepper.h"
#include <GyverTimers.h>
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
int dir_Clock = 0; // буфер направления мотора
int dir_Lift = 0; // буфер направления мотора
int dir_Nod = 0; // буфер направления мотора
int currentNod = 0;
int currentLift = 0;
int currentClock = 0;

//GStepper<STEPPER2WIRE> stepper(steps, step, dir, en);               // драйвер step-dir + пин enable
GStepper<STEPPER2WIRE> MotorClock(400, 9, 34, 25); 
GStepper<STEPPER2WIRE> MotorLift(400, 6, 37, 22); 
GStepper<STEPPER2WIRE> MotorNod(400, 8, 35, 24); 

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
  Timer3.setFrequency(600);  
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

  MotorNod.setMaxSpeed(256);
  MotorLift.setMaxSpeed(16384);  
  MotorClock.setMaxSpeed(4096);

  MotorNod.setAcceleration(8);
  MotorLift.setAcceleration(32);
  MotorClock.setAcceleration(8);
  Timer3.enableISR();                   // Запускаем прерывание (по умолч. канал А)

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
  // поменять лифт и нод , на пульте clock, lift, nod, carusel, rail
  MotorClock.enable();
  MotorLift.enable();
  MotorNod.enable();
 static uint32_t tmr;
  while (1){
    if(radio.available()){
       radio.read(&data, sizeof(data));
       #if SERIAL_EN
        logInput();
        #endif
    }
  // if (millis() - tmr >= 5) {
  //   tmr = millis();
  // }
 

    if(data[c_format] == FORMAT_DIR){ // инвертируем направление осей(изменится не во время движения)
      dir_Nod = data[c_nod];
      dir_Lift = data[c_lift];
      dir_Clock = data[c_clock];
    } else if (data[c_format] == FORMAT_SPEED){
      MotorClock.setMaxSpeed(data[c_clock]);
      MotorLift.setMaxSpeed(data[c_lift]);
      MotorNod.setMaxSpeed(data[c_nod]);
    } else if (data[c_format] == FORMAT_PAUSE){
      MotorClock.stop();
      MotorLift.stop();
      MotorNod.stop();
      return;
    } else if (data[c_format] == FORMAT_SET_CURRENT){
      MotorClock.setCurrent(data[c_clock]);
      MotorLift.setCurrent(data[c_lift]);
      MotorNod.setCurrent(data[c_nod]);
      return;
    } else if (data[c_format] == FORMAT_ACCEL){
      MotorClock.setAcceleration(data[c_clock]);
      MotorLift.setAcceleration(data[c_lift]);
      MotorNod.setAcceleration(data[c_nod]);
      return;
    } else if (data[c_format] == FORMAT_TARGET){
        MotorClock.setTarget(data[c_clock]);
        MotorLift.setTarget(data[c_lift]);
        MotorNod.setTarget(data[c_nod]);
      return;
    }


         dir_Clock==0?  MotorClock.reverse(false) : MotorClock.reverse(true) ;
         dir_Lift==0?  MotorLift.reverse(false) : MotorLift.reverse(true) ;
         dir_Nod==0?  MotorNod.reverse(false) : MotorNod.reverse(true) ;
    // if (MotorClock.ready()){
    //      dir_Clock==0?  MotorClock.reverse(false) : MotorClock.reverse(true) ;
    //   } // инвертировать направление мотора

    // if (MotorLift.ready()){
    //      dir_Lift==0?  MotorLift.reverse(false) : MotorLift.reverse(true) ;
    //   } // инвертировать направление мотора

    // if (MotorNod.ready()){
    //      dir_Nod==0?  MotorNod.reverse(false) : MotorNod.reverse(true) ;
    //   } // инвертировать направление мотора
  }
}

// Прерывание А таймера 2
ISR(TIMER3_A) {
  // MotorClock.tick2wr(); 
  // MotorNod.tick2wr(); 
  // MotorLift.tick2wr();
  MotorClock.tick(); 
  MotorNod.tick(); 
  MotorLift.tick();
}