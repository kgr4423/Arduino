void setup() {
  Serial.begin(115200); //シリアルポートを115200bpsで開く
}

void loop()
{
  //文字列のメッセージを1秒毎にシリアルに送信する
  Serial.print("Hello, this is Arduino.");
  Serial.println();
  delay(1000);
}
