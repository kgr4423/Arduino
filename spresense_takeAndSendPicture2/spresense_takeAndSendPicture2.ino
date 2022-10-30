#include <Camera.h>

#define BAUDRATE  9600
#define BAUDRATE2 38400

bool isOpened = false;
bool isSafe = false;
int safeCount = 0;

void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial){;}
  Serial2.begin(BAUDRATE2);
  while (!Serial2){;}

  theCamera.begin();
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  theCamera.setStillPictureImageFormat(320,240,CAM_IMAGE_PIX_FMT_RGB565);
}

void loop() {
  if(safeCount < 1000){
    safeCount += 1;
  }else{
    safeCount = 0;
    isSafe = false;
  }

  // Unityからsが来たら一定時間isSafeをtrueにする
  if (Serial2.read() == 's'){
    Serial.println("canbrella is safe");
    isSafe = true;
    safeCount = 0;
    //safeLED点灯
  }else{
    Serial.println("Incorrect password");
    //dangerLED点灯
  }

  recognizeOpen();

  if (isOpen == true){
    if (isSafe == true){
      sendActivity(); 
    }else{
      Serial.println("Call takePicture()");
      CamImage img = theCamera.takePicture();
      sendActivity();
      sendPicture();
    }
  }

}


void sendActivity(){

}

void sendPicture(){
  if (img.isAvailable()){
    Serial.println("Start sending Image");
    Serial2.write('h');

    delay(1000);

    img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    char *buf = img.getImgBuff();
    for (int i = 0; i < img.getImgSize(); ++i, ++buf) {
      Serial2.print((int)*buf);
      delay(1);
      Serial2.print(",");
      if(i%640 == 639)Serial2.println(" ");
    }

    Serial.println("End sending Image");
  }
}

void recognizeOpen(){
  // 傘が開かれたかどうか識別しisOpenedにbool値をいれる
}
