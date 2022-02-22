// Программа для приемника на тележке (рельс + поворот)
#include "main.h"
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "path.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#define SERIAL_EN 1

// For arduino speed up end
#define caruselStepPin 6
#define caruselDirPin 4
#define caruselEnPin A2
#define railStepPin 5
#define railDirPin 8
#define railEnPin A1
#define zStepPin 3// дополнительная ось
#define zDirPin 7
#define zEnPin A3
#define FASTADC 1

// MultiStepper.pde
// -*- mode: C++ -*-
// Use MultiStepper class to manage multiple steppers and make them all move to 
// the same position at the same time for linear 2d (or 3d) motion.

#include <AccelStepper.h>
#include <MultiStepper.h>

// EG X-Y position bed driven by 2 steppers
// Alas its not possible to build an array of these with different pins for each :-(
AccelStepper stepper1(AccelStepper::DRIVER, caruselStepPin, caruselDirPin);
AccelStepper stepper2(AccelStepper::DRIVER, railStepPin, railDirPin);
AccelStepper stepper3(AccelStepper::DRIVER, zStepPin, zDirPin);

// Up to 10 steppers can be handled as a group by MultiStepper
MultiStepper steppers;

void setup() {
  Serial.begin(9600);

  // Configure each stepper
  stepper1.setMaxSpeed(100);
  stepper2.setMaxSpeed(100);
  stepper3.setMaxSpeed(100);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
  steppers.addStepper(stepper3);
}

void loop() {
  long positions[3]; // Array of desired stepper positions
  
  positions[0] = 1000;
  positions[1] = 500;
  positions[2] = 700;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);
  
  // Move to a different coordinate
  positions[0] = 0;
  positions[1] = 0;
  positions[2] = 0;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);
}
