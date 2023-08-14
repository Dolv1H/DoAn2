void setup()
{
Serial.begin(9600); 
}
void loop()
{
int val;
val=analogRead(26); 
delay(500);
Serial.print("Analog value: "); 
Serial.println(val); 
delay(100);
}