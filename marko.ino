//At servo start (0) position, first sensor direction is 0 degrees. This should be opposite of motor 0 push direction.

#define DEBUG /*Comment this line out in production then all DP Serial instructions are ignored.*/
#include "DebugUtils.h"/*Leave this in, otherwise you get errors.*/

//#define PING //Keep this when using ultrasound distance sensors. 
             //Comment out if using VL53L0X micro-lidar. VL53L0X does not work with Atmega328.

#include <Servo.h>

#ifdef PING

  #include <NewPing.h>
  #include "AT328andPing.h"
  
#else

  #include <Wire.h>
  #include <VL53L0X.h>
  #include "layoutVL53L0X.h"
  
#endif

/*** Library object initializations. ***/

Servo sweeper;

/*** Settings ***/

const float motorPWMcoef[] = {1, 0.7, 1, 0.7};
int servoZero = 10; //degrees at which the servo does not damage itself


/*** Other variables. ***/
boolean haltMotors = true; 

unsigned long interruptTime; //for debouncing drive/stall mode button
unsigned long lastInterruptTime = 0; //for debouncing drive/stall mode button

int distances[40];//40 directions to measure makes each one 9 degrees wide
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

  #ifndef PING
    lidarInit();
  #endif
  
}

void loop() {

  //Move servo to 20 different positions and take 2 opposite direction measurements in each.
  for (i=0; i<20; i++) {
    sweeper.write(servoZero + i*9);
    delay(200);

    #ifdef PING
    
      distances[i] = firstSonar.ping_cm();
      delay(50);
      distances[20 + i] = secondSonar.ping_cm();
      
    #else
  
      int dist = sensor1.readRangeSingleMillimeters();
      if (!sensor1.timeoutOccurred()) {
        distances[i] = dist / 10;
      } else {
        distances[i] = 0;
        DPL("Lidar 1 failure.");
      }
      delay(100);
      dist = sensor2.readRangeSingleMillimeters();
      if (sensor2.timeoutOccurred()) {
        distances[20 + i] = dist / 10;
      } else {
        distances[20 + i] = 0;
        DPL("Lidar 2 failure.");
      }
      
    #endif

    delay(20);

    DPL("Measurements:");
    for (i=0; i<40; i++) {
      DP(i);
      DP("\t");
    }
    DPL();
    for (i=0; i<40; i++) {
      DP(distances[i]);
      DP("\t");
    }
    DPL();
  }

  /*Find largest distances in about 120 deg sectors close to last front and back.*/
  lastFrontDirection = frontDirection;
  int maxDistance = distances[lastFrontDirection];
  for (i=1; i<7; i++) {
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

  DP("Front direction is: ");
  DPL(frontDirection);

  
  /*Run motors if allowed.*/
  if (!haltMotors) {
    int sector = int(frontDirection/10);//divide possible 40 directions into 4 (90 degree) sectors
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
  else {//stop motors
    analogWrite(motor[0], 0);
    analogWrite(motor[1], 0);
    analogWrite(motor[2], 0);
    analogWrite(motor[3], 0);
  }
}


void toggleRunning() {
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200) {
    haltMotors = !haltMotors;
  }
  lastInterruptTime = interruptTime;
}

