#include <SDHCI.h>
SDClass SD;

void setup() {
  Serial2.begin(115200);

  // SDの初期化
  while (!SD.begin()) {
    ; // SDカードの挿入を認識するまで待機
  }

  //USB MSCの開始
  if (SD.beginUsbMsc()) {
    Serial2.println("USB MSC Failure!");
  } else {
    Serial2.println("*** USB MSC Prepared! ***");
    Serial2.println("Insert SD and Connect Extension Board USB to PC.");
  }
  
}

void loop() {
}
