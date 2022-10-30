
String voice;
const int red = 7;
const int green = 6;
const int blue = 5;

void RedOn() {
  digitalWrite (red, HIGH);
}
void RedOff() {
  digitalWrite (red, LOW);
}
void greenOn() {
  digitalWrite (green, HIGH);
}
void greenOff() {
  digitalWrite (green, LOW);
}
void BlueOn() {
  digitalWrite (blue, HIGH);
}
void BlueOff() {
  digitalWrite (blue, LOW);
}
void allon() {
  digitalWrite (red, HIGH);
  digitalWrite (green, HIGH);
  digitalWrite (blue, HIGH);
}
void alloff() {
  digitalWrite (red, LOW);
  digitalWrite (green, LOW);
  digitalWrite (blue, LOW);
}
void setup() {
  Serial.begin(9600);
  Serial2.begin(38400);
  pinMode(red, OUTPUT);
  digitalWrite (red, LOW);
  pinMode(green, OUTPUT);
  digitalWrite (green, LOW);
  pinMode(blue, OUTPUT);
  digitalWrite (blue, LOW);
}
void loop() {

  while (Serial2.available()) {
    delay(15);
    char c = Serial2.read();
    if (c == '#') {
      break;
    }
    voice += c;
  }
  if (voice.length() > 0) {
    Serial.println(voice);
    if (voice == "open")
    {
      greenOn();
      delay(3000);
      alloff();
    }
    else if (voice == "close"){
      alloff();
    }else{
      RedOn();
      delay(1000);
      alloff();
    }
    
    voice = "";
  }
}
