#include <SDHCI.h>
SDClass SD;

void setup() {
  Serial.begin(115200);

  /* Initialize SD */
  while (!SD.begin()) {
    ; /* wait until SD card is mounted. */
  }

  /* Start USB MSC */
  if (SD.beginUsbMsc()) {
    Serial.println("USB MSC Failure!");
  } else {
    Serial.println("*** USB MSC Prepared! ***");
    Serial.println("Insert SD and Connect Extension Board USB to PC.");
  }
}

void loop() {
}
