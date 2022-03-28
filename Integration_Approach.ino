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


int16_t a, ax, ay, az;
float accel = 0, accel_old = 0, vel = 0, vel_old = 0, disp = 0, disp_old = 0, a_c, a_g, dt, g = 9.80655;
long timer;


void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  // initialize serial communication
  Serial.begin(115200);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    //calculate acceleration constant
  for (int i = 0; i < 10000; i++) {
    accelgyro.getAcceleration(&ax, &ay, &az);
    a_c += az;
  }
  a_c /= 10000; 
  Serial.println(a_c);
  a_g = a_c/g;
}

void loop() {
  // put your main code here, to run repeatedly:
  a = 0;
  timer = micros();
  for (int i = 0; i < 100; i++) {
    accelgyro.getAcceleration(&ax, &ay, &az);
    a += az;
  }
  dt = (micros()-timer)/1000000;
  accel_old = accel;
  accel = float(a/100)-a_c; 
  vel_old = vel;
  vel = vel_old + ((accel+accel_old)/2*dt);
  disp_old = disp;
  disp = disp_old + ((vel+vel_old)/2*dt);
  Serial.print("Acceleration: ");
  Serial.print(accel);
  Serial.print("; Velocity: ");
  Serial.print(vel);
  //Serial.print("; Displacement: ");
  //Serial.print(disp);
  Serial.print("\n");
  delay(dt);
}
