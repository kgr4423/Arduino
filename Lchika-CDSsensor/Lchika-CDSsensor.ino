//Cdsセンサを使って、センサに手を近づけるとLEDが明るくなるシステム

const int led = 9;
const int sensorPin = 0;
int val = 0;

void setup(){
  pinMode(led, OUTPUT);
}
void loop(){
  val = analogRead(sensorPin);
  int intensity = val/4;
  analogWrite(led, intensity);
  delay(100);
}
