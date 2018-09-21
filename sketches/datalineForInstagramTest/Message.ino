void doMessage() {
  int xOffset = 0;
  
  
  String text = message;
  
  int index = 0;
  while (index < text.length()) {

    // Display Letter
    char c = (char) text[index];
    uint8_t w = disorientFont2017Widths[(int) c];
    for (int y = 0; y < 8; y++) {
      uint16_t letter = disorientFont2017[c][y];

      for (int x = 0; x < w; x++) {
        int thisX = x + xOffset + scrollOffset;
        if (thisX >= 0 && thisX < kMatrixWidth && (letter >> (15 - x)) & 1) {
          buffer[thisX * letterSpacing + stripOrder[y] * ledsPerStrip] = 0x888888;
        }
      }
    }

    xOffset += 1 + w;
    index++;
  }
  scrollOffset--;
  if (scrollOffset < -textWidth) {
    scrollOffset = kMatrixWidth;
  }
}

