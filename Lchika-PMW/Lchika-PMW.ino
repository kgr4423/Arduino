//LEDを光らせるプログラム
//aruduinoのpin13にLEDのアノードを、GNDにカソードを接続する
void setup() {
  // put your setup code here, to run once:
  //デジタルpin13を使うよ
  pinMode(13, OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH); //デジタルpin13の出力ON
  delay(1); //1秒まつ
  digitalWrite(13, LOW); //デジタルpin13の出力OFF
  delay(1); //delay(1000); //1秒まつ
}
