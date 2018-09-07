bool isGlitching = false;
uint32_t nextGlitch = random(100);
int glitchFrames = random(1, 6);
int glitchStrip = random(100);

void resetGlitch() {
  nextGlitch = frame + random(1, 60);
  glitchFrames = random(1, 10);
  isGlitching = false;
  glitchStrip = random(nStrips);
}

// Random strip flicker
void glitch() {
  if (frame >= nextGlitch) {
    isGlitching = true;
  }

  if (isGlitching) {
    int shift = random(4);
    if (frame >= nextGlitch && frame < nextGlitch + glitchFrames) {
      int start = ledsPerStrip * glitchStrip;
      int nFlicker = ledsPerStrip;
      for (int i = start; i < start + nFlicker; ++i) {
        int index = i % nLeds;
        if (buffer[index] >= 1) {
        uint32_t thisColor = palette[white];

          if (random(100) < 40) {
            thisColor = palette[yellow];
          }
          thisColor = shiftColor(thisColor, shift);
          buffer[index] = thisColor;
        }
        
      }
    } else {
      if (frame >= nextGlitch + glitchFrames) {
        resetGlitch();
      }
    }
  }
}

