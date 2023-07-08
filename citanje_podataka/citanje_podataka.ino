#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

MMA8452Q accel; // Create an instance of the MMA8452Q class

const float accelerationThreshold = 2.5;
const int numSamples = 100;
int samplesRead = numSamples;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Acceleration threshold for reading the data");
  
  // Set the scale to 8G
  // Set the output data rate to 100 Hz
  accel.init(SCALE_8G, ODR_100);
  Serial.println("Initialized successfully");
}

void loop() {
  float x, y, z;

  while (samplesRead == numSamples) {
    if (accel.available()) {
      accel.read();
      
      x = accel.cx;
      y = accel.cy;
      z = accel.cz;
      float totalAcceleration = fabs(x) + fabs(y) + fabs(z);
      
      if (totalAcceleration > accelerationThreshold) {
        samplesRead = 0;
        break;
      }
    }
  }

  while (samplesRead < numSamples) {
    if (accel.available()) {
      accel.read();

      samplesRead++;
      
      printCalculatedAccels();
      Serial.println();

      if (samplesRead == numSamples) {
        Serial.println();
      }
    }
  }
}

// The function demonstrates how to use the accel.x, accel.y and
//  accel.z variables.
// Before using these variables you must call the accel.read()
//  function!
void printAccels()
{
  Serial.print(accel.x);
  Serial.print(",");
  Serial.print(accel.y);
  Serial.print(",");
  Serial.print(accel.z);
}

// This function demonstrates how to use the accel.cx, accel.cy,
//  and accel.cz variables.
// Before using these variables you must call the accel.read()
//  function!
void printCalculatedAccels()
{
  Serial.print(accel.cx, 3);
  Serial.print(",");
  Serial.print(accel.cy, 3);
  Serial.print(",");
  Serial.print(accel.cz, 3);
}
