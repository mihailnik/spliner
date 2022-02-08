// приемник на тележке

#include "main.h"


byte rowPins[ROWS] = {23, 28, 27, 25}; 
byte colPins[COLS] = {24, 22, 26};
char customKey;
int encoders[5][5]; // _counter | _currentStateCLK | _lastStateCLK | _CLK | _DT
byte _up = 50;

// буфер данных для nrf24l01
const byte dataCount = 7;         
int data[dataCount];
int prevData[dataCount];

// переменные для функции мигания
unsigned long previousMillis = 0; 
const long interval = 1000;
bool blinkStatus = false;

byte mode = DIST_MODE;
byte mainDir = STOP_NOW_BTN;
int channels[5][7]; // en | speed | dir | accel | dist | dist_step | mode
const int resistors_count = 9;
const int channels_count = 5;
Sensor* resistors[resistors_count];

Button* dirButtons[channels_count];
Button* enButtons[channels_count]; 
Button* encoderButtons[channels_count];

Button* leftTimeRiskBtn;
Button* rightTimeRiskBtn;
Button* stopRiskBtn;
Button* joiButton;

// iarduino_RTC time(RTC_DS3231); 

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

TM1637Display displays[channels_count] = {
  TM1637Display(channel1_CLK, channel1_DIO),
  TM1637Display(channel2_CLK, channel2_DIO),
  TM1637Display(channel3_CLK, channel3_DIO),
  TM1637Display(channel4_CLK, channel4_DIO),
  TM1637Display(channel5_CLK, channel5_DIO)
};

TM1637Display display_time(time_CLK, time_DIO);

//TM1637Display displays[0] = TM1637Display display_ch1(channel1_CLK, channel1_DIO);

RF24 radio(48, 53); // nRF24L01+ (CE, CSN)

void setup() {
  // Setup Displays
  for (byte i = 0; i < channels_count; i++) {
    displays[i].setBrightness(5);
  }
  
  display_time.setBrightness(7);
  
  //time.begin();
  //time.period(1);

  // Connect Radio Channel
  radio.begin();    
  delay(10);  // itit nRF24L01+
  radio.setChannel(0x44);                              // Specify the data transfer channel (from 0 to 127), 5 - means data is transmitted at a frequency of 2.405 GHz (on one channel there can be only 1 receiver and up to 6 transmitters)
  radio.setDataRate     (RF24_2MBPS);                  // Specify the data transfer rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1 Mbps
  radio.setPALevel      (RF24_PA_HIGH);                 // Indicate the transmitter power (RF24_PA_MIN = -18dBm, RF24_PA_LOW = -12dBm, RF24_PA_HIGH = -6dBm, RF24_PA_MAX = 0dBm)
  byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};
  radio.openWritingPipe(address[5]);
  data[0] = 512; // Kivok
  data[1] = 512; // Lift
  data[2] = 512; // Clock
  data[3] = 0; // Karusel
  data[4] = 512; // Focus
  data[5] = 0; // Koleso 1
  data[6] = 512; // Koleso 2
  // Sensor
  resistors[c_clock] = new Sensor(A0, 2, true);    // Channel 1 
  resistors[c_nod] = new Sensor(A1, 2, true);      // Channel 2 
  resistors[c_lift] = new Sensor(A2, 2, true);     // Channel 3 
  resistors[c_carousel] = new Sensor(A3, 2, true); // Channel 4 
  resistors[c_rail] = new Sensor(A4, 2, true);     // Channel 5 
  
  resistors[l_rail] = new Sensor(A5, 1, false); // Faider (RAIL)
  resistors[l_nod] = new Sensor(A7, 445, 485); // X_Axis
  resistors[l_carousel] = new Sensor(A8, 465, 490); // Y_Axis
  resistors[l_clock] = new Sensor(A9, 490, 530); // Z_Axis

  dirButtons[0] = new Button(30); 
  enButtons[0] = new Button(31, true);
  
  dirButtons[1] = new Button(34);
  enButtons[1] = new Button(35, true);
  
  dirButtons[2] = new Button(38);
  enButtons[2] = new Button(39, true);
  
  dirButtons[3] = new Button(43);
  enButtons[3] = new Button(41, true);
  
  dirButtons[4] = new Button(47);
  enButtons[4] = new Button(45, true);

  //joiButtons = new Button(46);

  leftTimeRiskBtn = new Button(32, true);
  rightTimeRiskBtn = new Button(40, true);
  stopRiskBtn = new Button(36, true);

  encoderButtons[0] = new Button(29, true);
  encoderButtons[1] = new Button(33, true);
  encoderButtons[2] = new Button(37, true);
  encoderButtons[3] = new Button(42, true);
  encoderButtons[4] = new Button(44, true);
  
  pinMode(CLK_0, INPUT);
  pinMode(DT_0, INPUT);
  
  pinMode(CLK_1, INPUT);
  pinMode(DT_1, INPUT);

  pinMode(CLK_2, INPUT);
  pinMode(DT_2, INPUT);

  pinMode(CLK_3, INPUT);
  pinMode(DT_3, INPUT);

  pinMode(CLK_4, INPUT);
  pinMode(DT_4, INPUT);
  
  
  encoders[0][_counter] = 0;
  encoders[0][_CLK] = CLK_0;
  encoders[0][_DT] = DT_0;
  encoders[0][_lastStateCLK] = digitalRead(CLK_0);
  attachPCINT(digitalPinToPCINT(CLK_0), encoderRead0, CHANGE);
  
  encoders[1][_counter] = 0;
  encoders[1][_CLK] = CLK_1;
  encoders[1][_DT] = DT_1;
  encoders[1][_lastStateCLK] = digitalRead(CLK_1);
  attachPCINT(digitalPinToPCINT(CLK_1), encoderRead1, CHANGE);
  
  encoders[2][_counter] = 0;
  encoders[2][_CLK] = CLK_2;
  encoders[2][_DT] = DT_2;
  encoders[2][_lastStateCLK] = digitalRead(CLK_2);
  attachPCINT(digitalPinToPCINT(CLK_2), encoderRead2, CHANGE);
  
  encoders[3][_counter] = 0;
  encoders[3][_CLK] = CLK_3;
  encoders[3][_DT] = DT_3;
  encoders[3][_lastStateCLK] = digitalRead(CLK_3);
  attachPCINT(digitalPinToPCINT(CLK_3), encoderRead3, CHANGE);
  
  encoders[4][_counter] = 0;
  encoders[4][_CLK] = CLK_4;
  encoders[4][_DT] = DT_4;
  encoders[4][_lastStateCLK] = digitalRead(CLK_4);
  attachPCINT(digitalPinToPCINT(CLK_4), encoderRead4, CHANGE);
  
  Serial.begin(9600);
}

void loop() {

// Вызываем режим который хотим при инициализации
  setStopmMotionMode();
  while(1){
    ModeSwicher();
    blinkFunc();
  }
}

// Переключаем кнопкой "#" и "тикаем" активный режим
void ModeSwicher(){
  customKey = customKeypad.getKey();
  if (customKey == '#'){ // Переключаем режим кнопкой "#"
    if(mode == DIST_MODE)             {setStopMode(); }
//    else if(mode == STOP_MOTION_MODE) {setLiveMode(); }
//    else if(mode == LIVE_MODE)        {setSpeedMode(); }
//    else if(mode == SPEED_MODE)       {setAccMode(); }
//    else if(mode == ACC_MODE)         {setDistMode(); }
   else if(mode == STOP_MODE)         {setStopmMotionMode(); }
   else if(mode == STOP_MOTION_MODE)         {setDistMode(); }
    }

    //"тикаем" активный режим 
  if       (mode == STOP_MODE)  { stopMode(); }
  else if  (mode == STOP_MOTION_MODE)  { stopMotionMode(); }
  else if  (mode == DIST_MODE)         { distMode(); }
  // else if  (mode == RUN_MODE)          { runMode(); }
  // else if  (mode == ACC_MODE)          { accMode(); } 
  // else if  (mode == SPEED_MODE)         { speedMode(); }
  // else if  (mode == LIVE_MODE)         { liveControl(); } 

}

void distMode(){
  readResistors();
  readDirButtons();
  readEnButtons();
  showValues(c_accel);
  bool invert = false;
  if(mainDir == TO_THE_LEFT_BTN){
    invert = true;
    setDataForSend(invert);
  } else if (mainDir == TO_THE_RIGHT_BTN){
    invert = false;
    setDataForSend(invert);
  } 
}

void stopMode(){
  for (byte i = 0; i < channels_count; i++) {
    data[i] = 512;
  }
    sendData(STOP_FORMAT, c_empty);
}

void stopMotionMode(){
   if( readStopRunButtons() == STOP_NOW_BTN){sendData(PAUSE_FORMAT,c_dir );}
  // шлем изменения кнопок En, Dir
  if(readDirButtons()){sendData(DIR_FORMAT, c_dir);  } 
  if(readEnButtons()){sendData(EN_FORMAT, c_en );  } 

  if (readResistors()){
      showValues(c_accel);
  }
}

// Читаем кнопки << , STOP, >>. Если стоп сразу отправляем.
// Если <<  или  >> то btn=LEFT RIGHT, выводи на дисп rigt left stop
int8_t readStopRunButtons(){
  uint8_t btn = 0;
    if (stopRiskBtn->getState()){
      btn = STOP_NOW_BTN;
      display_time.setSegments(SEG_STOP);
    }else if(leftTimeRiskBtn->getState()){
      display_time.setSegments(SEG_LEFT);
      btn = TO_THE_LEFT_BTN;
    } else if (rightTimeRiskBtn->getState()){
      display_time.setSegments(SEG_RIGH);
      btn = TO_THE_RIGHT_BTN;
    }
 return btn;
}

// читаем кнопки MUTE, DIR каждого канала, если dir поменялись то отправляем их
bool readDirButtons(){
  bool st = 0;
  bool chng = false;
  for (byte i = 0; i < channels_count; i++) {
    //читаем Dir, если изменилось то сохраняем и шлем
    st = dirButtons[i]->getState();
    if(channels[i][c_dir] != st){
      chng = true;
      channels[i][c_dir] = st;
    } 
  }
  return chng;
}

bool readEnButtons(){
  bool chng = false;
  bool st = 0;
  for (byte i = 0; i < channels_count; i++) {
    //читаем En, если изменилось то сохраняем и шлем
    st = enButtons[i]->getState();
    if(channels[i][c_en] != st){
      chng = true;
      channels[i][c_en] = st;
    } 
  }
  return chng;
}

//считываем резистор скорости обновляем значения в каналах если изменения то отправляем
bool readResistors(){
  int val =0;
  bool changes = false;
  for (byte i = 0; i < channels_count; i++) {
    if(channels[i][c_en] != m_en){
        val = getValueById(i);
        if(channels[i][c_speed] != val){
           channels[i][c_speed] = val;
          changes = true;
          }
     }
  }
    return changes;
}

// void speedMode(){
//   readResistors();
//   readMuteDirButtons();
//   showValues();
// }

// void accMode(){
//   readMuteDirButtons();
//   showValues( c_accel );
//   sendData(ACCEL_FORMAT, c_accel );
// }

// void runMode(){
//   readResistors();
// //  showValues();
  
//   if(mainDir == TO_THE_LEFT_BTN){
//     setDataForSend(true);
//   } else if (mainDir == TO_THE_RIGHT_BTN){
//     setDataForSend(false);
//   } else if (mainDir == STOP_NOW_BTN) {
//     stopMode();
//     mode = SPEED_MODE;
//     display_time.setSegments(SEG_EDIT);
//   }
//   sendData();
// }



void setLiveChanelMode(){
  for (byte i = 0; i < channels_count; i++) {
    channels[i][c_mode] = m_live;
    displays[i].setSegments(SEG_LIVE);
  }
  mode = LIVE_MODE;
  display_time.setSegments(SEG_LIVE);
  delay(500);
}

void setSpeedChanelMode(){
  for (byte i = 0; i < channels_count; i++) {
    channels[i][c_mode] = m_active;
  }
  mode = SPEED_MODE; // SPEED
  display_time.setSegments(SEG_SPED);
  delay(500);
}

void setDistMode(){
  mode = DIST_MODE;
  display_time.setSegments(SEG_DIST);
  delay(500);
}

void setAccChanelMode(){
  mode = ACC_MODE;
  display_time.setSegments(SEG_ACC);
  delay(500);
}

void setStopMode(){
  mode = STOP_MOTION_MODE;
  display_time.setSegments(SEG_REPT);
  delay(500);
}
void setStopmMotionMode(){
  mode = STOP_MOTION_MODE;
  display_time.setSegments(SEG_REPT);
  delay(500);
}

void liveControl(){    
//    readMuteDirButtons();
    readResistors();
//    showValues();
    setDataForSend(true);
    data[c_format] = LIVE_FORMAT;
    sendData(LIVE_FORMAT,c_dist);
}

void sendData(uint8_t format, uint8_t param_index ){  // Отправка с пульта в терминал и на робот на робот
  
    for (byte l = 0; l < channels_count; l++) {
        data[l] = channels[l][param_index];
    }
    data[c_format] = format;

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
    
    radio.write(&data, sizeof(data));
}

void blinkFunc(){  // Функция меняет blinkStatus с периодом interval
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    blinkStatus = !blinkStatus;
  }
}


void displayChannel(byte id, int value){
  displays[id].showNumberDec(value, false);
}
//вывод на сегментные индикаторы SEG_DONE,SEG_OFF, SEG_DIR, SEG_RUN, SEG_GOO, SEG_LEFT, SEG_RIGH, SEG_STOP, SEG_EDIT
void displayChannel(byte id, char * seg){
   // displays[id].setSegments(seg);
}
// void displayChannel(byte id, const char *title){
//   if(title == "done"){
//     displays[id].setSegments(SEG_DONE);
//   } else if(title == "off"){
//     displays[id].setSegments(SEG_OFF);
//   } else if (title == "dir"){
//     displays[id].setSegments(SEG_DIR);
//   } else if(title == "run"){
//     displays[id].setSegments(SEG_RUN);
//   } else if(title == "goo"){
//     displays[id].setSegments(SEG_GOO);
//   } else if(title == "left"){
//     displays[id].setSegments(SEG_LEFT);
//   } else if(title == "right"){
//     displays[id].setSegments(SEG_RIGH);
//   } else if(title == "stop"){
//     displays[id].setSegments(SEG_STOP);
//   } else if(title == "edit"){
//     displays[id].setSegments(SEG_EDIT);
//   }
// }

// выводим на дисплей параметр  en | speed | dir | accel | dist | mode
void showValues(uint8_t param){
  for (byte i = 0; i < channels_count; i++) {
    if(channels[i][c_en] == 0){
      displays[i].setSegments(SEG_OFF);
    } else {
      displays[i].showNumberDec(channels[i][param], false);
    }
  }
}



byte convertToNumber(char numChar){
  byte num = 250;
  switch(numChar){
    case '0' : num = 0; break;
    case '1' : num = 1; break;
    case '2' : num = 2; break;
    case '3' : num = 3; break;
    case '4' : num = 4; break;
    case '5' : num = 5; break;
    case '6' : num = 6; break;
    case '7' : num = 7; break;
    case '8' : num = 8; break;
    case '9' : num = 9; break;
  }
  return num;
}

void encoderRead0(void) {
  _encode(0);
}

void encoderRead1(void) {
  _encode(1);
}

void encoderRead2(void) {
  _encode(2);
}

void encoderRead3(void) {
  _encode(3);
}

void encoderRead4(void) {
  _encode(4);
}

void _encode(byte id){
  encoders[id][_currentStateCLK] = digitalRead(encoders[id][_CLK]);
  if (encoders[id][_currentStateCLK] != encoders[id][_lastStateCLK]  && encoders[id][_currentStateCLK] == 1){
    digitalRead(encoders[id][_DT]) != encoders[id][_currentStateCLK] ? encoders[id][_counter] += _up : encoders[id][_counter] -=_up;
    channels[id][c_dist] = encoders[id][_counter];
  }
  encoders[id][_lastStateCLK] = encoders[id][_currentStateCLK];
}

uint16_t dirCorrection(uint16_t _speed, bool dir){
  uint16_t speed_value;
  dir = mainDir == TO_THE_LEFT_BTN ? dir : !dir;
  
  if(dir){
    speed_value = map(speed_value, 0, 1023, 512, 1023);
  } else {
    speed_value = map(speed_value, 0, 1023, 512, 0);
  }
  return speed_value;
}

uint16_t distCorrection(uint16_t _dist, byte dir){
  if(!dir){
    _dist = 0;
  }
  return _dist;
}

byte getLiveId(byte id){
  byte liveId = 0;
  switch(id){
    case c_clock : liveId = l_clock; break;
    case c_nod : liveId = l_nod; break;
    case c_carousel : liveId = l_carousel; break;
    case c_rail : liveId = l_rail; break;
    default : liveId = id;
  }
  return liveId;
}


int getValueById(byte id){
  resistors[id]->readValue();
  return resistors[id]->isFullRange ? dirCorrection(resistors[id]->value, channels[id][c_dir]) : resistors[id]->value;
}

void setDataForSend(bool invert){
  for (byte i = 0; i < channels_count; i++) {
    bool dir = invert ? channels[i][c_dir] : !channels[i][c_dir];
    if(mode == DIST_MODE){
      data[i] = channels[i][c_en] != m_off ? distCorrection(channels[i][c_dist], dir) : -10;
    } else {
      data[i] = channels[i][c_en] != m_off ? channels[i][c_speed] : 512;
    }
  }
 mode == DIST_MODE ? data[c_format] = DIST_FORMAT : data[c_format] = LIVE_FORMAT;
}

int getKeybordSpeed(){
int dispSpeed = 0;
  byte curNum = convertToNumber(customKey);
  if(curNum != 250){    
    if(dispSpeed == 0){
      dispSpeed = curNum;
    } else {
      dispSpeed = dispSpeed * 10 + curNum;
    }
  }
  return dispSpeed;
}
  

// void showTime(){
//   if(millis()%1000==0){
//     //Serial.println(time.gettime("H:i:s, D"));
//     time.gettime();
//     display_time.showNumberDec(time.Hours * 100 + time.minutes, false);
//     //display_time.setSegments(SEG_DP);
//     delay(1);                                            
//   }   
// }