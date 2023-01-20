
// Include all necessary libraries
#include "mbed.h"
#include "Arduino.h"
#include "spi_error.h"     // Library for SPI configuration and displaying error
#include "encodeDecode.h"   // Library for Protocol buffers
#include "sensor.h"        
#include "RFtrig.h"         // Library for configuring voltage regulators
#include "SHTSensor.h"      
#include "PCAL6254.h"
#include "LTC4151.h"
#include <Wire.h>

 
//SensirionI2CSht4x sht4x;

// define Trigger pins 
#define switchTrig1 D13
#define switchTrig2 D14

// define current and voltage sensor pins
#define battHealth A3

// define PWM input pins
#define droneTrig1 D1
#define droneTrig2 D2

// imaging mode triggers
#define EC1trig D0
#define EC2trig D20

// RP triggers
#define RP1trig D3
#define RP2trig D4
#define LCCtrig D19

// rp gpio pin
#define rpHealth D6

// Delays
#define SERIAL_DELAY 1000
#define RP_DELAY 5000

// SPI number of registers to write
#define NO_OF_REGISTERS 131

#define CNTEMP 5

using namespace mbed;
using namespace abc;
using namespace ProtoLib;
using namespace sense;
using namespace RFconfig;

SHTSensor sht1(SHTSensor::SHT3X);
SHTSensor sht2(SHTSensor::SHT3X);
PCAL6524 io;  //Address Pin is tied to ground (DEFAULT)

// LTC4151 current sensor
LTC4151 sensor1;

int RF_config, SPI_config;

// safety state timers
unsigned long startImaging = 0;
unsigned long overflowImaging = 0;

unsigned long lastTime = 0;
unsigned long lastTimeRP = 0;

// Flags
bool HFlag;
bool SFlag = true;

// read user input
char controls_mode[2] = {'x'};  // Initializing with a random character
char input;

// PWM rise and fall times
int rise_time1, rise_time2, fall_time1, fall_time2;

// Character form of Trigger, PWM and USER modes  (s, p, u)
char control_input[2];
char control_mode;

// defining MOSI,MISO,SCK,CS*
SPI spi(PC_3, PC_2, PI_1, PI_0);

//define temp sensors
//Adafruit_MLX90614 mlx1 = Adafruit_MLX90614();

// trigger values
int trig1Read;
int trig2Read;
int pwm1Read, pwm1;
int pwm2Read, pwm2;
int rpHealthVal = 0;
u_int64_t counter = 0;

// TIme stamps for PWM width measurement
int stamp1, stamp2;

// Flags used for state change
int readyTrigFlag = 0, readyPWMFlag = 0, cameraFlag = 0;
int idleTrigFlag = 1, idlePWMFlag = 1, idleUserFlag = 1; 

// FSM States IDLE, READY, IMAGING
char fsmState[9];  

// Buffer for storing the encoded data coming from EC
uint8_t buffer1[30];

//Health function
void healthDataCheck() {
  
  float temperature1, temperature2, Humidity1, Humidity2;
  //SHT temp sensors
  if (sht1.readSample()) {
    temperature1 = sht1.getTemperature();
    Humidity1 = sht1.getHumidity();
  } 
  if (sht2.readSample()) {
    temperature2 = sht2.getTemperature();
    Humidity2 = sht2.getHumidity();
  } 
  
  float current1LTC = sensor1.getLoadCurrent(0.1);

  // rp health
  rpHealthVal = digitalRead(rpHealth);

  // digital input triggers
  trig1Read = digitalRead(switchTrig1);
  trig2Read = digitalRead(switchTrig2);

  //batthealth
  float battHVal = sensorRead(battHealth);
  battHVal = 0.0609 * battHVal - 0.185;

  counter = counter + 1;
  
    int timeElapsed = millis();
    if(timeElapsed-lastTime>SERIAL_DELAY){
    
    // Array for storing the final encoded data 
    uint8_t cobs_buffer[64] ={};
   /* tempValueAmb = 20.39;
    tempValueObj = 30.32;
    currentRelay1Val = 0.01;
    current15vDCVal = 0.01;
    current5vDCVal = 0.01;
    rpHealthVal = 1;
    battHVal = 10;                       //Sample Values for checking Protocol buffers
    strcpy(fsmState, "IDLE");
    trig1Read = 2;
    trig2Read = 2;
    pwm1Read = 1200;
    pwm2Read = 1200;
    HFlag = true;
    user_input[0] = 'i';
    SFlag = true;
   */
   
   // Encode data using Protocol Buffers and COBS 
    int encoded_size = encode(cobs_buffer, temperature1, Humidity1, temperature2, Humidity2, current1LTC, rpHealthVal, fsmState, trig1Read, trig2Read, pwm1Read, pwm2Read, HFlag, controls_mode, SFlag);
    Serial3.write(cobs_buffer, encoded_size);   //For UART3 communication
    lastTime = millis();
    }

    // For displaying the temperature or current which has exceeded its Threshold (in Binary form)
    int err = errorDisplay(temperature1, Humidity1, temperature2, Humidity2, current1LTC);

    if(err == 0)
      HFlag = 1;
    else{
          HFlag = 0;
          Serial.println(err, BIN);
    }
}

// PWM 1 measurement
void ISR1() 
{
  if (digitalRead(droneTrig1) == 1) 
    rise_time1 = micros();
  
  if(digitalRead(droneTrig1) == 0)
  {
    fall_time1 = micros();
    pwm1 = fall_time1 - rise_time1;
    
    if(pwm1 >1000)            // To eliminate any jitters below 1000 micros
       pwm1Read = pwm1;
  }
}

// PWM 2 measurement
void ISR2()
{
  if (digitalRead(droneTrig2) == 1) 
    rise_time2 = micros();
    
  if(digitalRead(droneTrig2) == 0)
  {
    fall_time2 = micros();
    pwm2 = fall_time2 - rise_time2;
    
    if(pwm2 >1000)            // To eliminate any jitters below 1000 micros
       pwm2Read = pwm2; 
  }
}

void setup()
{
  Serial.begin(115200);  // Initialization of serial communication.
  Serial3.begin(115200);
  Serial.setTimeout(100);
  // Serial.println("code uploaded");
  lastTime = millis();
  lastTimeRP = millis();

  //Pins for SPI
  pinMode(PC_2, INPUT);
  pinMode(PC_3, OUTPUT);
  pinMode(PC_1, OUTPUT);
  pinMode(PI_0, OUTPUT);  
  pinMode(EC1trig, OUTPUT);            //EC trigger
  pinMode(EC2trig, OUTPUT);            //EC trigger
  pinMode(RP1trig, OUTPUT);            //EC trigger
  pinMode(RP2trig, OUTPUT); 
  //Pin config for analog and digital input
  pinMode(battHealth, INPUT);
  pinMode(droneTrig1, INPUT_PULLDOWN);
  pinMode(droneTrig2, INPUT_PULLDOWN);
  pinMode(switchTrig1, INPUT_PULLDOWN);
  pinMode(switchTrig2, INPUT_PULLDOWN);
  pinMode(rpHealth, INPUT_PULLDOWN);

  //initial output
  digitalWrite(EC1trig, LOW);          //EC trigger off
  digitalWrite(EC2trig, LOW);          //EC trigger off
  //SPI Configuration
  spi.frequency(1000000);             // Set up your frequency.
  spi.format(24, 0);                  // Messege length (bits)
  digitalWrite(PI_0, HIGH);           // Drive the CS HIGH.

 
  // Attach the PWM pins to Interrupt
  attachInterrupt(digitalPinToInterrupt(droneTrig1), ISR1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(droneTrig2), ISR2, CHANGE);

  io.begin();
  //Initialize the desired Pin to desired mode (Input, or Output)
  io.remotepinMode(P0_0, OUTPUT); //Seting Pin P0_0 to an Output;
  io.remotepinMode(P0_1, OUTPUT);  //Sets Pin P0_1 to be an Output;
  io.remotepinMode(P0_2, OUTPUT);
  io.remotepinMode(P0_3, OUTPUT);
  io.remotepinMode(P0_4, OUTPUT);
  io.remotepinMode(P0_5, OUTPUT);
  io.remotepinMode(P0_6, OUTPUT);
  io.remotepinMode(P0_7, OUTPUT);
  io.remotepinMode(P1_0, OUTPUT);
  io.remotepinMode(P1_1, OUTPUT);
  io.remotepinMode(P1_2, OUTPUT);
  io.remotepinMode(P1_3, OUTPUT);
  io.remotepinMode(P1_4, OUTPUT);
  io.remotepinMode(P1_5, OUTPUT);
  io.remotepinMode(P1_6, OUTPUT);
  io.remotepinMode(P1_7, OUTPUT);
  io.remotepinMode(P2_0, OUTPUT);
  io.remotepinMode(P2_1, OUTPUT);

  sensor1.init(LTC4151::L, LTC4151::L);    // Slave Address 0x6F
  
  // Temperature sensor
  Wire.begin();
  sht1.init();
  sht2.init();
}

void loop()
{
  
  healthDataCheck();
  int imgCount = 0;
  HFlag = 1;
  if(HFlag == 1 && SFlag == 1)
  {   
     int i = 0;
     while (Serial3.available() > 0)    // Read the control mode coming from EC (0 : Trigger mode; 1 : PWM mode; 2 : User input(i, r, c) ) 
     {
        buffer1[i] = Serial3.read();
        i++;  
     }   
     
     char Temp = decode_char(buffer1, &RF_config, &SPI_config);   
  
     if(Temp == 's' || Temp == 'p' || Temp == 'u')
       if(idleTrigFlag == 1 && idlePWMFlag == 1 && idleUserFlag == 1)    // Change control mode only in IDLE state 
       {
         control_mode = Temp;
         controls_mode[0] = Temp;
       } 
     if(Temp == 'i' || Temp == 'r' || Temp == 'c')         // Change states in 'u' mode 
        input = Temp;
      
      switch(control_mode)  
      {
        case 's':  // SWITCH TRIGGER 
           //IDLE
           if(idleTrigFlag == 1)
           {
             idleTrigFlag = 1;
             idlePWMFlag = 1;
             idleUserFlag = 1;
             digitalWrite(LCCtrig, LOW);
             digitalWrite(EC1trig, LOW);
             digitalWrite(EC2trig, LOW);
             digitalWrite(RP1trig, LOW);
             digitalWrite(RP2trig, LOW);
             if(readyTrigFlag == 1)           
                digitalWrite(RP1trig, HIGH);     // Trigger to RP when state changes from READY to IDLE
             readyTrigFlag = 0;
             // No Triggers for closing any RF (5v, -5v) Triggers  
             RF(0);       
             if (rpHealthVal == 0)
              {
                int timeElapsedRP = millis();
                if(timeElapsedRP-lastTimeRP>RP_DELAY){
                delay(5000);
                digitalWrite(RP1trig, LOW);
                lastTimeRP = millis();}
              }
              imgCount = 0;
              strcpy(fsmState, "IDLE");   // "IDLE" is copied into the character array fsmState
           }
           if (trig1Read == 1)   //READY
           {
              digitalWrite(LCCtrig, HIGH);
              digitalWrite(EC1trig, HIGH);
              digitalWrite(EC2trig, LOW);
              digitalWrite(RP1trig, LOW);
              digitalWrite(RP2trig, LOW);
              imgCount = 0;
              imgCount = 0;
              idleTrigFlag = 0;
              RF(RF_config);
              readyTrigFlag = 1;
              strcpy(fsmState, "READY");    // "READY" is copied into the character array fsmState
           }

           if((readyTrigFlag == 1) && (trig2Read == 1) )   //IMAGING
           {
              if(imgCount == 0)
              {
                digitalWrite(LCCtrig, HIGH);
                digitalWrite(EC1trig, HIGH);
                digitalWrite(EC1trig, HIGH);
                digitalWrite(RP1trig, HIGH);
                digitalWrite(RP2trig, HIGH);
                delay(100);
                spiWriteSequence(SPI_config);
                startImaging = millis();
                imgCount = 1;
                strcpy(fsmState, "IMAGING");     // "IMAGING" is copied into the character array fsmState
              }
                else {
                  overflowImaging = millis();                
                  if ((overflowImaging - startImaging) >= 120000) // Checks if the system has been in imaging for longer than 100 seconds
                  {
                    idleTrigFlag = 1;   // Goto IDLE
                    readyPWMFlag = 0;
                    input = 'x';     // To stop the flow from entering READY and IMAGING
    //                Serial.println("State flag goes low");
                    SFlag = false; // Sets the safety flag to zero
                    break;
                  }
                  break;
             }
           }
           break;

        
      case 'p':   //PWM
         if(idlePWMFlag == 1)
         {
           // IDLE
           idleTrigFlag = 1;
           idlePWMFlag = 1;
           idleUserFlag = 1;
           digitalWrite(LCCtrig, LOW);
           digitalWrite(EC1trig, LOW);
           digitalWrite(EC2trig, LOW);
           digitalWrite(RP1trig, LOW);
           digitalWrite(RP2trig, LOW);
           if(readyPWMFlag == 1)
              digitalWrite(RP1trig, HIGH);        // Trigger to RP when state changes from READY to IDLE
          
           // No Triggers for closing any RF (5v, -5v) Triggers  
           RF(0);       
           if (rpHealthVal == 0)
            {
              int timeElapsedRP = millis();
              if(timeElapsedRP-lastTimeRP>RP_DELAY){
              delay(5000);
              digitalWrite(RP1trig, LOW);
              lastTimeRP = millis();}
            }
            imgCount = 0;
            strcpy(fsmState, "IDLE");      // "IDLE" is copied into the character array fsmState
         }
         if(pwm1Read > 1700)   //READY
         { 
       
            digitalWrite(LCCtrig, HIGH);
            digitalWrite(EC1trig, HIGH);
            digitalWrite(EC2trig, LOW);
            digitalWrite(RP1trig, LOW);
            digitalWrite(RP2trig, LOW);
            imgCount = 0;
            idlePWMFlag = 0;
            imgCount = 0;
            RF(RF_config);
            readyPWMFlag = 1;
            strcpy(fsmState, "READY");          // "READY" is copied into the character array fsmState
         } 

         if((readyPWMFlag == 1) && (pwm2Read > 1700))     // IMAGING
         {
           if(imgCount == 0)
           {
              digitalWrite(LCCtrig, HIGH);
              digitalWrite(EC1trig, HIGH);
              digitalWrite(EC1trig, HIGH);
              digitalWrite(RP1trig, HIGH);
              digitalWrite(RP2trig, HIGH);
              delay(100);
              spiWriteSequence(SPI_config);
              startImaging = millis();
              imgCount = 1;
              strcpy(fsmState, "IMAGING");      // "IMAGING" is copied into the character array fsmState
              Serial.println("IMAGING PWM");
           }
           else {
//              Serial.println("Imaging but without configuration");
                overflowImaging = millis();                
                if ((overflowImaging - startImaging) >= 120000) // Checks if the system has been in imaging for longer than 100 seconds
                {
                  idlePWMFlag = 1;   // Goto IDLE
                  readyPWMFlag = 0;
//                Serial.println("State flag goes low");
                  SFlag = false; // Sets the safety flag to zero
                  break;
                }
                break;
             }
         }
         break;

         case 'u':   //USER

           //IDLE
           if(idleUserFlag == 1 || input == 'i')
           {
             idleTrigFlag = 1;
             idlePWMFlag = 1;
             idleUserFlag = 1;
             digitalWrite(LCCtrig, LOW);
             digitalWrite(EC1trig, LOW);
             digitalWrite(EC2trig, LOW);
             digitalWrite(RP1trig, LOW);
             digitalWrite(RP2trig, LOW);
             if(cameraFlag == 1)
                digitalWrite(RP1trig, HIGH);
             cameraFlag = 0;
             RF(0);       
             cameraFlag = 0;
             if (rpHealthVal == 0)
              {
                int timeElapsedRP = millis();
                if(timeElapsedRP-lastTimeRP>RP_DELAY){
                delay(5000);
                digitalWrite(RP1trig, LOW);
                lastTimeRP = millis();}
              }
              imgCount = 0;
              strcpy(fsmState, "IDLE");     // "IDLE" is copied into the character array fsmState
           }
           
            
            if(input == 'r' || input == 'c')     // READY OR IMAGING
            {
                idleUserFlag = 0;
                digitalWrite(LCCtrig, HIGH);
                digitalWrite(EC1trig, HIGH);
                digitalWrite(EC2trig, LOW);
                digitalWrite(RP1trig, LOW);
                digitalWrite(RP2trig, LOW);
                imgCount = 0;
                RF(RF_config);
                cameraFlag = 1;
                strcpy(fsmState, "READY");     // "READY" is copied into the character array fsmState
             }
     
            if(cameraFlag == 1 && input == 'c')   //IMAGING
            {
              if(imgCount == 0)
              {
                digitalWrite(LCCtrig, HIGH);
                digitalWrite(EC1trig, HIGH);
                digitalWrite(EC1trig, HIGH);
                digitalWrite(RP1trig, HIGH);
                digitalWrite(RP2trig, HIGH);
                delay(100);
                spiWriteSequence(SPI_config);
                startImaging = millis();
                imgCount = 1;
                strcpy(fsmState, "IMAGING");     // "IMAGING" is copied into the character array fsmState
              }
              else {
//              Serial.println("Imaging but without configuration");
                overflowImaging = millis();                
                if ((overflowImaging - startImaging) >= 120000) // Checks if the system has been in imaging for longer than 100 seconds
                {
                  idleUserFlag = 1;   // Goto IDLE
                  cameraFlag = 0;
//                Serial.println("State flag goes low");
                  SFlag = false; // Sets the safety flag to zero
                  break;
                }
                break;
             }
            }
            break;
      }
    
  }

  else 
  {
       //IDLE
       idleTrigFlag = 1;
       idlePWMFlag = 1;
       idleUserFlag = 1;
       digitalWrite(LCCtrig, LOW);
       digitalWrite(EC1trig, LOW);
       digitalWrite(EC1trig, LOW);
       digitalWrite(RP1trig, LOW);
       digitalWrite(RP2trig, LOW);
       RF(0);       
       if (rpHealthVal == 0)
        {
          int timeElapsedRP = millis();
          if(timeElapsedRP-lastTimeRP>RP_DELAY){
          delay(5000);
          digitalWrite(RP1trig, LOW);
          lastTimeRP = millis();}
        }
        imgCount = 0;
        strcpy(fsmState, "IDLE");     // "IDLE" is copied into the character array fsmState
  }

}
