#define DEBUG /*Comment this line out in production then all DP Serial instructions are ignored.*/
#include "DebugUtils.h"/*Leave this in, otherwise you get errors.*/


#include <Servo.h>
#include <NewPing.h>

/*** Layout ***/

const int radarServo = 11;

#define TRIGGER_PIN_FIRST  3
#define ECHO_PIN_FIRST     4
#define MAX_DISTANCE_FIRST 300

#define TRIGGER_PIN_SECOND  7
#define ECHO_PIN_SECOND     8
#define MAX_DISTANCE_SECOND 300

const int motor[] = {5, 6, 9, 10};

const int runToggleButton = 2;

/*** Library object initializations. ***/
Servo sweeper;
NewPing firstSonar(TRIGGER_PIN_FIRST, ECHO_PIN_FIRST, MAX_DISTANCE_FIRST);
NewPing secondSonar(TRIGGER_PIN_SECOND, ECHO_PIN_SECOND, MAX_DISTANCE_SECOND);


/*** Settings ***/

const float motorPWMcoef[] = {1, 0.7, 1, 0.7};
int servoZero = 10; //degrees at which the servo does not damage itself


/*** Other variables. ***/
boolean haltMotors = true; 

unsigned long interruptTime; //for debouncing drive/stall mode button
unsigned long lastInterruptTime = 0; //for debouncing drive/stall mode button


int distances[40];
int frontDirection = 0;
int lastFrontDirection = 0;

int i;


void setup() {
  sweeper.attach(radarServo);
  /*Motors.*/
  for (i=0; i<4; i++) {
    pinMode(motor[i], OUTPUT);
  }

  attachInterrupt(digitalPinToInterrupt(runToggleButton), toggleRunning, LOW);

  #ifdef DEBUG
    Serial.begin(9600);
    DPL("Start");
  #endif
}

void loop() {

  for (i=0; i<20; i++) {
    if (!haltMotors) {
      sweeper.write(servoZero + i*9);
    }
    delay(200);
    distances[i] = firstSonar.ping_cm();
    distances[20 + i] = secondSonar.ping_cm();
    delay(20);
  }

  /*Find largest distances in about 120 deg sectors close to last front and back.*/
  lastFrontDirection = frontDirection;
  int maxDistance = distances[lastFrontDirection];
  for (i=1; i<10; i++) {
    byte index1 = lastFrontDirection - i;
    if (index1 < 0) {
      index1 = 40 + index1;
    }
    byte index2 = lastFrontDirection + i;
    if (index2 > 39) {
      index2 = index2 - 40;
    }
    if (distances[index1] > maxDistance) {
      maxDistance = distances[index1];
      frontDirection = index1;
    }
    if (distances[index2] > maxDistance) {
      maxDistance = distances[index2];
      frontDirection = index2;
    }
  }

  /*Run motors if allowed.*/

  if (!haltMotors) {

    int sector = int(frontDirection/90);
    switch (sector) {
      
      case 0:
        analogWrite(motor[0], cos(frontDirection*9));
        analogWrite(motor[1], cos(90 - frontDirection*9));
        analogWrite(motor[2], 0);
        analogWrite(motor[3], 0);
        break;
      
      case 1:
        analogWrite(motor[0], 0);
        analogWrite(motor[1], cos(frontDirection*9 - 90));
        analogWrite(motor[2], cos(180 - frontDirection*9));
        analogWrite(motor[3], 0);
        break;
  
      case 2:
        analogWrite(motor[0], 0);
        analogWrite(motor[1], 0);
        analogWrite(motor[2], cos(frontDirection*9 - 180));
        analogWrite(motor[3], cos(270 - frontDirection*9));
        break;
      
      case 3:
        analogWrite(motor[0], cos(360 - frontDirection*9));
        analogWrite(motor[1], 0);
        analogWrite(motor[3], 0);
        analogWrite(motor[4], cos(frontDirection*9 - 270));
        break;
   
    }
  }
}


void toggleRunning() {
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200) {
    haltMotors = !haltMotors;
  }
  lastInterruptTime = interruptTime;
}

