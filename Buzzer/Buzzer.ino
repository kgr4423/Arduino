//50%DT比の矩形波を生成するプログラム

#define SPK_PIN 9

void setup() {
  pinMode(SPK_PIN, OUTPUT);
}

void loop() {
  tone(SPK_PIN, 500);
  delay(2000);
  tone(SPK_PIN, 1000);
  delay(2000);

}
