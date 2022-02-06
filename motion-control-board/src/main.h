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
#include <iarduino_RTC.h>
#include <Keypad.h>
#include "constants.h"
#include "sensor.h"
#include "button.h"
#include "encoder.h"
#include "PinChangeInterrupt.h"
void encoderRead0(void);
void encoderRead1(void);
void encoderRead2(void);
void encoderRead3(void);
void encoderRead4(void);
void _encode(byte id);
void setDataForSend(bool invert);
void sendData(void);
void sendSpeed(bool invert);
void sendAcc(void);
void showTime(void);
void showValues(void);
void setLiveMode(void);
void liveControl(void);
void setStopmMotionMode(void);
void setDistMode(void);
void setAccMode(void);
void setSpeedMode(void);
bool isDataChanged(void);
void blinkFunc(void);
void sendData(void);
void stopMotionMode(void);
void stopMode(void);
void runMode(void);
void editMode(void);
void accMode(void);
void distMode(void);
void displayChannel(byte id, const char *title);
void displayChannel(byte id, int value);
void readChannels(void);
void readStopRunButton(void);
void readMuteDirButtons(void);
void readResistors(void);
int getValueById(byte id);
byte convertToNumber(char numChar);
byte getLiveId(byte id);
uint16_t distCorrection(uint16_t _dist, byte dir);
uint16_t dirCorrection(uint16_t _speed, bool dir);

#endif