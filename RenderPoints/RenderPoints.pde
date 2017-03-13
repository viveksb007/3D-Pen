void setup(){
  size(600,400);
}

int prevX,prevY;
boolean first = true;

void draw(){
  /*  Renders only when mouse is pressed
  
  if((mousePressed)&&(!first)){
    line(mouseX,mouseY,prevX,prevY);
    prevX = mouseX;
    prevY = mouseY;
  }else{
    prevX = mouseX;
    prevY = mouseY;
    first = false;
  } */
}

void mouseClicked(){
  if(!first){
    line(mouseX,mouseY,prevX,prevY);
    prevX = mouseX;
    prevY = mouseY;
  }else{
    prevX = mouseX;
    prevY = mouseY;
    first = false;
  }
}