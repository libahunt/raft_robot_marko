/*Include this file if using 
 * VL53L0X micro LIDAR sensors. */

/*** Pin connections ***/

const int radarServo = 4;

const int motor[] = {5, 6, 9, 10};

const int runToggleButton = 2;

byte sensor1pin = 8;
byte sensor2pin = 7;
byte sensor1addr = 22;
byte sensor2addr = 23;
unsigned int sensorTimeout = 500;

const int motorCoefPot[] = {A0, A1, A2, A3};

const int rgbData = 1;
const int rgbClock = 3;


/*** Library object initializations. ***/

VL53L0X sensor1;
VL53L0X sensor2;


/*Function for setting Lidar SPI addresses*/

void lidarInit() {
  
  pinMode(sensor1pin, OUTPUT);
  pinMode(sensor2pin, OUTPUT);
  digitalWrite(sensor1pin, LOW);
  digitalWrite(sensor2pin, LOW);

  delay(500);
  Wire.begin();
  delay(500);

  pinMode(sensor1pin, INPUT);
  delay(150);
  sensor1.init(true);
  delay(100);
  sensor1.setAddress(sensor1addr);

  pinMode(sensor2pin, INPUT);
  delay(150);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress(sensor2addr);

  DPL("Lidar addresses set");

  #ifdef DEBUG
    DPL("Testing lidar devices");
    Wire.beginTransmission(sensor1addr);
    if (Wire.endTransmission () == 0) {
      DPL("Sensor1 OK");
    }
    else {
      DPL("Sensor1 not responding.");
    }
    Wire.beginTransmission (sensor2addr);
    if (Wire.endTransmission () == 0) {
      DPL("Sensor2 OK");
    }
    else {
      DPL("Sensor2 not responding.");
    }
  #endif

  sensor1.init();
  sensor1.setTimeout(sensorTimeout);
  delay(10);
  sensor2.init();
  sensor2.setTimeout(sensorTimeout);
  
}

