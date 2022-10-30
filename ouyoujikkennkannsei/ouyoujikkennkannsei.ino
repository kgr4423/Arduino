#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
const int led=13;
const int Button=7;
int val=0;
int old_val=0;
int state=0;
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;
float x,y,z;
float theta1a, theta2a,theta1b,theta2b,theta1,theta2;
float old_theta1=8,old_theta2=0;
const float pi = 3.14;
float g_x,g_y,g_z;
float cosa,cosb,cos_;
unsigned long time_;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupMPU6050();
    // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(Button,INPUT);
}


void loop() {
  recordAccelRegisters();
  recordGyroRegisters();
  fixOffset();
  //printData();
  CaluculateTheta();
  delay(200);
}

void setupMPU6050() {
  //MPU6050との通信を開始し、ジャイロと加速度の最大範囲を指定
    val=digitalRead(Button);
  if(val==HIGH&&old_val==LOW&&state==LOW){
    digitalWrite(led,HIGH);
    state=HIGH;
    delay(500);
    }else if(val==HIGH&&old_val==LOW&&state==HIGH){
      digitalWrite(led,LOW);
       state=LOW;
      delay(500);
      }
   old_val=digitalRead(Button);
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x6B); //Accessing the register 6B
  Wire.write(0b00000000); //SLEEP register to 0
  Wire.endTransmission();
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration
  Wire.write(0x00000000); //gyro to full scale ± 250deg./s
  Wire.endTransmission();
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration
  Wire.write(0b00000000); //accel to +/- 2g
  Wire.endTransmission();
}

void recordAccelRegisters() {
  //加速度読み取り
  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B); // Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); //Request Accel Registers (3B - 40)
  while (Wire.available() < 6);
  accelX = Wire.read() << 8 | Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read() << 8 | Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read() << 8 | Wire.read(); //Store last two bytes into accelZ
  calculateAccelData();
}

void calculateAccelData() {
  //読み取った値をgに変換
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0;
  gForceZ = accelZ / 16384.0;
}

void recordGyroRegisters() {
  //ジャイロの値を読み取る
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); //Request Gyro Registers (43 - 48)
  while (Wire.available() < 6);
  gyroX = Wire.read() << 8 | Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read() << 8 | Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read() << 8 | Wire.read(); //Store last two bytes into accelZ
  calculateGyroData();
}

void calculateGyroData() {
  //読み取った値をdeg/secに変換
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0;
  rotZ = gyroZ / 131.0;
}

void fixOffset(){
  rotX = rotX - 3.7; 
  rotY = rotY + 2.0;
  rotZ = rotZ + 0.6;
  gForceX = gForceX - 0.06;
  gForceY = gForceY + 0.01;
  gForceZ = gForceZ + 0.06;
}


void printData() {
  //シリアルモニタに出力
  Serial.print("Gyro");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print("deg/sec");
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print("deg/sec");
  Serial.print(" Z=");
  Serial.print(rotZ);
  Serial.print("deg/sec");
  Serial.print("  Accel");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print('g');
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print('g');
  Serial.print(" Z=");
  Serial.print(gForceZ);
  Serial.println('g');

  

}

float calccos1(float a1,float a2,float b1,float b2){
  cos_=sin(a2)*sin(b2)+cos(a2)*cos(b2);
    return cos_;
  }


void CaluculateTheta(){
  theta1a =atan2(-1*gForceX, gForceZ);
  theta2a = atan2(-1*cos(theta1a)*cos(theta1a)*gForceY, cos(theta1a)*gForceZ);
  theta1b = atan2(gForceX, -1*gForceZ);
  theta2b = atan2(-1*cos(theta1b)*cos(theta1b)*gForceY, cos(theta1b)*gForceZ);
  //g_x=-1*cos(theta1a)*gForceX+sin(theta1a)*cos(theta2a)*gForceY+sin(theta1a)*sin(theta2a)*gForceZ;
  //g_y=sin(theta2a)*gForceY-cos(theta2a)*gForceZ;
  //g_z=-1*sin(theta1a)*gForceX-cos(theta1a)*cos(theta2a)*gForceY-cos(theta1a)*sin(theta2a)*gForceZ;
  //g_z=-sin(theta1a)*gForceZ*gForceZ*(-1*cos(theta1)*sin(theta2))*10000000;
  //g_y=sin(theta1a)*cos(theta2a)*gForceX+sin(theta2a)*gForceY-cos(theta1a)*cos(theta2a)*gForceZ;

  //difa=sqrt(g_x*g_x+(g_y+1)*(g_y+1)+g_z*g_z);

    cosa=calccos1(theta1a,theta2a,old_theta1,old_theta2);
  cosb=calccos1(theta1b,theta2b,old_theta1,old_theta2);
  if(abs(sin(theta2)>0.95)){
      old_theta2+=(millis()-time_)*rotX;
    }
  if(cosa>cosb){
      theta1=theta1a;
      theta2=theta2a;
    }else{
      theta1=theta1b;
      theta2=theta2b;
      }
  old_theta1=theta1;
  old_theta2=theta2;
  
    theta2 = theta2 * 180 / 3.14;
  theta1 = theta1 * 180/3.14;
  theta2b = theta2b * 180 / 3.14;
  theta1b = theta1b * 180 / 3.14;
  Serial.print(theta1);
  Serial.print(";") ;
  Serial.print(theta2);
  Serial.print(";") ;
    Serial.print(theta1b);
  Serial.print(";") ;
  Serial.print(theta2b);
  Serial.print(";") ;
  Serial.print(cosa);
  Serial.print(";") ;
  Serial.print(cosb);
  Serial.print(";") ;
  Serial.print(state);
    //Serial.print(";") ;
  //Serial.print(g_y);
  //Serial.print(";") ;
  //Serial.print(gForceX);
  //Serial.print(";") ;
  //Serial.print(gForceY);
  Serial.println();
  time_=millis();
}
