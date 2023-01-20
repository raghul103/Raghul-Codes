#ifndef ENCODEDECODE_H_
#define ENCODEDECODE_H_
namespace ProtoLib{
  int encode(uint8_t cobs_buffer[], float temperature1, float Humidity1, float temperature2, float Humidity2, float current1LTC, int rpHealthVal, char state[], int trig1Read, int trig2Read, int PWM1, int PWM2, bool Hflag, char input[], bool Sflag);
  char decode_char(uint8_t buffer1[], int *ptr, int *spi_config); 
}
#endif
