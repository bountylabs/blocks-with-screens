// Based on https://github.com/sparkfun/SparkFun_MMA8452Q_Arduino_Library/blob/master/examples/Example1_BasicReading/Example1_BasicReading.ino

#include <SPI.h>
#include <Wire.h>
#include <SparkFun_MMA8452Q.h>
#include <Adafruit_NeoPixel.h>

// Create instance of the MMA8452 class
MMA8452Q accel(0x1C);                   

// Pin setup
#define LED_PIN 4 // D2
Adafruit_NeoPixel pixels(1, LED_PIN, NEO_GRB);

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
    float x = accel.getCalculatedX();
    float y = accel.getCalculatedY();
    float z = accel.getCalculatedZ();
    Serial.print(x, 3);
    Serial.print("\t");
    Serial.print(y, 3);
    Serial.print("\t");
    Serial.print(z, 3);
    Serial.println();

    pixels.begin();
    pixels.clear(); // Set all pixel colors to 'off'
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    uint8_t xbyte = min(max((int)(x*128+127), 0), 255);
    uint8_t ybyte = min(max((int)(y*128+127), 0), 255);
    uint8_t zbyte = min(max((int)(z*128+127), 0), 255);

    Serial.print(xbyte);
    Serial.print("\t");
    Serial.print(ybyte);
    Serial.print("\t");
    Serial.print(zbyte);
    Serial.println();

    pixels.setPixelColor(0, pixels.Color(xbyte, ybyte, zbyte));
    pixels.show();   // Send the updated pixel colors to the hardware.

    Wire.begin();
    accel.begin(Wire, 0x1C);
  }
}

