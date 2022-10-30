#include <Camera.h>

void setup()
{
  Serial.begin(9600);
  Serial2.begin(230400);
  while (!Serial) {};
  while (!Serial2) {};

  Serial.println("Prepare camera");
  theCamera.begin();

  Serial.println("Set Auto white balance parameter");
  theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT); 

  Serial.println("Set still picture format");
  theCamera.setStillPictureImageFormat(
     CAM_IMGSIZE_QUADVGA_H,
     CAM_IMGSIZE_QUADVGA_V,
     CAM_IMAGE_PIX_FMT_JPG);
}

void loop()
{
  while (Serial2.available() <= 0);
  // taking a picture is started by receiving 'S'
  if (Serial2.read() != 'S') return;
 
  Serial.println("call takePicture()");
  CamImage img = theCamera.takePicture();

  if (!img.isAvailable()) {
    Serial.println("take image failure");
    return;
  }

  Serial.println("start transfer");
  char* buf = img.getImgBuff();
  for (int i = 0; i < img.getImgSize(); ++i, ++buf) {
    Serial2.write(*buf);
  }
  Serial.println("end transfer");
}
