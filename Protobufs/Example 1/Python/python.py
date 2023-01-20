import serial     #Importing necessary libraries
import python_pb2 as data  
from cobs import cobs
  
count = 0         #Initializing logs count number

while True:      
 
 try:
  count = count + 1    
  data_obj = data.Data()                   # Create an object for the class Data
  ser = serial.Serial('/dev/ttyACM0', 115200)   
  byte_string = ser.read_until(expected = b'\x00', size = 80)    # Read until zero-byte (de-limiting)
  cobs_decoded = cobs.decode(byte_string[:-1])           # Decode the cobs encoded byte string
  data_obj.ParseFromString(cobs_decoded)       # Decode cobs decoded byte string using Protobufs 
  
  print(data_obj)   # Print the data log on to the screen
  print(count)

 except:     # The first byte string coming into python buffer is not complete. So it throws an error 
            # on decoding it for the first time. Then this exception block will be executed (reading data from buffer again)

  byte_string = ser.read_until(expected = b'\x00', size = 80)  # Read until zero-byte (de-limiting)
  decoded = cobs.decode(byte_string[:-1])    # Decode the cobs encoded byte string
  data_obj.ParseFromString(decoded)          # Decode cobs decoded byte string using Protobufs 
  
  print(data_obj)
  print(count)
  
 else:
    continue

