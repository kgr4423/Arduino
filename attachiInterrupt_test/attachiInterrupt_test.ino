//タクトスイッチによる割り込み LED制御

int pin = 13;
volatile int state = HIGH;

void blink() {
  Serial.print("brink\n");
  state = !state;
}

void setup() {
  Serial.begin(9600);
  pinMode(pin, OUTPUT);
  attachInterrupt(0, blink, CHANGE);
}

void loop() {
  Serial.print("loop\n");
  digitalWrite(pin, state);
}
