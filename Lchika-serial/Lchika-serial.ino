//シリアル通信を利用してCDSセンサの出力を読み取る

const int led = 9;
const int sensorPin = 0;
int val = 0;

void setup(){
  Serial.begin(9600);  //シリアルポートを開く
                       //毎秒9600bitでコンピュータにデータを通信
}

void loop(){
  val = analogRead(sensorPin); //センサから値を取得
  Serial.print("A0=");         //シリアルポートに文字列を出力
  Serial.print(val);           //シリアルポートに値を出力
  Serial.print("\n");
  delay(100);
}
