int columns = 16;
int rows = 4;
int totalSteps = columns * rows;

int stepWidth, stepHeight;
int currentStep;
int lastSecond;

Step[] mySteps = new Step[totalSteps];
float[] pattern = new float[totalSteps];
float[] markers = new float[totalSteps];

void setup() {
  size(800,200);
  
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
    mySteps[i].displayBase(markers[i]);
  }
  
  for (int i = 0; i < mySteps.length; i++) {
    mySteps[i].displayPattern(pattern[i]);
  }
  
  mySteps[currentStep].displayCurrentStep();
  
  int s = second();
  if (s != lastSecond) {
    currentStep++;
    if (currentStep >= totalSteps) currentStep = 0;
  }
  lastSecond = s;
  
}

void mousePressed(){
  int mouseColumn = mouseX / stepWidth;
  int mouseRow = mouseY / stepHeight;
  //println("mouse col " + mouseColumn + " row " + mouseRow);
  int stepPressed = columns * mouseRow + mouseColumn;
  
  if (pattern[stepPressed] == 0.0){
    pattern[stepPressed] = 1.0;
  } else {
    pattern[stepPressed] = 0.0;
  }
}

void keyPressed(){
  if ('1' <= key && key <= '9')
  {
    int skipRate = key-'0';
    markers = new float[totalSteps];
    for (int i = 0; i < mySteps.length; i=i+skipRate) {
      markers[i] = 1.0;
    }
  }
  
  if (key == 'm')
  {
    for (int i = 0; i < markers.length; i++) {
      if (markers[i] != 0.0 && random(0,100) > 50)
      {
        pattern[i] = markers[i];
      }
    }
  }
  
  if (key == 'M')
  {
    for (int i = 0; i < markers.length; i++) {
      if (markers[i] != 0.0)
      {
        pattern[i] = markers[i];
      }
    }
  }
  
  if (key == 'c')
  {
    pattern = new float[totalSteps];
  }
  
}