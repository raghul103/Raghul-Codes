## Problem Statement:
   ESP32 sends the data logs to client (python) using serial communication.
## Protobuf encoding-decoding:
   The data is encoded using nanopb in the server side (ESP32) and the data received by client is dedoded using google protobufs.  
## COBS encoding-decoding:
   Protocol buffers does not support de-limiting characters at the end of a data packet. So we again encode the protobuf encoded bitstream using COBS Algorithm. This is known as COBS encoding.
   Reference: https://blog.mbedded.ninja/programming/serialization-formats/consistent-overhead-byte-stuffing-cobs/
## Data Flow:
   Data in ESP32 ----> Protobuf encoding (ESP32) -----> COBS encoding (ESP32) -----> COBS decoding (client) -----> Protobuf decoding (client)
## Statistics: (Using Arduino UNO; baud rate = 9600)
  1. 9.97 logs/sec (A log contains the information of all temperatures, currents, flag status, PWM width, etc.)
  2. 992.67 bytes/sec
  3. Overhead for COBS encoding = 2 bytes
  4. Actual size of message = 198 bytes
  5. Size of data after protobuf and COBS encoding = 56 bytes
  6. Size of log in drone V1 code = 61 bytes
