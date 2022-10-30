//********************************
//加速度センサの値を取得するプログラム
//********************************
void setup()
{
  // シリアルモニターの初期化をする
  Serial.begin(9600) ;
}
void loop()
{
  long x , y , z ;
  x = y = z = 0 ;
  x = analogRead(5) ; // Ｘ軸
  y = analogRead(4) ; // Ｙ軸
  z = analogRead(3) ; // Ｚ軸
  Serial.print("X:") ;
  Serial.print(x) ;
  Serial.print(" Y:") ;
  Serial.print(y) ;
  Serial.print(" Z:") ;
  Serial.println(z) ;
  
  Serial.print(" theta1:") ;
  Serial.println(atan2(y, x));
  Serial.print(" theta2:") ;
  Serial.println(atan2(z, sqrt(pow(x, 2)+pow(y, 2))));
  
  delay(50) ;
}
