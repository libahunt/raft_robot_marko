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

const int led = 13;

/*** Library object initializations. ***/
Servo sweeper;
NewPing firstSonar(TRIGGER_PIN_FIRST, ECHO_PIN_FIRST, MAX_DISTANCE_FIRST);
NewPing secondSonar(TRIGGER_PIN_SECOND, ECHO_PIN_SECOND, MAX_DISTANCE_SECOND);


/*** Settings ***/

const float motorPWMcoef[] = {1, 1, 1, 1};
int servoZero = 3; //degrees at which the servo does not damage itself
unsigned long startDelay = 5500; //ms


/*** Other variables. ***/
int systemState = 0; 
#define STOP 0
#define DELAYSTART 1
#define GO 2

boolean scanDirection = 1;

unsigned long interruptTime; //for debouncing drive/stall mode button
unsigned long lastInterruptTime = 0; //for debouncing drive/stall mode button

unsigned long startSignal;


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

  sweeper.write(servoZero);
  delay(1000);
}

void loop() {

  if (systemState == DELAYSTART && millis() <= startSignal + startDelay) {
    systemState = GO;
    digitalWrite(led, LOW);
  }

  if (scanDirection == 1) {

    for (i=0; i<20; i++) {
      if (systemState == GO) {
        sweeper.write(servoZero + i*9);
      }
      delay(200);
      distances[i] = firstSonar.ping_cm();
      distances[20 + i] = secondSonar.ping_cm();
      delay(20);
    }

  }
  else {

    for (i=19; i>=0; i--) {
      if (systemState == GO) {
        sweeper.write(servoZero + i*9);
      }
      delay(200);
      distances[i] = firstSonar.ping_cm();
      distances[20 + i] = secondSonar.ping_cm();
      delay(20);
    }

  }
  scanDirection = !scanDirection;

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

  if (systemState == GO) {

    int sector = int(frontDirection/90);
    switch (sector) {
      
      case 0:
        analogWrite(motor[0], int(cos((frontDirection*9)*motorPWMcoef[0])));
        analogWrite(motor[1], int(cos((90 - frontDirection*9)*motorPWMcoef[1])));
        analogWrite(motor[2], 0);
        analogWrite(motor[3], 0);
        break;
      
      case 1:
        analogWrite(motor[0], 0);
        analogWrite(motor[1], int(cos((frontDirection*9 - 90)*motorPWMcoef[1])));
        analogWrite(motor[2], int(cos((180 - frontDirection*9)*motorPWMcoef[2])));
        analogWrite(motor[3], 0);
        break;
  
      case 2:
        analogWrite(motor[0], 0);
        analogWrite(motor[1], 0);
        analogWrite(motor[2], int(cos((frontDirection*9 - 180)*motorPWMcoef[2])));
        analogWrite(motor[3], int(cos((270 - frontDirection*9)*motorPWMcoef[3])));
        break;
      
      case 3:
        analogWrite(motor[0], int(cos((360 - frontDirection*9)*motorPWMcoef[0])));
        analogWrite(motor[1], 0);
        analogWrite(motor[3], 0);
        analogWrite(motor[4], int(cos((frontDirection*9 - 270)*motorPWMcoef[3])));
        break;
   
    }
  }


  for (i=0; i<40; i++) {
    DP(distances[i]);
    DP(" ");
  }
  DP("Front sector: ");
  DPL(frontDirection);
  
}


void toggleRunning() {
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200) {
    systemState++;
    if (systemState == 3) systemState = 0;
    if (systemState == DELAYSTART) {
      startSignal = millis();
      digitalWrite(led, HIGH);
    }
  }
  lastInterruptTime = interruptTime;
}

