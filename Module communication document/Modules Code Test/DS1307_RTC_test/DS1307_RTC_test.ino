#include "RTClib.h"

RTC_DS1307 rtc; //khởi tạo đối tượng RTC_DS1307 có tên rtc.

int _day, _month, _year, _hour24, _hour12, _minute, _second, _dtw; //khai báo các biến cục bộ có kiểu dữ liệu tương ứng để lưu trữ thông tin thời gian và ngày trong tuần được đọc từ module RTC.
int hr24;

String st;

char nameoftheday[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};  //khai báo mảng hai chiều nameoftheday để lưu trữ tên của các ngày trong tuần.

void setup() {
  Serial.begin(9600); //khởi tạo kết nối Serial với tốc độ truyền 9600 baud
  Serial.println("Arduino RTC DS1307"); //in ra màn hình dòng chữ "Arduino RTC DS1307"
  delay(1000);
  
  if (! rtc.begin()) {  //kiểm tra xem module RTC có được phát hiện hay không.
    Serial.println("Couldn't find RTC");  //Nếu module không được phát hiện, dòng chữ "Couldn't find RTC" sẽ được in ra màn hình và chương trình sẽ bị treo vô hạn trong vòng lặp while.
    while (1);
  }
  if (! rtc.isrunning()) {  //module RTC đang hoạt động hay không. Nếu module không đang hoạt động, dòng chữ "RTC is NOT running!" sẽ được in ra màn hình.
    Serial.println("RTC is NOT running!");
    
  }
  rtc.adjust(DateTime(2023, 4, 1, 10, 27, 0));
}
void loop() {
  GetDateTime(); //gọi hàm GetDateTime() để lấy thông tin thời gian và ngày trong tuần từ module RTC và lưu vào các biến _day, _month, _year, _hour24, _hour12, _minute, _second và _dtw.
  Serial.print(nameoftheday[_dtw]);
  Serial.print(',');    
  Serial.print(_day, DEC);
  Serial.print('/');
  Serial.print(_month, DEC);
  Serial.print('/');
  Serial.print(_year, DEC);
  Serial.print(" ");   
  Serial.print(_hour24, DEC);
  Serial.print(':');
  Serial.print(_minute, DEC);
  Serial.print(':');
  Serial.print(_second, DEC);

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

void GetDateTime() {  //Hàm lấy thông tin thời gian và ngày trong tuần từ module RTC và lưu vào các biến _day, _month, _year, _hour24, _hour12, _minute, _second và _dtw.
  DateTime now = rtc.now(); //sử dụng hàm now() để lấy thời gian hiện tại từ module RTC và lưu vào đối tượng now kiểu DateTime
  _day=now.day();
  _month=now.month();
  _year=now.year();
  _hour24=now.hour();
  _minute=now.minute();
  _second=now.second();
  _dtw=now.dayOfTheWeek();

  hr24=_hour24; //sử dụng biến hr24 để lưu giá trị của biến _hour24 để có thể sử dụng sau đó.  kiểm tra giá trị của biến _hour24 để tính giá trị của biến _hour12 dạng 12 giờ.
  if (hr24>12) {
    _hour12=hr24-12;
  }
  else if (hr24==0) {
    _hour12=12;
  }
  else {
    _hour12=hr24;
  }

  if (hr24<12) {  //sử dụng biến hr24 để kiểm tra xem thời gian hiện tại là buổi sáng (AM) hay buổi chiều (PM) và lưu vào biến st.
    st="AM";
  }
  else {
    st="PM"; 
  }  
}
