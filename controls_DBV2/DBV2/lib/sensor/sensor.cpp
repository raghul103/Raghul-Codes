#include "mbed.h"
#include "Arduino.h"
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#define averageValue 500
#define adcRes 8

using namespace mbed;

namespace sense
{
 float sensorRead(int sensorPin) {
  analogReadResolution(adcRes);
  float sensorValue = 0;
  for (int i = 0; i < averageValue; i++)
  {
    sensorValue += analogRead(sensorPin);
    delayMicroseconds(100);        // delay in between reads for stability
  }
  sensorValue = sensorValue / averageValue;
  return sensorValue;
}
}
