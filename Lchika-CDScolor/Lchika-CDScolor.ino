//Cdsセンサの値に応じてLEDの色を変化させるプログラム
int val=0;

void setup() {
  //デジタルpin11,12,13を使うよ
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT); 
  //シリアルポートを開く
  Serial.begin(9600);
}

void loop() {
  val = analogRead(0);

  Serial.print("A0=");
  Serial.print(val);
  Serial.print("\n");

  if(val<=200){
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, HIGH);
  }else if(val<=450){
    digitalWrite(11, LOW);
    digitalWrite(13, LOW);
    digitalWrite(12, HIGH);
  }else{
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  }

  delay(100);
}
