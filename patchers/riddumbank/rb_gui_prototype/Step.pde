class Step {

  int w, h;
  int x, y;

  Step(int newWidth, int newHeight, int newX, int newY) {
    w = newWidth;
    h = newHeight;
    x = newX;
    y = newY;
  }

  void displayBase(float v) {
    if (v != 0) {
      fill(100);
    } else {
      fill(200);
    }
    stroke(0);
    strokeWeight(3);
    rectMode(CORNER);
    rect(x, y, w, h);
  }

  void displayPattern(float v) {
    if (v != 0) {
      fill(255, 0, 0, 150);
      stroke(255, 0, 0);
      strokeWeight(2);
      rect(x, y, w, h);
    }
  }

  void displayCurrentStep() {
    fill(0, 255, 0, 150);
    stroke(0, 255, 0);
    strokeWeight(2);
    rect(x, y, w, h);
  }

}