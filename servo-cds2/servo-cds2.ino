//Cdsセンサの値に応じてサーボモーターの回転角度を変化させるプログラム・改
#include<Servo.h>

Servo servo;

int val=0;

void setup() {
  servo.attach(3);
  
  //シリアルポートを開く
  Serial.begin(9600);
}

void loop() {

  val = analogRead(0);

  Serial.print("A0=");
  Serial.print(val);
  Serial.print("\n");

  val = (360/(1005-885)) * (val-885);
  servo.write(val);
 
  delay(100);
  
}
