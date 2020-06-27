// Based on https://github.com/sparkfun/SparkFun_MMA8452Q_Arduino_Library/blob/master/examples/Example1_BasicReading/Example1_BasicReading.ino

#include <SPI.h>
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

// create instance of the MMA8452 class
MMA8452Q accel(0x1C);                   

void setup() {
  Serial.begin(9600);
  Serial.println("MMA8452Q Basic Reading Code!");
  Wire.begin();

  if (accel.begin(Wire, 0x1C) == false) {
    Serial.println("Not Connected. Please check connections and read the hookup guide.");
    while (1);
  }
}

void loop() {
  if (accel.available()) {      // Wait for new data from accelerometer
    // Acceleration of x, y, and z directions in g units
    Serial.print(accel.getCalculatedX(), 3);
    Serial.print("\t");
    Serial.print(accel.getCalculatedY(), 3);
    Serial.print("\t");
    Serial.print(accel.getCalculatedZ(), 3);
    Serial.println();
  }
}