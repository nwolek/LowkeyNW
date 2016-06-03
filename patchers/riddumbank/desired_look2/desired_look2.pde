int columns = 16;
int rows = 4;
int totalSteps = columns * rows;

int stepWidth, stepHeight;

Step[] mySteps = new Step[totalSteps];

void setup() {
  size(400,100);
  
  stepWidth = width / columns;
  stepHeight = height / rows;
  
  for (int i = 0; i < mySteps.length; i++) {
    
    int xPos = i % columns;
    xPos = stepWidth * xPos;
    
    int yPos = i / columns;
    yPos = stepHeight * yPos;
    
    mySteps[i] = new Step(stepWidth,stepHeight,xPos,yPos);
    //println(i + " " + xPos + " " + yPos);
  }
  
  
  
  
}

void draw() {
  background(255);
  
  for (int i = 0; i < mySteps.length; i++) {
    mySteps[i].drawOnScreen();
  }
  
}

void keyPressed(){
  if ('1' <= key && key <= '9')
  {
    int skipRate = key-'0';
    for (int i = 0; i < mySteps.length; i++) {
      mySteps[i].updateMark(false);
    }
    for (int i = 0; i < mySteps.length; i=i+skipRate) {
      mySteps[i].updateMark(true);
    }
  }
}