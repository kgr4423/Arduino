//プログラムを開始してからの時間をミリ秒単位で測るプログラム

unsigned long time;

void setup() {
  Serial.begin(9600);
}

void loop() {
  time = millis();
  Serial.println(time);

  delay(1000);

}
