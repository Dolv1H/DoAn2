void setup() 
{
  Serial.begin(9600);
}

void loop()
{
  delay(1000);
  Serial.print("Analog value: ");
  Serial.print(analogRead(25));
  Serial.print(" | ");
  Serial.print("Digital value: ");
  Serial.println(digitalRead(12));
}
