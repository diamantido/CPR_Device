// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro;

int16_t ax, ay, az;


int dt = 25;
long timer = 0;
float pos = 0;
float vel = 0;
float g = 9.80665;
float a_c;

void red1() {
  digitalWrite(9, LOW);
  digitalWrite(10,LOW);
  digitalWrite(8, HIGH);
}

void green() {
  digitalWrite(8, LOW);
  digitalWrite(10,LOW);
  digitalWrite(9, HIGH);
}

void red2() {
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10,HIGH);
}

void piezo() {
  if (timer % 300 == 0) {
    tone(11, 1500);
  }
  if (timer % 600 == 0) {
    noTone(11);
  }
}

void setup() {
   // join I2C bus (I2Cdev library doesn't do this automatically)
   #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
       Wire.begin();
   #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
       Fastwire::setup(400, true);
   #endif

   // initialize serial communication
   // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
   // it's really up to you depending on your project)
   Serial.begin(38400);

   // initialize device
   Serial.println("Initializing I2C devices...");
   accelgyro.initialize();

   // verify connection
   Serial.println("Testing device connections...");
   Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

   // use the code below to change accel/gyro offset values
   Serial.println("Updating internal sensor offsets...");
   /*
   accelgyro.setXAccelOffset(8400);
   accelgyro.setYAccelOffset(550);
   accelgyro.setZAccelOffset(5100);
   */
   Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76 //16516
   Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359 //-552
   Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688 //-5092
   Serial.print("\n");

   //calculate acceleration constant
   accelgyro.getAcceleration(&ax,&ay,&az);
   a_c = az/g;
    
   // configure Arduino LED pin for output
   pinMode(8, OUTPUT);
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(11, OUTPUT);
}


void loop() {
  //read data from accelerometer and update displacement value
  accelgyro.getAcceleration(&ax,&ay,&az);
  az /= a_c;
  // divide by 25400 to get inches
  
  pos += vel*dt+1/2*az*(dt^2);
  vel += az*dt;
  if(pos > 1.0 && pos < 2.0) {
    red1();
  } else if(pos >= 2.0 && pos <= 2.4) {
    green();
  } else if(pos > 2.4 && pos < 3.4) {
    red2();
  } else {
    Serial.println("Invalid position value: " + String(pos));
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
  }
  piezo();
  //Serial.print(pos);
  timer += dt;
  Serial.println("Delay = " + String(millis()-timer) +";");
}
