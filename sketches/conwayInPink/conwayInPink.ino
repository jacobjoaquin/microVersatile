#include <FastLED.h>

#define COLOR_ORDER  GRB
#define CHIPSET      WS2811
#define BRIGHTNESS   255

const uint8_t STRIP_PIN_0 = 2;
const uint8_t STRIP_PIN_1 = 7;
const uint8_t STRIP_PIN_2 = 14;

const uint8_t nPanels = 3;
const uint8_t panelWidth = 32;
const uint8_t panelHeight = 8;
const uint8_t kMatrixWidth = nPanels * panelWidth;
const uint8_t kMatrixHeight = panelHeight;
const bool  kMatrixSerpentineLayout = true;
const int nPixels = kMatrixWidth * kMatrixHeight;
const uint16_t nLEDs = nPixels;
const uint16_t nLEDsPerStrip = nLEDs / nPanels;

boolean grid[kMatrixHeight][kMatrixWidth];
boolean temp[kMatrixHeight][kMatrixWidth];

uint32_t frameDelay = 0;
uint32_t interpFrames = 8;
uint32_t interpCounter = interpFrames;
uint32_t nFrames = 120;
uint32_t frameCounter = nFrames;

CRGB cBlack = CRGB(0, 0, 0);
CRGB c0 = CRGB(255, 0, 32);
CRGB c1 = CRGB(128, 32, 0);
CRGB c2 = CRGB(255, 32, 128);

CRGB leds[nLEDs + 1];

// Map serpentine to grid
int16_t XY(uint16_t x, uint16_t y) {
  // The first LED panel in microVersatile is wired in upside. This compensates.
  if (x < panelWidth) {
    x = (panelWidth - 1) - x;
    y = (panelHeight - 1) - y;
  }
  
  int i = x % 2;
  return (x + i) * kMatrixHeight - i + (i ? -y : y);
}

void initGrid() {
  uint32_t odds = random(10, 60);
  for (long y = 0; y < kMatrixHeight; y++) {
    for (long x = 0; x < kMatrixWidth; x++) {
      grid[y][x] = random(100) < odds;
    }
  }
}

void initGlider() {
  grid[0][1] = true;
  grid[1][2] = true;
  grid[2][0] = true;
  grid[2][1] = true;
  grid[2][2] = true;
}

boolean isAlive(long x, long y) {
  return grid[y][x];
}

int countAdjacent(int x, int y) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (!(i == 0 && j == 0)) {
        int xOffset = x + j;
        int yOffset = y + i;
        if (xOffset < 0) {
          xOffset += kMatrixWidth;
        } else if (xOffset >= kMatrixWidth) {
          xOffset -= kMatrixWidth;
        }
        if (yOffset < 0) {
          yOffset += kMatrixHeight;
        } else if (yOffset >= kMatrixHeight) {
          yOffset -= kMatrixHeight;
        }

        if (isAlive(xOffset, yOffset)) {
          count++;
        }
      }
    }
  }
  return count;
}

void tempToGrid() {
  for (int y = 0; y < kMatrixHeight; y++) {
    for (int x = 0; x < kMatrixWidth; x++) {
      grid[y][x] = temp[y][x];
    }
  }
}

void setup() {
  FastLED.addLeds<CHIPSET, STRIP_PIN_0, COLOR_ORDER>(leds, 0, nLEDsPerStrip).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, STRIP_PIN_1, COLOR_ORDER>(leds, nLEDsPerStrip, nLEDsPerStrip).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, STRIP_PIN_2, COLOR_ORDER>(leds, 2 * nLEDsPerStrip, nLEDsPerStrip).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  randomSeed(analogRead(0) + analogRead(1) + analogRead(2));
  initGrid();
  //  initGlider();
}

int counter = 0;
void loop() {
  FastLED.clear();

  for (int y = 0; y < kMatrixHeight; y++) {
    for (int x = 0; x < kMatrixWidth; x++) {
      boolean isAlive = grid[y][x];

      int liveCells = countAdjacent(x, y);
      if (isAlive) {
        temp[y][x] = liveCells == 2 || liveCells == 3;
      } else {
        temp[y][x] = liveCells == 3;
      }
    }
  }

  for (uint32_t i = 0; i < interpFrames; i++) {
    FastLED.clear();

    for (long y = 0; y < kMatrixHeight; y++) {
      for (long x = 0; x < kMatrixWidth; x++) {
        int nAdjacent = countAdjacent(x, y);
        CRGB c;

        if (nAdjacent == 8) {
          c = c2;
        } else {
          c = c0.lerp8(c1, nAdjacent * 32);
        }

        float n = (float) i / interpFrames;
        float g0 = grid[y][x];
        float g1 = temp[y][x];

        float amt = (1 - n) * g0 + n * g1;
        CRGB temp = cBlack.lerp8(c, amt * 255);

        leds[XY(x, y)] = temp;
      }
    }
    
    FastLED.show();
    delay(frameDelay);
  }

  tempToGrid();

  frameCounter--;
  if (frameCounter == 0) {
    initGrid();
    frameCounter = nFrames;
  }
}

