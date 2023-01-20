#ifndef SPI_ERROR_H_
#define SPI_ERROR_H_
#include<Arduino.h>
namespace abc{
void spiWriteSequence(int spi_config);
int errorDisplay(float temperature1, float humidity1, float temperature2, float humidity2,  float current1LTC);
}
#endif
