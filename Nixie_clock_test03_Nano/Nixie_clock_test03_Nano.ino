#include <DS3231.h>
#include <SerialNixieDriver.h>

SerialNixieDriver driver;
Time t;

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
/**
 * Please plug the following pin:
 *  Pin 10 to the RCK pin on the board
 *  Pin 13 to the SCK pin on the board
 *  Pin 11 to the DIN (data in) pin on the board
 *  Output Enable should be tied to Ground
 */
/* 
int rckPin = 10;
int sckPin = 13;
int dataPin = 11;
*/
//对应Arduino Nano 的D10、D11、D12引脚
#define rckPin 10
#define sckPin 11
#define dataPin 12


//set time
int hour1;
int minute;
int second;
int hl,hr,ml,mr,sl,sr;


//const int time_on = 2;
// The nixie tube driver declaration


// Array used to hold our numbers that will be displayed on the tube


// The nunber of tubes used in the daisy chain
const uint8_t NB_TUBES = 6;

void setup(){
  // Setup Serial connection
  Serial.begin(115200);
  // Setup as software serial, no output enable pin
  driver.begin(rckPin, sckPin, dataPin);
  // Initialize the rtc object
  rtc.begin();
  
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(7, 13, 25);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(14, 9, 2020);   // Set the date to January 1st, 2014
}


void loop(){
  showTime();
  //串口输出时间信息，波特率：11520，用于调试，正常运行请关闭此功能以得到最佳运行状态
  serialPrinting();  
}


void serialPrinting() {
    // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  
  // Wait one second before repeating :)
  //delay (1000);
  }

void randShow(){
    int nixieShow[] = {4,5,2,7,8,3,1,0,6,9,5,8,4,7,1,3,6,9,2,0};
    for (uint8_t n = 0; n < 19; n++) {
    driver.send(nixieShow[n]);
    delay(25);
    } 
}




void showTime() {
    //-------------------get clock value---------------------------
    // Get data from the DS3231
    t = rtc.getTime();
    hour1 = t.hour;
    minute = t.min;
    second = t.sec;

//    int j; //second number from right
//    int k; //first number from right

    hl = hour1/10;
    hr = hour1%10;
    ml = minute/10;
    mr = minute%10;
    sl = second/10;
    sr = second%10;

  //uint8_t data[] = {hl,hr,ml,mr,sl,sr};
  uint8_t data[] = {sl,sr,ml,mr,hl,hr};
  //每十秒随机变化一次
/*  if(sr == 0){
    randShow();  
  }
*/  
  randShow();
  //display time on a daisy chain of 6 tubes
  driver.send(data, NB_TUBES);
  delay(500);
  
}
