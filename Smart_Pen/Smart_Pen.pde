import processing.serial.*;

Serial myPort;
int x,y,z;

void setup(){
  size(600,400);
  String portName = Serial.list()[0]; // assuming data is coming from 0 port
  myPort = new Serial(this,portName,9600);
}

void draw(){
  if(myPort.available()>0){
    
  }
}