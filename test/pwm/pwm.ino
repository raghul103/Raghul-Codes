
// the number of the LED pin
const int ledPin = 26;  // 16 corresponds to GPIO16
const int ledPin2 = 14;
const int ledChannel2 = 0;

const int pin1 = 32;
const int pin2 = 33;
const int pin3 = 34;
const int pin4 = 35;

// setting PWM properties
const int freq = 500;
const int ledChannel1 = 0;
const int resolution = 8;
const int dutyCycle1 = 100;
const int dutyCycle2= 250;
void setup(){
  // configure LED PWM functionalitites
  Serial.begin(115200);  
  ledcSetup(ledChannel1, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel1);

}

 
void loop(){

    ledcWrite(ledChannel1, dutyCycle2);
   
}
