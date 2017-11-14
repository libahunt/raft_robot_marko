/*
 * At servo start (0) position, first sensor direction is 0 degrees. 
 * This should be opposite of motor 0 push direction.
 * 
 * Choice between ultrasonic sensors or VL53L0X lidar can be made by 
 * commenting line 15.
 * 
 * Pin connections are in files layoutUltasonic.h or layoutVL53L0X.h respectively.
*/


#define DEBUG /*Comment this line out in production then all DP Serial instructions are ignored.*/
#include "DebugUtils.h"/*Leave this in, otherwise you get errors.*/

//#define PING //Keep this when using ultrasound distance sensors. 
             //Comment out if using VL53L0X micro-lidar. VL53L0X does not work with Atmega328.

#include <Servo.h>

#include "LPD8806.h"
#include "SPI.h"

#ifdef PING

  #include <NewPing.h>
  #include "layoutUltrasonic.h"
  
#else

  #include <Wire.h>
  #include <VL53L0X.h>
  #include "layoutVL53L0X.h"
  
#endif

/*** Library object initializations. ***/

Servo sweeper;

LPD8806 lights = LPD8806(10, rgbData, rgbClock);

/*** Settings ***/

const int fullPWM = 255;
const int servoZero = 0; //degrees at which the servo does not damage itself
const int halfSector = 7; //Half of the sector width (out of 40 directions) 
//to use when deciding new frontDirection. 7*2+1=15 directions makes it 135 degrees.


/*** Other variables. ***/
boolean haltMotors = true; 
int PWMs[4];
float motorPWMcoef[] = {1, 1, 1, 1};

unsigned long interruptTime; //for debouncing drive/stall mode button
unsigned long lastInterruptTime = 0; //for debouncing drive/stall mode button

int distances[40];//40 directions to measure makes each one 9 degrees wide
int frontDirection = 0; //direction sector (0-39)
int lastFrontDirection = 0;
bool scanDirection = 1;


void setup() {
  
  sweeper.attach(radarServo);

  sweeper.write(servoZero);
  delay(500);
  
  /*Motors.*/
  for (int i=0; i<4; i++) {
    pinMode(motor[i], OUTPUT);
  }

  pinMode(runToggleButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(runToggleButton), toggleRunning, LOW);

  #ifdef DEBUG
    Serial.begin(9600);
    DPL("Start");
  #endif

  #ifndef PING
    lidarInit();
  #endif

  lights.begin();
  lights.show();

  readMotorCoefPots();
  
}

void loop() {

  //Move servo to 20 different positions and take 2 opposite direction measurements in each.
  if (scanDirection == 1) {
    for (int i=0; i<20; i++) {
      sweeper.write(servoZero + i*9);
      delay(100);
      takeAMeasurement(i);
    }
  }
  else {
    for (int i=19; i>=0; i--) {
      sweeper.write(servoZero + i*9);
      delay(100);
      takeAMeasurement(i);
    }
  }

  scanDirection = !scanDirection;

  DPL("Measurements:");
  for (int i=0; i<40; i++) {
    DP(i);
    DP("\t");
  }
  DPL();
  for (int i=0; i<40; i++) {
    DP(distances[i]);
    DP("\t");
  }
  DPL();

  /*Find largest distances in set sectorwidth close to last front and back.*/
  lastFrontDirection = frontDirection;
  int maxDistance = distances[lastFrontDirection];
  for (int i=1; i<halfSector; i++) {
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

  DP("Front direction in degrees is: ");
  DPL(frontDirection*9);

  showDirectionLights();
  
  /*Run motors if allowed.*/
  if (!haltMotors) {
    runMotors(frontDirection);
  }
  else {
    stopMotors();
  }
}


void toggleRunning() {
  interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200) {
    haltMotors = !haltMotors;
  }
  lastInterruptTime = interruptTime;
}

void takeAMeasurement(int i) {
  
  #ifdef PING
    
      distances[i] = firstSonar.ping_cm();
      distances[20 + i] = secondSonar.ping_cm();
      
    #else
  
      int dist = sensor1.readRangeSingleMillimeters();
      if (!sensor1.timeoutOccurred()) {
        distances[i] = dist / 10;
      } else {
        distances[i] = 0;
        DPL("Lidar 1 failure.");
      }
      dist = sensor2.readRangeSingleMillimeters();
      if (!sensor2.timeoutOccurred()) {
        distances[20 + i] = dist / 10;
      } else {
        distances[20 + i] = 0;
        DPL("Lidar 2 failure.");
      }
      
    #endif
}


void readMotorCoefPots() {
  for (int i=0; i<4; i++) {
    motorPWMcoef[i] = map( analogRead(motorCoefPot[i]), 0, 1023, 0.5, 1.0 );
    DP("motorPWMcoef[");
    DP(i);
    DP("] = ");
    DP(motorPWMcoef[i]);
    DP("\t\t");
    delay(50);
  }
  DPL("");
  
}

