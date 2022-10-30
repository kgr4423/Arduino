#include <Camera.h>

#define BAUDRATE  9600
#define BAUDRATE2 38400

void setup() {
  Serial.begin(BAUDRATE);
  Serial2.begin(BAUDRATE2);
  while(Serial2.available()>0){Serial2.read();}

  theCamera.begin();
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  theCamera.setStillPictureImageFormat(320,240,CAM_IMAGE_PIX_FMT_RGB565);
}

void loop() {
  // 送信データ格納用配列
  byte sendByte[1];
  
  // Unityからのデータを読み取る
  if(Serial2.available() > 0){
    char receivedByte = Serial2.read();
    Serial.print(receivedByte);
    Serial.print(' ');
  }

  // アクティビティデータの送信
  char temp = Serial.read();
  if(temp == 'a'){

    // アクティビティデータ送信の合図
    Serial.println("Send Activity Sign");
    sendByte[0] = 'a';
    for(int i=1; i<=20; ++i){
      Serial2.write(sendByte, 1);
    }

    delay(5000);

    // アクティビティデータ本体の送信
    String ActivityStr = "danger,34.8327034,138.5168499,";
    Serial.println("Send Activity Data");
    for(int i=0; i<ActivityStr.length(); ++i){
      sendByte[0] = ActivityStr.charAt(i);
      Serial2.write(sendByte, 1);
    }
  }

  // 写真の撮影と送信
  if(temp == 'p'){
    // 写真撮影
    Serial.println("Call takePicture()");
    CamImage img = theCamera.takePicture();

    // 画像データ送信の合図
    Serial.println("Send Picture sign");
    sendByte[0] = 'p';
    for(int i=1; i<=20; ++i){
      Serial2.write(sendByte, 1);
    }

    delay(5000);

    // 画像データ本体の送信
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

  }

}
