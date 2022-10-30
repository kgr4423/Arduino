
void setup() {
  Serial.begin(9600);
  Serial2.begin(38400);
}


void loop()
{
  //Unity側に送信
  Serial2.print("Hello, this is Arduino.");
  Serial2.println();

  //ループ条件
  bool canLoop = false;
  if(Serial2.available())canLoop = true;
  
  while (canLoop) {
    //受信したデータの1バイトを読み取りcに入れる
    char c = Serial2.read();

    //見やすい形で出力
    char buf[100];
    sprintf(buf, "received: %d | %c", c, c);
    Serial.println(buf);

    //出力できるデータがなくなったら終了
    if(!Serial2.available()){
      canLoop = false;
      Serial.println(" ");
    }
  }

  delay(100);


}
