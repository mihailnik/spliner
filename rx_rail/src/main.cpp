// Программа для приемника на тележке (рельс + поворот)
#include "main.h"
// #include "Motor.h"
#include "GyverStepper2.h"
#include "GyverPlanner2.h"
#include <GyverTimers.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#define SERIAL_EN 1

#define FASTADC 1
// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
// For arduino speed up end
#define caruselStepPin 6
#define caruselDirPin 4
#define caruselEnPin A2
#define railStepPin 5
#define railDirPin 8
#define railEnPin A1
#define StepPin 3// дополнительная ось
#define DirPin 7
#define EnPin A3
// Define some steppers and the pins the will use
RF24 radio(9, 10); // nRF24L01+ (CE, CSN)

int data[7] = {512, 512, 512, 512, 0, 512, 512};
int dir_Carusel = 0; // буфер направления мотора
int dir_Rail = 0; // буфер направления мотора
int currentCarusel = 0;
int currentRail = 0;

//GStepper<STEPPER2WIRE> stepper(steps, step, dir, en);               // драйвер step-dir + пин enable
GStepper2<STEPPER2WIRE> MotorCarusel(400, caruselStepPin, caruselDirPin); 
GStepper2<STEPPER2WIRE> MotorRail(400, railStepPin, railDirPin); 
Stepper<STEPPER2WIRE> MotorPlanCarusel(caruselStepPin, caruselDirPin);
Stepper<STEPPER2WIRE> MotorPlanRail(railStepPin, railDirPin);
GPlanner2<STEPPER2WIRE, 2> planner;

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
// Timer3.setFrequency(8192);  
  // For arduino speed up
  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
  pinMode(caruselStepPin, OUTPUT);
  pinMode(caruselDirPin, OUTPUT);
  pinMode(railStepPin, OUTPUT);
  pinMode(railDirPin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  
  byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
   #if SERIAL_EN
  Serial.begin(9600);
   #endif

  //Serial.begin(9600);

  radio.begin();
  delay(10);
  radio.setChannel(0x44); // data receiving channel (from 0 to 127), 5 - means data is being received at a frequency of 2.405 GHz (on one channel there can be only 1 receiver and up to 6 transmitters)
  radio.setDataRate(RF24_2MBPS); // set speed (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // set power (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe(1,address[5]);      //want to listen to the pipe 0
  //radio.openReadingPipe(1, 0x1234567800LL); // We open 1 pipe with identifier 0x1234567890 for receiving data (up to 6 different pipes can be opened on the channel, which should differ only in the last byte of the identifier)
  radio.startListening(); // Turn on the receiver, start listening to an open pipe

  MotorCarusel.setMaxSpeed(1024);
  MotorRail.setMaxSpeed(1024);  

  MotorCarusel.setAcceleration(512);
  MotorRail.setAcceleration(256);

  MotorCarusel.enable();
  MotorRail.enable();
//  Timer3.enableISR();                   // Запускаем прерывание (по умолч. канал А)

}


void loop()
{
  // поменять лифт и нод , на пульте clock, lift, nod, carusel, rail
   static uint32_t tmr;
    // MotorCarusel.setTarget(2000);
    // MotorLift.setTarget(2000);
  while (1){
  // if (millis() - tmr >= 1) {
  //   tmr = millis();
//    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  // }

   MotorCarusel.tick(); 
   MotorRail.tick();

    radioRX();

    fsmInput();

    motorStopped();

  }
}

void radioRX(){
  if(radio.available())
  {
    radio.read(&data, sizeof(data));
    #if SERIAL_EN
      logInput();
    #endif
   }
}

void fsmInput(){
    if(data[c_format] == FORMAT_DIR){ // инвертируем направление осей(изменится не во время движения)
      dir_Carusel = data[c_carousel];
      dir_Rail = data[c_rail];
    } else if (data[c_format] == FORMAT_SPEED){
      MotorCarusel.setMaxSpeed(data[c_carousel]);
      MotorRail.setMaxSpeed(data[c_rail]);
    } else if (data[c_format] == FORMAT_STOP){
      MotorCarusel.stop();
      MotorRail.stop();
    } else if (data[c_format] == FORMAT_SET_CURRENT){
      MotorCarusel.setCurrent(data[c_carousel]);
      MotorRail.setCurrent(data[c_rail]);
    } else if (data[c_format] == FORMAT_ACCEL){
      MotorCarusel.setAcceleration(data[c_carousel]);
      MotorRail.setAcceleration(data[c_rail]);
    } else if (data[c_format] == FORMAT_TARGET){
        MotorCarusel.setTarget(data[c_carousel]);
        MotorRail.setTarget(data[c_rail]);
    }
}

void motorStopped(){
    if (MotorCarusel.ready()){
         dir_Carusel==0?  MotorCarusel.reverse(false) : MotorCarusel.reverse(true) ;
      } // инвертировать направление мотора

    if (MotorRail.ready()){
         dir_Rail==0?  MotorRail.reverse(false) : MotorRail.reverse(true) ;
      } // инвертировать направление мотора
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
  // if (millis() - tmr >= 5) {
  //   tmr = millis();
  // }

// Прерывание А таймера 2
ISR(TIMER3_A) {
  MotorCarusel.tick(); 
  MotorRail.tick();
}
