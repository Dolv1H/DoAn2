const int dry = 2960;  // value for dry sensor
const int wet = 1100;  // value for wet sensor
void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorVal = analogRead(26);
  int percentageHumididy = map(sensorVal, wet, dry, 100, 0);
  Serial.print(percentageHumididy);
  Serial.println("%");
  delay(100);
}