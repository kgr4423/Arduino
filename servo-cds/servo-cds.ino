//Cdsセンサの値に応じてサーボモーターの回転角度を変化させるプログラム
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

  if(val<=910){
    servo.write(0);
  }else if(val<=970){
    servo.write(90);
  }else{
    servo.write(180);
  }

  delay(100);
  
}
