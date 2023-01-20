
int pin = 6;
void setup()
{
  Serial.begin(115200);
 pinMode(pin, INPUT);
}

void loop()
{
  Serial.println(digitalRead(pin));
  delay(500);
}
