/*Include this file if using 
 * VL53L0X micro LIDAR sensors and
 * ... controller. */

/*** Pin connections ***/

const int radarServo = 11;

const int motor[] = {5, 6, 9, 10};

const int runToggleButton = 2;

byte sensor1pin = 3;
byte sensor2pin = 2;
byte sensor1addr = 22;
byte sensor2addr = 23;
unsigned int sensorTimeout = 500;


/*** Library object initializations. ***/

VL53L0X sensor1;
VL53L0X sensor2;


/*Function for setting Lidar SPI addresses*/

void lidarInit() {
  
  pinMode(sensor1pin, OUTPUT);
  digitalWrite(sensor1pin, LOW);
  pinMode(sensor2pin, OUTPUT);
  digitalWrite(sensor2pin, LOW);

  delay(500);
  Wire.begin();

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
      DPL("Sensor1 not responding.");
    }
  #endif

  sensor1.init();
  sensor1.setTimeout(sensorTimeout);
  delay(10);
  sensor2.init();
  sensor2.setTimeout(sensorTimeout);
  
}

