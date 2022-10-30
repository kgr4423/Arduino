#include <Camera.h>

#define BAUDRATE 38400

void CamCB(CamImage img) {

  if (img.isAvailable() == false){
    return;
  }

  while (Serial2.available() <= 0){
    delay(100);
    ;
  } 
  
  // take a picture by receiving 'S'
  if (Serial2.read() != 'S'){
    return;
  }
  delay(1);

  digitalWrite(LED0, HIGH);
  img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
  char *buf = img.getImgBuff();
  for (int i = 0; i < img.getImgSize(); ++i, ++buf) {
    Serial2.write(*buf);
  }
  digitalWrite(LED0, LOW);
  while(Serial2.read() != 'E'){
    Serial2.write('0');
  }
  
}

void setup() {

  Serial2.begin(BAUDRATE);
  while (!Serial2) {
    ;
  }

  theCamera.begin();
  theCamera.startStreaming(true, CamCB);
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
}

void loop() {
  /* do nothing here */
}
