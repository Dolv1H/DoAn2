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

#define FORMAT_SPIFFS_IF_FAILED true
char dataBuffer[1024];  // Khai báo một mảng để lưu trữ dữ liệu từ file

int dataLength = 0;
int IDin = 3;
int IDout = 0;
int mucnuocout = 0;
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
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
void setup() {
  listDir(SPIFFS, "/", 0);
  Serial.begin(9600);
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.println("Start");
  writeFile(SPIFFS, "/type.csv", "ID,Muc Nuoc\n"); //thay đổi tên và đuôi tệp theo ý muốn vào dấu ngoặc kép đầu tiên, thay đổi tên hàng cột vào dấu ngoặc kép thứ 2
  Serial.println("Dang ghi");
  appendFile(SPIFFS, "/type.csv", "1,50\n2,60\n3,70\n"); //thay đổi tên và đuôi tệp theo ý muốn vào dấu ngoặc kép đầu tiên, thay đổi nội dung cần lưu vào dấu ngoặc kép phía sau, dấu "," là qua cột mới "\n" là xuống dòng
  delay(3000);

  readFiles(SPIFFS, "/type.csv");
  Serial.println(F("Data read from file:"));
  for (int i = 0; i < dataLength; i++) {
    Serial.write(dataBuffer[i]);
  }
  delay(5000);
  sosanh(dataBuffer, dataLength, IDin);
  Serial.print("ID: "); Serial.println(IDout);
  Serial.print("Muc nuoc: "); Serial.println(mucnuocout);
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

      IDout = currentID;
      Serial.print(F(", NAME: "));
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
}