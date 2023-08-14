#include "DHT.h"
#define DHTPIN 32      // what digital pin we're connected to
#define DHTTYPE DHT11  // DHT 11
#include "RTClib.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SimpleDHT.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "FS.h"
#include "SPIFFS.h"
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>
#define FORMAT_SPIFFS_IF_FAILED true

Adafruit_ADS1115 ads;
RTC_DS1307 rtc;

#define WIFI_SSID "Hehe"
#define WIFI_PASSWORD "12345678"

#define FIREBASE_HOST "doan2-008-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "dz1QmzoFIwajwFk49iStV9wyonCD5FdwL8dplJRt"
FirebaseData fbdo;
int i;

int16_t adc0;
int16_t adc1;
float lux;
DHT dht(DHTPIN, DHTTYPE);

int _day, _month, _year, _hour24, _hour12, _minute, _second, _dtw;  //khai báo các biến cục bộ có kiểu dữ liệu tương ứng để lưu trữ thông tin thời gian và ngày trong tuần được đọc từ module RTC.
int hr24;

char nameoftheday[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };  //khai báo mảng hai chiều nameoftheday để lưu trữ tên của các ngày trong tuần.
String st;

char dataBuffer[1024];
int dataLength = 0;

int flashID = 0;
int IDin = 1;
//int IDout = 0;
int mucnuocout = 60;


int relayPin = 23;
//int muaanalog = 25;
int muadigital = 35;

const int dry = 13160;  // value for dry sensor
const int wet = 5100;   // value for wet sensor
//unsigned long previousTime = 0;

bool e = 0;
//bool enable = true;

int mode = 0;
const unsigned long delay4h = 4 * 60 * 60 * 1000;    // Độ trễ 4 giờ
const unsigned long delay8h = 8 * 60 * 60 * 1000;    // Độ trễ 8 giờ
const unsigned long delay12h = 12 * 60 * 60 * 1000;  // Độ trễ 12 giờ
float thoigian = 0;

FirebaseData stream;

unsigned long previous4h = 0;
unsigned long previous8h = 0;
unsigned long previous12h = 0;

float h = dht.readHumidity();
float t = dht.readTemperature();
float f = dht.readTemperature(true);

int digital = digitalRead(muadigital);

unsigned long now = 0;

int trangthairelay = digitalRead(relayPin);

int amdatpercent = map(adc0, wet, dry, 100, 0);

//test delay ngắn
//const unsigned long delay4h = 4000;
//const unsigned long delay8h = 8000;
//const unsigned long delay12h = 12000;

unsigned long startTime = 0;    // Thời gian bắt đầu đếm
unsigned long elapsedTime = 0;  // Thời gian đã trôi qua

void streamCallback(StreamData data) {
  Serial.printf("stream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  printResult(data);  // see addons/RTDBHelper.h
  String test = data.to<String>();
  IDin = test.toInt();
  Serial.println(IDin);

  // This is the size of stream payload received (current and max value)
  // Max payload size is the payload size under the stream path since the stream connected
  // and read once and will not update until stream reconnection takes place.
  // This max value will be zero as no payload received in case of ESP8266 which
  // BearSSL reserved Rx buffer size is less than the actual stream payload.
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", data.payloadLength(), data.maxPayloadLength());
}

void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup() {
  pinMode(relayPin, OUTPUT);
  //pinMode(muaanalog, INPUT);
  pinMode(muadigital, INPUT);
  //pinMode(datpin, INPUT);
  Serial.begin(9600);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.println("START");


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  Wire.begin();
  ads.begin();
  dht.begin();
  rtc.begin();
  //readFiles(SPIFFS, "/type.csv");
  // Serial.println(F("Data read from file:"));
  // for (int i = 0; i < dataLength; i++) {
  //   Serial.write(dataBuffer[i]);
  // }
  if (!Firebase.beginStream(stream, "/TT_IoT/changeID/ID"))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);
  // rtc.adjust(DateTime(2023, 7, 12, 14, 55, 0));
}
//kết thúc setup



void anhsang() {
  float volts = adc1 * 5.0 / 24684;
  float amps = volts / 10000.0;  // across 10,000 Ohms
  float microamps = amps * 1000000;
  lux = microamps * 2.0;
  Serial.println(lux);
  delay(750);
  //Serial.println();
}

void docthongso() {
  if (Firebase.setFloat(fbdo, "/TT_IoT/nhietdo", dht.readTemperature()) && Firebase.setFloat(fbdo, "/TT_IoT/doam", dht.readHumidity()))
    Serial.println("Upload humid and temp success");
  else
    Serial.println("Upload humid and temp fail");

  if (Firebase.setInt(fbdo, "/TT_IoT/cambienmua", digital))
    Serial.println("Upload mua success");
  else
    Serial.println("Upload mua fail");

  if (Firebase.setInt(fbdo, "/TT_IoT/doamdat", amdatpercent))
    Serial.println("Upload do am dat success");
  else
    Serial.println("Upload do am dat fail");

  if (Firebase.setInt(fbdo, "/TT_IoT/trangthairelay", trangthairelay))
    Serial.println("Upload relay success");
  else
    Serial.println("Upload relay fail");

  if (Firebase.setInt(fbdo, "/TT_IoT/anhsang", lux))
    Serial.println("Upload anh sang success");
  else
    Serial.println("Upload anh sang fail");
}

void GetDateTime() {         //Hàm lấy thông tin thời gian và ngày trong tuần từ module RTC và lưu vào các biến _day, _month, _year, _hour24, _hour12, _minute, _second và _dtw.
  DateTime now = rtc.now();  //sử dụng hàm now() để lấy thời gian hiện tại từ module RTC và lưu vào đối tượng now kiểu DateTime
  _day = now.day();
  _month = now.month();
  _year = now.year();
  _hour24 = now.hour();
  _minute = now.minute();
  _second = now.second();
  _dtw = now.dayOfTheWeek();

  hr24 = _hour24;  //sử dụng biến hr24 để lưu giá trị của biến _hour24 để có thể sử dụng sau đó.  kiểm tra giá trị của biến _hour24 để tính giá trị của biến _hour12 dạng 12 giờ.
  if (hr24 > 12) {
    _hour12 = hr24 - 12;
  } else if (hr24 == 0) {
    _hour12 = 12;
  } else {
    _hour12 = hr24;
  }

  if (hr24 < 12) {  //sử dụng biến hr24 để kiểm tra xem thời gian hiện tại là buổi sáng (AM) hay buổi chiều (PM) và lưu vào biến st.
    st = "AM";
  } else {
    st = "PM";
  }
}

void thoigianthuc() {
  GetDateTime();  //gọi hàm GetDateTime() để lấy thông tin thời gian và ngày trong tuần từ module RTC và lưu vào các biến _day, _month, _year, _hour24, _hour12, _minute, _second và _dtw.
  Serial.print(nameoftheday[_dtw]);
  Serial.print(',');
  Serial.print(_day, DEC);
  Serial.print('/');
  Serial.print(_month, DEC);
  Serial.print('/');
  Serial.print(_year, DEC);
  Serial.print(" ");
  // Serial.print(_hour24, DEC);
  // Serial.print(':');
  // Serial.print(_minute, DEC);
  // Serial.print(':');
  // Serial.print(_second, DEC);

  Serial.print(' ');

  Serial.print(_hour12, DEC);
  Serial.print(':');
  Serial.print(_minute, DEC);
  Serial.print(':');
  Serial.print(_second, DEC);
  Serial.print(' ');
  Serial.print(st);

  Serial.println();
  delay(1000);
}

void tuoinuoc(int percent) {
  trangthairelay = digitalRead(relayPin);
  if (amdatpercent <= percent) {
    digitalWrite(relayPin, HIGH);
    Serial.println("đang tưới cây");
    Serial.println(percent);
    Serial.println(mucnuocout);
  } else {
    digitalWrite(relayPin, LOW);
    Serial.println("máy bơm đang tắt");
  }
}
void ngungtuoinuoc() {


  //int muaa = analogRead(muaanalog);
  int muad = digitalRead(muadigital);
  float t = dht.readTemperature();

  if (muad == 0) {
    if (startTime == 0) {    // Kiểm tra nếu chưa bắt đầu đếm
      startTime = millis();  // Gán thời gian bắt đầu đếm
      Serial.println("cảm biến phát hiện nước");
    }
  } else if (muad == 1) {
    if (startTime > 0) {                   // Kiểm tra nếu đã bắt đầu đếm
      elapsedTime = millis() - startTime;  // Tính thời gian đã trôi qua
      startTime = 0;                       // Reset thời gian bắt đầu đếm
      thoigian = (double)elapsedTime / 60000;
      //thoigian = (float)elapsedTime;
      now = startTime;
      elapsedTime = startTime;
      // In ra thời gian đã trôi qua
      Serial.print("Cảm biến bị ướt trong: ");
      Serial.print(thoigian);
      Serial.println(" phút");
    }
  }
}

void cambienmua() {
  //int analog = analogRead(muaanalog);
  digital = digitalRead(muadigital);
  delay(1000);
  //Serial.print("Analog value: ");
  //Serial.print(analog);
  //Serial.print(" | ");
  Serial.print("Digital value: ");
  Serial.println(digital);
}
void cambiendht() {
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%t |");
  Serial.print(" Temperature: ");
  Serial.print(t);
  Serial.print("*C = ");
  Serial.print(f);
  Serial.print("*Ft |");
  Serial.print(" Heat index: ");
  Serial.print(hic);
  Serial.print("*C = ");
  Serial.print(hif);
  Serial.println("*F");
}

void chanadc() {

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
}

void doamdat() {

  amdatpercent = map(adc0, wet, dry, 100, 0);
  Serial.print("moisture: ");
  Serial.print(amdatpercent);
  Serial.println("%");
  //Serial.print(sensorVal);
  delay(1000);
}

void cacmode() {
  if (thoigian != 0 && thoigian < 15) {
    //tưới bình thường
    Serial.println("mưa dưới 15p hệ thống chạy bình thường");
    thoigian = 0;
    mode = 0;
    //enable = true;
  } else if (thoigian >= 15 && thoigian < 30) {
    //ngưng tưới 4h
    if (now == 0) {
      Serial.println("hệ thống không cho phép tưới nước trong vòng 4h");

      now = millis();

      thoigian = 0;
      mode = 1;
    }
    //delay_millis(delay4h);
  } else if (thoigian >= 30 && thoigian < 60) {
    if (now == 0) {
      Serial.println("hệ thống không cho phép tưới nước trong vòng 8h");

      now = millis();

      thoigian = 0;
      mode = 2;
    }
  } else if (thoigian >= 60) {
    if (now == 0) {
      Serial.println("hệ thống không cho phép tưới nước trong vòng 12h");

      now = millis();

      thoigian = 0;
      mode = 3;
    }
  }
}

void readFiles(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Đọc dữ liệu từ file và lưu vào mảng dataBuffer
  int i = 0;
  while (file.available()) {
    dataBuffer[i] = file.read();
    i++;
  }
  dataLength = i;

  file.close();
}

void sosanh(const char *data, int length, int targetID) {
  // Bỏ qua dòng tiêu đề
  int startIndex = 0;
  while (data[startIndex] != '\n') {
    startIndex++;
  }
  startIndex++;  // Đặt startIndex tại ký tự đầu tiên của dòng dữ liệu thứ nhất
  int endIndex = startIndex;
  int currentID = 0;
  int waterheight = 0;
  while (endIndex < length) {
    // Đọc ID
    while (data[endIndex] != ',') {
      currentID = currentID * 10 + (data[endIndex] - '0');
      endIndex++;
    }
    endIndex++;  // Bỏ qua dấu phẩy

    // Đọc NAME
    int nameIndex = 0;
    while (data[endIndex] != '\n') {
      waterheight = waterheight * 10 + (data[endIndex] - '0');
      endIndex++;
      nameIndex++;
    }
    endIndex++;  // Bỏ qua ký tự xuống dòng

    // So sánh ID với targetID
    if (currentID == targetID) {
      //waterheight[nameIndex] = '\0';  // Kết thúc chuỗi NAME
      Serial.print(F("ID: "));
      Serial.print(currentID);

      //IDout = currentID;
      Serial.print(F(", muc nuoc: "));
      Serial.println(waterheight);
      mucnuocout = waterheight;
      break;
    }
    // Reset ID và NAME
    currentID = 0;
    //memset(waterheight, 0, sizeof(waterheight));
    waterheight = 0;
  }
}

void loop() {

  switch (mode) {
    case 0:
      Serial.println("đang chạy mode 0");
      thoigianthuc();
      chanadc();
      while (IDin != flashID) {
        flashID = IDin;
        readFiles(SPIFFS, "/type.csv");
        sosanh(dataBuffer, dataLength, IDin);

        Serial.println(F("Data read from file:"));
        for (int i = 0; i < dataLength; i++) {
          Serial.write(dataBuffer[i]);
        }
      }
      anhsang();
      doamdat();
      cambienmua();
      tuoinuoc(mucnuocout);
      ngungtuoinuoc();
      docthongso();
      cacmode();
      break;
    case 1:  //4h
      Serial.println("đang chạy mode 1");
      digitalWrite(relayPin, LOW);
      thoigianthuc();
      chanadc();
      while (IDin != flashID) {
        flashID = IDin;
        readFiles(SPIFFS, "/type.csv");
        sosanh(dataBuffer, dataLength, IDin);

        Serial.println(F("Data read from file:"));
        for (int i = 0; i < dataLength; i++) {
          Serial.write(dataBuffer[i]);
        }
      }
      anhsang();
      cambienmua();
      Serial.println(now);
      //Serial.println(previous4h);
      Serial.println(millis() - now);

      if (millis() - now >= delay4h) {

        Serial.println("chuyển về mode 0");
        now = 0;
        mode = 0;
      }

      break;
    case 2:  //8h
      Serial.println("đang chạy mode 2");
      digitalWrite(relayPin, LOW);
      thoigianthuc();
      chanadc();
      anhsang();
      cambienmua();
      while (IDin != flashID) {
        flashID = IDin;
        readFiles(SPIFFS, "/type.csv");
        sosanh(dataBuffer, dataLength, IDin);

        Serial.println(F("Data read from file:"));
        for (int i = 0; i < dataLength; i++) {
          Serial.write(dataBuffer[i]);
        }
      }
      Serial.println(now);
      //Serial.println(previous4h);
      Serial.println(millis() - now);

      if (millis() - now >= delay8h) {

        Serial.println("chuyển về mode 0");
        now = 0;
        mode = 0;
      }
      break;
    case 3:  //12h
      Serial.println("đang chạy mode 3");
      digitalWrite(relayPin, LOW);
      thoigianthuc();
      chanadc();
      anhsang();
      cambienmua();
      if (IDin != flashID) {
        flashID = IDin;
        readFiles(SPIFFS, "/type.csv");
        sosanh(dataBuffer, dataLength, IDin);

        Serial.println(F("Data read from file:"));
        for (int i = 0; i < dataLength; i++) {
          Serial.write(dataBuffer[i]);
        }
      }
      Serial.println(now);
      //Serial.println(previous4h);
      Serial.println(millis() - now);

      if (millis() - now >= delay12h) {

        Serial.println("chuyển về mode 0");
        now = 0;
        mode = 0;
      }
      break;
  }
}