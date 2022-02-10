/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
#ifndef _MAIN_H_INCLUDED
#define _MAIN_H_INCLUDED

#include <SPI.h>
#include <nRF24L01.h>
#include "RF24.h"
#include <TM1637Display.h>
//#include <iarduino_RTC.h>
#include <Keypad.h>
#include "constants.h"
#include "sensor.h"
#include "button.h"
#include "encoder.h"
#include "PinChangeInterrupt.h"

void blinkFunc(void);
byte convertToNumber(char numChar);
uint16_t dirCorrection(uint16_t _speed, bool dir);
uint16_t distCorrection(uint16_t _dist, byte dir);
void distMode(void);
void displayChannel(byte id, const char *title);
void displayChannel(byte id, int value);
void encoderRead0(void);
void encoderRead1(void);
void encoderRead2(void);
void encoderRead3(void);
void encoderRead4(void);
void _encode(byte id);
bool isDataChanged(void);
int getValueById(byte id);
byte getLiveId(byte id);
void liveControl(void);
void runMode(void);
void readChannels(void);
int8_t readStopRunButtons(void);
bool readEnButtons(void);
bool readDirButtons(void);
bool readResistors(int half_gisterests);
void setStopmMotionMode(void);
void setStopMode(void);
void setDistMode(void);
void setAccMode(void);
void setSpeedMode(void);
void stopMotionMode(void);
void stopMode(void);
void speedMode(void);
void accMode(void);
void setDataForSend(bool invert);
void sendData(uint8_t format, uint8_t param_index );
void sendDir(void);
void sendEn(void);
void sendSpeed(bool invert);
void sendAcc(void);
void showTime(void);
void showValues(uint8_t);
void setLiveMode(void);
void ModeSwicher(void);

#endif