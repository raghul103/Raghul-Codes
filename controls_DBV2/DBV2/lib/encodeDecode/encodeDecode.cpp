#include "Arduino.h"
#include "Ardcheck.pb.h" 
#include "pb_common.h"
#include "pb.h"
#include "pb_encode.h"
#include"string.h"
#include"cobs.h"
#include "pb_decode.h"

bool ADD_ZERO_BYTE = true;

namespace ProtoLib{
int encode(uint8_t cobs_buffer[], float temperature1, float Humidity1, float temperature2, float Humidity2, float current1LTC, int rpHealthVal, char state[], int trig1Read, int trig2Read, int PWM1, int PWM2, bool Hflag, char input[], bool Sflag)
{
 
  uint8_t buffer[64];
  Data message = Data_init_zero;
 
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  message.temperature1 = temperature1;
  message.Humidity1 = Humidity1;
  message.temperature2 = temperature2;
  message.Humidity2 = Humidity2;
  message.current1LTC = current1LTC;
  message.DAQ_flag = rpHealthVal;
  strcpy(message.state, state);
  message.switch1 = trig1Read;
  message.switch2 = trig2Read;
  message.PWM1 = PWM1;
  message.PWM2 = PWM2;
  message.Hflag = Hflag;
  strcpy(message.input, input);
  message.Sflag = Sflag;
  bool status = pb_encode(&stream, Data_fields, &message);
 
  if(!status)
    return -1;
    
  size_t cobs_buffer_size = getCOBSBufferSize(stream.bytes_written, ADD_ZERO_BYTE);
  size_t encoded_size = encodeCOBS(buffer, stream.bytes_written, cobs_buffer, cobs_buffer_size, ADD_ZERO_BYTE);
  return encoded_size;
   
}

char decode_char(uint8_t bufferk[], int *ptr, int *spi_config)
{
  char return_char;
  command message1 = command_init_zero;
  pb_istream_t streamk = pb_istream_from_buffer(bufferk, sizeof(bufferk));
  bool statusi = pb_decode(&streamk, command_fields, &message1); 
  *ptr = message1.RG_config;
  *spi_config = message1.spi_config;
  return_char = message1.character[0];
  return return_char;
}

}
