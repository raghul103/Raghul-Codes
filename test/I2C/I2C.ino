#include <Wire.h>

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop()
{
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(20);
  Wire.endTransmission();
  delay(500);
}
