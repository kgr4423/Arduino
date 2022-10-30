//LED２つを交互に光らせるプログラム

void setup() {
  //Arduinoに12,13番ピンを出力用として使うと知らせる
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, LOW);
  digitalWrite(12, HIGH);
  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  delay(1000);
}
