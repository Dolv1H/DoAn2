
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

int i;
int16_t adc0; //Chân A0
int16_t adc1; //Chân A1
float lux;
const int dry = 13160;  // value for dry sensor
const int wet = 5100;   // value for wet sensor
int amdatpercent = map(adc0, wet, dry, 100, 0);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  ads.begin();
  Serial.println("START");
}

void chanadc() {
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
}

void doamdat() {  //Cảm biến độ ẩm đất dùng chân A0 của ADS1115
  amdatpercent = map(adc0, wet, dry, 100, 0);
  Serial.print("moisture: ");
  Serial.print(amdatpercent);
  Serial.println("%");
  delay(1000);
}

void anhsang() {  //Cảm biến ánh sáng chân A1 của ADS1115
  float volts = adc1 * 5.0 / 24684;
  float amps = volts / 10000.0;
  float microamps = amps * 1000000;
  lux = microamps * 2.0;
  Serial.println(lux);
  delay(750);
}

void loop() {
  chanadc();
  anhsang();
  doamdat();
}