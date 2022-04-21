// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include "BuBp2.h"


MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float ag, timer, meanaz, accel, vel, disp, oldaccel, oldvel, olddisp, dt = 5e-4;
FilterBuBp2 accels = FilterBuBp2(), vels = FilterBuBp2(), disps = FilterBuBp2();


void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Wire.setClock(400000L);

  // initialize serial communication
  Serial.begin(115200);

  // initialize device
  Serial.println("Initializing I2C devices...");
  mpu.initialize();
  mpu.setRate(350);
  mpu.setDLPFMode(2);

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // calibrate offsets
  mpu.setXAccelOffset(0); mpu.setYAccelOffset(0); mpu.setZAccelOffset(0);
  mpu.CalibrateAccel(10);

  meanaz = 0;
  accel = 0; oldaccel = 0;
  vel = 0;  oldvel = 0;
  disp = 0; olddisp = 0;

  for (int i = 0; i < 5000; i++) {
    mpu.getAcceleration(&ax, &ay, &az);
    meanaz += az;
  }
  ag = meanaz / 5000;

}

void loop() {
  mpu.getAcceleration(&ax, &ay, &az);               //read acceleration data
  az -= ag;                                         //eliminate graviational acceleration
  oldaccel = accels.step(az);
  accel = accels.val();

  vel = oldvel + ((oldaccel + accel) * dt / 2);     //calculate new velocity
    oldvel = vels.step(vel);                        //update old velocity
    vel = vels.val();                               //and filter new velocity

  disp = olddisp + ((vel + oldvel) * dt / 2);       //calculate new displacement
  olddisp = disps.step(disp);                       //update old displacement
  disp = disps.val();                               //and filter new displacement



  //Serial.print(accel); Serial.print("\t");
  //Serial.print(vel); Serial.print("\t");
  Serial.println(disp * 100000);                    //amplify to meaningful value and display


}
