#include "mbed.h"
#include "Arduino.h"
#include "PCAL6254.h"
#include <Wire.h>

using namespace mbed;

namespace RFconfig
{

PCAL6524 io;
 
void RF(int user_config)
{
  int count = 1, place, lastBit;
  if(user_config == 0)
  {
    io.remotedigitalWrite(P0_0, LOW);  // 5 V
    io.remotedigitalWrite(P0_1, LOW);  // 5 V
    io.remotedigitalWrite(P0_2, LOW);  // 5 V
    io.remotedigitalWrite(P0_3, LOW);  // 5 V
    io.remotedigitalWrite(P0_4, LOW);  // 5 V
    io.remotedigitalWrite(P0_5, LOW);  // 5 V
    io.remotedigitalWrite(P0_6, LOW);  // 5 V
    io.remotedigitalWrite(P0_7, LOW);  // 5 V
    io.remotedigitalWrite(P1_0, LOW);  // 5 V
    io.remotedigitalWrite(P1_1, LOW);  // 5 V
    io.remotedigitalWrite(P1_2, LOW);  // 5 V
    io.remotedigitalWrite(P1_3, LOW);  // 5 V
    io.remotedigitalWrite(P1_4, LOW);  //-5 V
    io.remotedigitalWrite(P1_5, LOW);  //-5 V
    io.remotedigitalWrite(P1_6, LOW);  //-5 V
    io.remotedigitalWrite(P1_7, LOW);  //-5 V
    io.remotedigitalWrite(P2_0, LOW);  //-5 V
    io.remotedigitalWrite(P2_1, LOW);  //-5 V
  }
    
  while(user_config != 0)
  {
    lastBit = user_config & 1;   // Extract the last Bit of the new (after Shift Right) user_config
    if(lastBit == 1)
      place = count;
    user_config = user_config>>1;   // Shift Right by one bit
    if(place == 1)
      io.remotedigitalWrite(P0_0, HIGH);
    if(place == 2)
      io.remotedigitalWrite(P0_1, HIGH);
    if(place == 3)
      io.remotedigitalWrite(P0_2, HIGH);
    if(place == 4)
      io.remotedigitalWrite(P0_3, HIGH);
    if(place == 5)
      io.remotedigitalWrite(P0_4, HIGH);
    if(place == 6)
      io.remotedigitalWrite(P0_5, HIGH);
    if(place == 7)
      io.remotedigitalWrite(P0_6, HIGH);
    if(place == 8)
      io.remotedigitalWrite(P0_7, HIGH);
    if(place == 9)
      io.remotedigitalWrite(P1_0, HIGH);
    if(place == 10)
      io.remotedigitalWrite(P1_1, HIGH);
    if(place == 11)
      io.remotedigitalWrite(P1_2, HIGH);
    if(place == 12)
      io.remotedigitalWrite(P1_3, HIGH);
    if(place == 13)
      io.remotedigitalWrite(P1_4, HIGH);
    if(place == 14)
      io.remotedigitalWrite(P1_5, HIGH);
    if(place == 15)
      io.remotedigitalWrite(P1_6, HIGH);
    if(place == 16)
      io.remotedigitalWrite(P1_7, HIGH);
    if(place == 17)
      io.remotedigitalWrite(P2_0, HIGH);
      if(place == 18)
      io.remotedigitalWrite(P2_1, HIGH);
   
    count++;
  }  
}

}
