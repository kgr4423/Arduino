#include <Camera.h>

#define BAUDRATE 38400

void setup() {
  Serial.begin(9600);
  Serial2.begin(BAUDRATE);
  while (!Serial2) {
    ;
  }

  theCamera.begin();
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  theCamera.setStillPictureImageFormat(
     320,
     240,
     CAM_IMAGE_PIX_FMT_RGB565);
     int i =0;
  for(i = 5;i <= 11;i++){
    if(i ==8)continue;
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
  }
  
  
}

void loop() {
  while(true){
    
  };
  while (Serial2.available() <= 0){
    Serial.println("waiting data");
    delay(100);
  } 

  if (Serial2.read() != 's'){
    Serial.println("waiting 's' ");
    return;
  }

  Serial.println("Call takePicture()");
  CamImage img = theCamera.takePicture();

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

  while(Serial2.read() != 'e'){
    Serial.println("waiting 'e' ");
  }

}
