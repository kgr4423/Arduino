//スイッチを押したら、手をはなしてもLEDのONOFFが維持されるプログラム

int val=0;
int count=0;

void setup(){
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  val = analogRead(0);
  

  Serial.print("A0=");
  Serial.print(val);
  Serial.print("\n");

  if(val==0){
    digitalWrite(13, LOW);
  }else{
    digitalWrite(13, HIGH);
  }
}
