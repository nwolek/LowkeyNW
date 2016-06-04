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
  size(800, 200);

  stepWidth = width / columns;
  stepHeight = height / rows;

  for (int i = 0; i < mySteps.length; i++) {

    int xPos = i % columns;
    xPos = stepWidth * xPos;

    int yPos = i / columns;
    yPos = stepHeight * yPos;

    mySteps[i] = new Step(stepWidth, stepHeight, xPos, yPos);
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

void mousePressed() {
  int mouseColumn = mouseX / stepWidth;
  int mouseRow = mouseY / stepHeight;
  //println("mouse col " + mouseColumn + " row " + mouseRow);
  int stepPressed = columns * mouseRow + mouseColumn;

  if (pattern[stepPressed] == 0.0) {
    pattern[stepPressed] = 1.0;
  } else {
    pattern[stepPressed] = 0.0;
  }
}

void keyPressed() {
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
    quickFillRandomMarkers();
  }

  if (key == 'M')
  {
    quickFillAllMarkers();
  }

  if (key == 'r')
  {
    quickFillRandomPattern();
  }

  if (key == 'c')
  {
    clearPattern();
  }
  
  if (key == 'g')
  {
    rotatePattern(1);
  }
  
  if (key == 'f')
  {
    rotatePattern(-1);
  }
}

void clearPattern() {
  pattern = new float[totalSteps];
}

void quickFillAllMarkers() {
  for (int i = 0; i < markers.length; i++) {
    if (markers[i] != 0.0) {
      pattern[i] = 1.0;
    }
  }
}

void quickFillRandomMarkers() {
  for (int i = 0; i < markers.length; i++) {
    if (markers[i] != 0.0) {
      if (random(0, 100) > 50) {
        pattern[i] = 1.0;
      } else {
        pattern[i] = 0.0;
      }
    }
  }
}

void quickFillRandomPattern() {
  for (int i = 0; i < pattern.length; i++) {
    if (random(0, 100) > 50) {
      pattern[i] = 1.0;
    } else {
      pattern[i] = 0.0;
    }
  }
}

void rotatePattern(int rotateAmount) {
  float[] newPattern = new float[pattern.length];
  
  while (rotateAmount < 0) rotateAmount += pattern.length;
  
  for (int i = 0; i < pattern.length; i++) {
    int j = (i + rotateAmount) % pattern.length;
    newPattern[j] = pattern[i];
  }
  
  pattern = newPattern;
}