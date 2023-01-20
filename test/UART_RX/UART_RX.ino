void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  
  //Serial2.begin(115200, SERIAL_8N1, 16, 17);
  //pinMode(D14, OUTPUT);
}

void loop()
{
  Serial.println(Serial2.read());
 
}
