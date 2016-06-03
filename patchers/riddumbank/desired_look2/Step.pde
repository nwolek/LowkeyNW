class Step {
  
  int w, h;
  int x, y;
  boolean marked = false;
  
  Step(int newWidth, int newHeight, int newX, int newY) {
    w = newWidth;
    h = newHeight;
    x = newX;
    y = newY;
  }
  
  void drawOnScreen() {
    if (marked) {
      fill(100);
    } else {
      fill(200);
    }
    stroke(0);
    strokeWeight(3);
    rectMode(CORNER);
    rect(x,y,w,h);
  }
  
  void updateMark(boolean m) {
    marked = m;
  }
}