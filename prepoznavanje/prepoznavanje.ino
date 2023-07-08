#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
//#include <tensorflow/lite/version.h>

#include "model.h"

MMA8452Q accel; // Create an instance of the MMA8452Q class

const float accelerationThreshold = 2.5;
const int numSamples = 100;
int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "left",
  "right"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  // Set the scale to 8G
  // Set the output data rate to 100 Hz
  accel.init(SCALE_8G, ODR_100);

  Serial.println();

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  // Serial.println(tflModel->version());
  // if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
  //   Serial.println("Model schema mismatch!");
  //   while (1);
  // }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);


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

  Serial.println("Significant motion detected!");

  while (samplesRead < numSamples) {
    if (accel.available()) {
      accel.read();

      Serial.print("Read sample number: ");
      Serial.println(samplesRead);

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 3 + 0] = (x + 8.0) / 16.0;
      tflInputTensor->data.f[samplesRead * 3 + 1] = (y + 8.0) / 16.0;
      tflInputTensor->data.f[samplesRead * 3 + 2] = (z + 8.0) / 16.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }

        // Loop through the output tensor values from the model
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 3);
        }
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
