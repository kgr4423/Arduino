#include <Camera.h>

#define BAUDRATE 230400

int counter = 0;

void setup() {

  Serial.begin(9600);
  Serial2.begin(BAUDRATE);
  while (!Serial2) {
    ;
  }
}

void loop() {
  if(Serial2.read() != 'S'){
    return;
  }

  Serial.println("1");

  while(true){
    Serial2.write(counter);
    ++counter;

    Serial.println("2");

    //delay(1);

    if(Serial2.read() == 'E'){
      break;
    }

  }
  
}
