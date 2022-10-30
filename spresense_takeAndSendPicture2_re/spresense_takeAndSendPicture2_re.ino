#include <Camera.h>

//about gps
//#include <stdio.h>
//#include <String.h>
//#include <GNSS.h>
//#include <GNSSPositionData.h>
//
////#include <iostream>
////#include <String>       // ヘッダファイルインクルード
//using namespace std;         //  名前空間指定

//#include "gnss_tracker.h"
//#include "gnss_nmea.h"
//#include "gnss_file.h"

///* Default parameter. */
//#define DEFAULT_INTERVAL_SEC    1          /**< Default positioning interval in seconds*/
//#define DEFAULT_ACTIVE_SEC      60         /**< Default positioning active in seconds */
//#define DEFAULT_SEEP_SEC        240        /**< Default positioning sleep in seconds */
//#define INITIAL_ACTIVE_TIME     300        /**< Initial positioning active in seconds */
//#define IDLE_ACTIVE_TIME        600        /**< Idle positioning active in seconds */
//
//#define SEPARATOR           0x0A           /**< Separator */
//
//enum LoopState {
//  eStateSleep,  /**< Loop is not activated */
//  eStateActive  /**< Loop is activated */
//};
//
//enum TrackerMode {
//  eModeNormal = 0,  /**< Run positioning. Output NMEA text. */
//  eModeShell        /**< Run nsh to print and delete NMEA text. */
//};
//
//typedef struct
//{
//  ParamSat      SatelliteSystem;  /**< Satellite system(GPS/GLONASS/ALL). */
//  boolean       NmeaOutUart;      /**< Output NMEA message to UART(TRUE/FALSE). */
//  boolean       NmeaOutFile;      /**< Output NMEA message to file(TRUE/FALSE). */
//  boolean       BinaryOut;        /**< Output binary data to file(TRUE/FALSE). */
//  unsigned long IntervalSec;      /**< Positioning interval sec(1-300). */
//  unsigned long ActiveSec;        /**< Positioning active sec(60-300). */
//  unsigned long SleepSec;         /**< Positioning sleep sec(0-240). */
//  SpPrintLevel  UartDebugMessage; /**< Uart debug message(NONE/ERROR/WARNING/INFO). */
//} ConfigParam;
//
//SpGnss Gnss;                            /**< SpGnss object */
//ConfigParam Parameter;                  /**< Configuration parameters */
//unsigned int Mode;                      /**< Tracker mode */
//char FilenameTxt[OUTPUT_FILENAME_LEN];  /**< Output file name */
//char FilenameBin[OUTPUT_FILENAME_LEN];  /**< Output binary file name */
//AppPrintLevel AppDebugPrintLevel;       /**< Print level */
//unsigned long SetActiveSec;             /**< Positioning active sec(60-300) */


//////gps setting end

#define BAUDRATE  9600
#define BAUDRATE2 38400

bool isOpened;
bool isClosed;
bool isSafe;
int safeCount = 0;

///define pin
//led1
#define blue1 5
#define green1 6
#define red1 7
//led2
#define blue2 8
#define green2 9
#define red2 10
//force sensing
#define forcePin A0

//define about send
const String gpsSend = "";
const String picSend = "";
//const String 
String strIdo="34.8327034",strKeido="138.5168499",strActivity ="";

//define about receive
const String weatherRecv = "";

const String sunnyDay = "";
const String cloudyDay = "";
const String rainyDay = "";

const String sunnyHour = "";
const String cloudyHour = "";
const String rainyHour = "";

const char voiceOrder = 'd';
const String correctPass = "open";
const char reconnectUnity = 'c';

byte sendByte[1];
CamImage img;

const String gpsOrder = "";

char orderRecv;
String voiceRecv="",weatherDay = "", weatherHour = "";

///threashold
int openThresh = 3000;
int closeThresh = 1000;
int openCnt = 1000000;






void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial){;}
  Serial2.begin(BAUDRATE2);
  while (!Serial2){;}
  
  //camera setting
  theCamera.begin();
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  theCamera.setStillPictureImageFormat(320,240,CAM_IMAGE_PIX_FMT_RGB565);
  
  //pin initialize  
  pinMode(blue1,OUTPUT);
  pinMode(green1,OUTPUT);
  pinMode(red1,OUTPUT);
  pinMode(blue2,OUTPUT);
  pinMode(green2,OUTPUT);
  pinMode(red2,OUTPUT);
  allOff();

  isOpened = false;
  isSafe = false;
  isClosed = false;

  
//  //gps setting
//  int error_flag = 0;
//  char KeyRead[2] = {0, 0};
//
//  /* Wait mode select. */
//  sleep(3);
//
//  /* Read key input. */
//  KeyRead[0] = Serial.read();
//
//  /* Convet to mode value. */
//  Mode = strtoul(KeyRead, NULL, 10);
//
//  switch (Mode) {
//#if 0 // TBD:Not implemented
//    case eModeShell:
//      /* nop */
//      break;
//#endif
//
//    case eModeNormal:
//    default:
//      error_flag = SetupPositioning();
//      break;f
//  }
//
//  SetActiveSec = INITIAL_ACTIVE_TIME;

}






void loop() {

  // 一定時間したらsafe状態が終わる
  if(safeCount < openCnt){
    safeCount += 1;
  }else{
    safeCount = 0;
    isSafe = false;
    ledOff(green1);
  }

  // Unityからのデータを受け取りそれに応じた処理を行う
  unityOrder();
  
  // 傘の開閉を識別
  //recognizeOpen();

  // デバッグ用
  if(Serial.available() > 0){
    char readByte = Serial.read();
    if(readByte == 'p'){
      isOpened = true;
    }
  }

  // gpsGetting();
  
  // 傘が開かれたらUnityにデータを送る
  if (isOpened){
    if (isSafe){
      sendActivity(); 
    }else{
      Serial.println("Call takePicture()");
      img = theCamera.takePicture();
      sendActivity();
      delay(5000);
      sendPicture();
    }
    isOpened = false;
  }

}






// 関数群

// Unityからの指令に応じて処理を行う
void unityOrder(){
  isSafe = true;
  if(Serial2.available()){
    orderRecv = Serial2.read();
    if(orderRecv == voiceOrder){
      Serial.println("disconnect from unity");
      recognizeVoice();
    }
//    else if(orderRecv == gpsOrder){
//      Serial.println("gps mode");
//      sendActivity();
//    }else if(orderRecv == weatherRecv){
//      Serial.println("weather mode");
//      weatherInfo();
//    }
  }
}

// 音声認識アプリからきたデータの処理
void recognizeVoice(){
  
  ledOn(blue1);
  delay(500);
  ledOff(blue1);

  while(true){
    if(Serial2.available()){
      voiceRecv = Serial2.readString();
      
      //succeed  /from arduino voice control
      if(voiceRecv == correctPass){
        Serial.println("canbrella is safe\nplease reconnect to unity");
        isSafe = true;
        ledOn(green1);
        delay(2000);
        ledOff(green1);
        safeCount = 0;    
        break;
      //failed  /from arduino voice control
      }else{
        Serial.println("Incorrect password");
        ledOn(red1);
        delay(2000);
        ledOff(red1);
      }
    }
  }
}

// 傘が開かれたかどうか識別しisOpenedにbool値をいれる
void recognizeOpen(){
  int val = analogRead(forcePin);
  if(val > openThresh){
    isOpened = true;
    allOff();
  }else if(val < closeThresh){
    isClosed = true;
  }

}

// GPSの値を取得する
void gpsGetting(){
//  static int State = eStateActive;
//  static int TimeOut = IDLE_ACTIVE_TIME;
//  static bool PosFixflag = false;
//  static char *pNmeaBuff     = NULL;
//  static char *pBinaryBuffer = NULL;
}

// アクティビティデータ送信
void sendActivity(){
  // アクティビティデータ送信の合図
  Serial.println("Send Activity Sign");
  sendByte[0] = 'a';
  for(int i=1; i<=20; ++i){
    Serial2.write(sendByte, 1);
  }

  delay(5000);

  // アクティビティデータ本体の送信
  if(isSafe)strActivity = "safe,";
  else strActivity = "danger,";
  strActivity += strIdo;
  strActivity += ",";
  strActivity += strKeido;
  strActivity += ",";
  Serial.println("Send Activity Data");
  for(int i = 0;i < strActivity.length();i++){
    sendByte[0] = strActivity.charAt(i);
    Serial2.write(sendByte,1);
  }
}

// 写真送信
void sendPicture(){
  if (img.isAvailable()){
    // 写真送信の合図
    Serial.println("Start sending Image");
    sendByte[0] = 'p';
    for(int i=1; i<=20; ++i){
      Serial2.write(sendByte, 1);
    }

    delay(5000);

    img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    char *buf = img.getImgBuff();
    for (int i = 0; i < img.getImgSize(); ++i, ++buf) {
      sendByte[0] = *buf;
      Serial2.write(sendByte, 1);
      if(i%640 == 0){
        while(Serial2.availableForWrite()<=10){;}
        Serial.print("Send byte : ");
        Serial.println(i);
        delay(1); //早くデータを送り過ぎると接続切れるので注意
      }
    }

    Serial.println("End sending Image");
  }
}

// 天気情報の処理
void weatherInfo(){
  int weathercnt = 0;
  while(true){
    if(Serial2.available()){
      if(weathercnt == 0){
        weatherDay = Serial2.readString();
        weathercnt++;
      }else if(weathercnt == 1){
        weatherHour = Serial2.readString();
        break;
      }
    }
  }
  
  if(isOpened)return;

  allOff();
  if(weatherDay == sunnyDay){
    ledOn(red1);
  }else if(weatherDay == cloudyDay){
    ledOn(green1);
  }else{
    ledOn(blue1);
  }

  if(weatherHour == sunnyHour){
    ledOn(red2);
  }else if(weatherHour == cloudyHour){
    ledOn(green2);
  }else{
    ledOn(blue2);
  }

}

// GPSデータの処理
static int SetupPositioning(void)
{
//  int error_flag = 0;
//
//  /* Set default Parameter. */
//  Parameter.SatelliteSystem  = eSatGpsGlonassQz1c;
//  Parameter.NmeaOutUart      = true;
//  Parameter.NmeaOutFile      = true;
//  Parameter.BinaryOut        = false;
//  Parameter.IntervalSec      = DEFAULT_INTERVAL_SEC;
//  Parameter.ActiveSec        = DEFAULT_ACTIVE_SEC;
//  Parameter.SleepSec         = DEFAULT_SEEP_SEC;
//  Parameter.UartDebugMessage = PrintNone;
//
//  /* Mount SD card. */
//  if (BeginSDCard() != true)error_flag = 1;
//  else SetupParameter();
//
//  /* Set Gnss debug mode. */
//  Gnss.setDebugMode(Parameter.UartDebugMessage);
//  AppDebugPrintLevel = (AppPrintLevel)Parameter.UartDebugMessage;
//
//  if (Gnss.begin(Serial) != 0)error_falg = 1;
//  else
//  {
//
//    Gnss.select(GPS);
//    Gnss.select(GLONASS);
//    Gnss.select(QZ_L1CA);
//
//    Gnss.setInterval(Parameter.IntervalSec);
//
//    if (Gnss.start(HOT_START) != OK)error_flag = 1;
//
//  }
//
//  /* Create output file name. */
//  FilenameTxt[0] = 0;
//  FilenameBin[0] = 0;
//  if ( (Parameter.NmeaOutFile == true) || (Parameter.BinaryOut == true) )
//  {
//    int FileCount = GetFileNumber();
//
//    if (Parameter.NmeaOutFile == true)
//    {
//      /* Create a file name to store NMEA data. */
//      snprintf(FilenameTxt, sizeof(FilenameTxt), "%08d.txt", FileCount);
//    }
//    if (Parameter.BinaryOut == true)
//    {
//      /* Create a file name to store binary data. */
//      snprintf(FilenameBin, sizeof(FilenameBin), "%08d.bin", FileCount);
//    }
//  }
//
//  return error_flag;
}

// 全てのLEDをOFFにする
void allOff(){
  digitalWrite(blue1,LOW);
  digitalWrite(green1,LOW);
  digitalWrite(red1,LOW);
  digitalWrite(blue2,LOW);
  digitalWrite(green2,LOW);
  digitalWrite(red2,LOW);
}
