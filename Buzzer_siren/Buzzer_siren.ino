//50%DT比の矩形波を生成するプログラム

#define SPK_PIN 9

void setup() {
  pinMode(SPK_PIN, OUTPUT);
}

void loop() {
  int i;
  for(i=0; i<256; ++i){
    tone(SPK_PIN, i*10, 20);
    delay(10);
  }

}
