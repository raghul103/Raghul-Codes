// Initializing necessary libraries
#include "Arduino.pb.h" 
#include "pb_common.h"
#include "pb.h"
#include "pb_encode.h"
#include"string.h"
#include"cobs.h"
#include "pb_decode.h"

// Initializing data log for sample testing
float Temp1 = 23.69;
float Temp2 = 23.85;
float Curr1 = 0.07;
float Curr2 = 0.06;
float Curr3 = 0.64;
bool DAQ_flag = false;
float BattVolt = 15.11;
char state[128] = "READY";
int switch1 = 0;
int switch2 = 0;
int PWM1 = 1052;
int PWM2 = 1001;
bool Hflag = true; 
bool ADD_ZERO_BYTE = true;
char user[3] = "r";
int incoming;
int i = 0, flag = 0;

void setup() {
 
  Serial.begin(115200);

}

void loop() {

  // Buffer for encoding data logs
  uint8_t buffer[100];
  // Buffer for decoding command
  uint8_t buffer1[30];
 // command for encoding Acknowledgement
  uint8_t buffer2[30];
  
  size_t msg_len;
  Data message = Data_init_zero;
 

  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
 
 // Assigning data log values
  message.Temp1 = Temp1;
  message.Temp2 = Temp2;
  message.Curr1 = Curr1;
  message.Curr2 = Curr2;
  message.Curr3 = Curr3;
  message.DAQ_flag = DAQ_flag;
  message.BattVolt = BattVolt;
  strcpy(message.state, state);
  message.switch1 = switch1;
  message.switch2 = switch2;
  message.PWM1 = PWM1;
  message.PWM2 = PWM2;
  message.Hflag = Hflag;
  strcpy(message.user, user);
  message.Sflag = true;
  
 // Encoding Data logs
  bool status = pb_encode(&stream, Data_fields, &message);

  if (!status)
  {
      Serial.println("Failed to encode");
      return;
  }

  size_t ENCODED_MESSAGE_SIZE = getCOBSBufferSize(stream.bytes_written, ADD_ZERO_BYTE);

    // Allocate buffer big enough to hold result of COBS encoding.
  uint8_t cobs_buffer[ENCODED_MESSAGE_SIZE] = {0};
  size_t cobs_buffer_size = sizeof(cobs_buffer);
 

  size_t encoded_size = encodeCOBS(buffer, stream.bytes_written, cobs_buffer, cobs_buffer_size, ADD_ZERO_BYTE);
  Serial.write(cobs_buffer, encoded_size);
 // delay(500);
   
  // Changing data log values for every iteration
  Temp1 = Temp1 + 0.01;
  Temp2 = Temp2 + 0.01;
  Curr1 = Curr1 + 0.002;
  Curr2 = Curr2 + 0.002;
  Curr3 = Curr3 + 0.002;
  BattVolt = BattVolt + 0.01;  

  while(Serial.available())  // Reading the command from EC
  {
    buffer1[i] = Serial.read();
    i++;
    flag = 1;
    
    }
   
  if(flag == 1){  // If command is received
    command message1 = command_init_zero;
   
   // Decode the command
    pb_istream_t stream1 = pb_istream_from_buffer(buffer1, sizeof(buffer1));
    bool statusi = pb_decode(&stream1, command_fields, &message1); 

    commandAck message2 = commandAck_init_zero;
   // Set Acknowledgement
    if(message1.commandtype != 0){
     
     message2.statuss = 1;
    }

    else{
      message2.statuss = 0;
    }
   
   // Encode command Acknowledgement 
     pb_ostream_t streams = pb_ostream_from_buffer(buffer2, sizeof(buffer2));
     bool statuss = pb_encode(&streams, commandAck_fields, &message2);
     
      if (!statuss)
      {
      Serial.println("Failed to encode");
      return;
      }
              
      // Encode the protobuf encoded Acknowledgement again using COBS (This data should be COBS decoded and then protobuf decoded in pythn side)
      size_t ENCODED_MESSAGE_SIZE2 = getCOBSBufferSize(streams.bytes_written, ADD_ZERO_BYTE);

     // Allocate buffer big enough to hold result of COBS encoding.
       uint8_t cobs_buffer2[ENCODED_MESSAGE_SIZE2] = {0};
      size_t cobs_buffer_size2 = sizeof(cobs_buffer2);
 

      size_t encoded_size2 = encodeCOBS(buffer2, streams.bytes_written, cobs_buffer2, cobs_buffer_size2, ADD_ZERO_BYTE);

      Serial.write(cobs_buffer2, encoded_size2); // Send the encoded Acknowledgement

      flag = 0;   // Acknowledgement has been sent
    
  
  }
}
