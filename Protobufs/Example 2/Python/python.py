import serial     #Importing necessary libraries
import python_pb2 as data  
from cobs import cobs
import keyboard   # For detecting keypress

# Initializing state flags  
iflag = 0
rflag = 0
cflag = 0

# Initializing logs count number
count = 0

while True:
  ser = serial.Serial('/dev/ttyACM0', 15200)

  def DATA():       # For reading data logs
       
    try:
      
      data_obj = data.Data()                   # Create an object for the class Data
      commandack_obj = data.commandAck()       # Create an object for the class commandAck (Acknowledgement)
      byte_string = ser.read_until(expected = b'\x00', size = 80)    # Read until zero-byte (de-limiting)
      cobs_decoded = cobs.decode(byte_string[:-1])           # Decode the cobs encoded byte string
      commandack_obj.ParseFromString(cobs_decoded)       # Decode cobs decoded byte string using Protobufs 
      print(commandack_obj)
    #  print(byte_string)
      byte_string = ser.read_until(expected = b'\x00', size = 80)    # Read until zero-byte (de-limiting)
      cobs_decoded = cobs.decode(byte_string[:-1])           # Decode the cobs encoded byte string
      data_obj.ParseFromString(cobs_decoded)       # Decode cobs decoded byte string using Protobufs 
      print(data_obj)   # Print the data log on to the screen
     # print(byte_string)
      
            
    except:     
       
      byte_string = ser.read_until(expected = b'\x00', size = 80)    # Read until zero-byte (de-limiting)
      cobs_decoded = cobs.decode(byte_string[:-1])           # Decode the cobs encoded byte string
      commandack_obj.ParseFromString(cobs_decoded)       # Decode cobs decoded byte string using Protobufs 
      print(commandack_obj)          # Print the Acknowledgement data  
            	   
      byte_string = ser.read_until(expected = b'\x00', size = 80)    # Read until zero-byte (de-limiting)
      cobs_decoded = cobs.decode(byte_string[:-1])           # Decode the cobs encoded byte string
      data_obj.ParseFromString(cobs_decoded)       # Decode cobs decoded byte string using Protobufs 
      print(data_obj) 

      

  def command_encode(i):
 
    command_obj = data.command()          #Create object for class command
    if (i == 1):
      command_obj.commandtype = 1
      command_obj.datas = "i"
      command_byte = command_obj.SerializeToString()    
      ser.write(command_byte)       # Send IDLE command to Portenta

    if (i == 2):
      command_obj.commandtype = 2
      command_obj.datas = "r"
      command_byte = command_obj.SerializeToString()
      ser.write(command_byte)      # Send READY command to Portenta
     
    if (i == 3):
      command_obj.commandtype = 3
      command_obj.datas = "c"
      command_byte = command_obj.SerializeToString()
      ser.write(command_byte)     # Send IMAGING command to Portenta
    

  count = count + 1
  print(count)
  DATA()        # Function for reading the data logs

  if iflag == 0 and keyboard.is_pressed('i'):  # If 'i' is pressed
      command_encode(1)    # Function to Encode and send IDLE command to EC 
      iflag = 1        
      rflag = 0
      cflag = 0
      
  if rflag == 0 and keyboard.is_pressed('r'):  # If 'r' is pressed
      command_encode(2)   # Function to Encode and send READY command to EC
      iflag = 0
      rflag = 1
      cflag = 0  
      
  if cflag == 0 and keyboard.is_pressed('c'):  # If 'c' is pressed
      command_encode(3)  # Function to Encode and send IMAGING command to EC
      iflag = 0
      rflag = 0
      cflag = 1

