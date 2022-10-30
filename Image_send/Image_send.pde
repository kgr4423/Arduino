import processing.serial.Serial;
import java.io.*;
Serial myPort; 

OutputStream output;
PImage img;
String filename;
boolean started = false;
int file_number = 0;
int serialTimer = 0;
int nodata_counter = 0;

void setup() {
  size(1280, 960); background(0);
  printArray(Serial.list());
  // when debugging, you may have 2 serial ports
  myPort = new Serial(this, Serial.list()[1], 9600);
  // myPort = new Serial(this, Serial.list()[0], 2000000);
  myPort.clear();
}

void draw() {
  try {
    if (started == false) return;
    
    // The serial of Processing communicates every 50msec
    if (millis() - serialTimer > 50) {
      serialTimer = millis();
      if (myPort.available() <= 0) {
        ++nodata_counter; 
        return;
      }
      while (myPort.available() > 0) {
        output.write(myPort.read());
      }
      nodata_counter = 0;
    }
  } catch (Exception e) {
    e.printStackTrace();
    return;
  }
}

void keyPressed() {
  started = true;
  filename = "foge" + String.format("%03d", file_number) + ".jpg";
  output = createOutput(filename);  
  ++file_number;
  myPort.write('S');
  myPort.clear();
}

void mousePressed() {
  println("nodata_counter: " + String.format("%d", nodata_counter));
  // wait 100msec after receiving all jpg data
  if (nodata_counter < 2 || started == false) {
    return;
  }

  try {
    output.flush();
    output.close();
  } catch (Exception e) {
    e.printStackTrace();
    return;
  }  
  
  img = loadImage(filename);
  image(img, 0, 0);
  started = false;
}
