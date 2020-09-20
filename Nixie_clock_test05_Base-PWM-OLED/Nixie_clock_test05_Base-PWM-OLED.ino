#include <DS3231.h>
#include <SerialNixieDriver.h>
#include "U8glib.h"

SerialNixieDriver driver;
Time t;

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
/**
 * Please plug the following pin:
 *  Pin 10 to the RCK pin on the board
 *  Pin 13 to the SCK pin on the board
 *  Pin 11 to the DIN (data in) pin on the board
 *  Output Enable should be tied to Ground
 */
int dataPin = 8;
int oePin = 9;   //输出使能引脚，低电平有效
int rckPin = 10;
int sckPin = 11;

int button0Pin = 7;

//set time
int hour1;
int minute;
int second;
int hl,hr,ml,mr,sl,sr;

int buttonState;    //按键状态变量
int buttonLastState;    //按键最后一次状态检测
int buttonCounter;    //按键计数变量
int displayMode;   //显示模式变量

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
  pinMode(oePin,OUTPUT);
  // Initialize the rtc object
  rtc.begin();
  digitalWrite(oePin,LOW);
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(19, 30, 30);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(27, 8, 2020);   // Set the date to January 1st, 2014
  //Init(2020,9,20,17,18,50);
  //Button set
  pinMode(button0Pin,INPUT_PULLUP);
  //设置按键0为默认上拉模式

  
}


void loop(){

  //串口输出时间信息，波特率：11520，用于调试，正常运行请关闭此功能以得到最佳运行状态
  serialPrinting();  

  //显示模式选择
  buttonState = digitalRead(button0Pin);
  if(buttonState != buttonLastState)
  {
    if(buttonState)
    {
        buttonCounter++;
    }
    delay(100);
  }

  buttonLastState = buttonState;
  if(buttonCounter%2)
  {
    digitalWrite(oePin,LOW);
    showTime_base();
    displayMode = 0; 
    //Serial.println("In basic display");  
  }
  else
  {
    showTime_PWM();
    displayMode = 1;
    //Serial.println("In PWM display");      
  }

  //OLED显示部分
  u8g.firstPage();  
    do {
      OLED_Display();
    } while( u8g.nextPage() );

    
}

void Init(int y,int mmon,int d,int h,int mmin,int s)    //RTC初始化
{
  rtc.setTime(h,mmin,s);
  rtc.setDate(d,mmon,y);
  rtc.setDOW();
}

void OLED_Display()
{
  /*char data[10];
  char week[10];
  char t[8];
  
  data = rtc.getDateStr(FORMAT_LONG,FORMAT_BIGENDIAN,'.');
  week = rtc.getDOWStr(FORMAT_SHORT);
  t = rtc.getTimeStr(FORMAT_LONG);*/

  u8g.setFont(u8g_font_unifont);
  u8g.setFontPosTop();
  
  u8g.setPrintPos(0, 0); 
  u8g.print(rtc.getDateStr(FORMAT_LONG,FORMAT_BIGENDIAN,'.') );

  u8g.setPrintPos(100, 0); 
  u8g.print(rtc.getDOWStr(FORMAT_SHORT) );

  u8g.setPrintPos(35, 20); 
  u8g.print(rtc.getTimeStr(FORMAT_LONG) );

  switch(displayMode){
    case 0:{
      u8g.setPrintPos(27, 40);
      u8g.print("Basic Mode"); 
      break;
      }
    case 1:{
      u8g.setPrintPos(35, 40);
      u8g.print("PWM Mode");
      break;
      }
    } 
  
}


void serialPrinting() {
  Serial.println("Status feedback:");
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

  //打印当前显示模式
  switch(displayMode){
    case 0:{
      Serial.println("In basic display"); 
      break;
      }
    case 1:{
      Serial.println("In PWM display");
      break;
      }
    }
  Serial.println("");
  
  }

void randShow(){
    int nixieShow[] = {4,5,2,7,8,3,1,0,6,9,5,8,4,7,1,3,6,9,2,0};
    for (uint8_t n = 0; n < 19; n++) {
    driver.send(nixieShow[n]);
    delay(25);
    } 
}




void showTime_base() {
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

  uint8_t data[] = {hl,hr,ml,mr,sl,sr};
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


void showTime_PWM() {
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

  uint8_t data[] = {hl,hr,ml,mr,sl,sr};
  //每十秒随机变化一次
  if(sr == 0){
    randShow();  
  }

  for (int oevalue = 0;oevalue <= 250; oevalue = oevalue + 51) {
    analogWrite(oePin, oevalue);
    //delayMicroseconds(2000);
    delay(90);
  } 

  //randShow();
  //display time on a daisy chain of 6 tubes
  driver.send(data, NB_TUBES);
  delay(100);
  
  for (int oevalue = 250;oevalue >= 0; oevalue = oevalue - 51) {  
    analogWrite(oePin, oevalue);
    //delayMicroseconds(2000);
    delay(90);
  }
}
