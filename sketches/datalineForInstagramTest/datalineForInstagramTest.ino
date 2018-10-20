/*
  Dataline
  By Jacob Joaquin.

  This is a port of the LED patterns used for Disorient's 2018 frontage.
  https://github.com/jacobjoaquin/D18/tree/master/helios/dataline



  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <FastLED.h>
#include "disfont2017.h"

// Combine rbg values to color
#define rgb(R, G, B)  ((((uint32_t)(R)) << 16) | (((uint32_t)(G)) << 8) | ((uint32_t)(B)))
#define COLOR_ORDER  GRB
#define CHIPSET      WS2811
#define BRIGHTNESS   255




// Agent
struct Agent {
  float position;
  uint8_t length;
  float direction;
  uint8_t color;
  uint8_t framesLeft;
  uint8_t nFrames;
};




// User-defined
const int frameRate = 60;
const int letterSpacing = 1;

// User defined colors
uint32_t colorOrange = rgb(255, 48, 0);
uint32_t colorMagenta = rgb(128, 0, 92);
uint32_t colorBlack = rgb(0, 0, 0);
uint32_t colorWhite = rgb(255, 255, 255);
uint32_t colorPink = rgb(128, 0, 64);
uint32_t colorCyan = rgb(0, 255, 128);
uint32_t colorYellow = rgb(255, 255, 128);

// Color constants
const uint8_t black = 0;
const uint8_t orange = 1;
const uint8_t pink = 2;
const uint8_t white = 3;
const uint8_t magenta = 4;
const uint8_t cyan = 5;
const uint8_t yellow = 6;
uint32_t palette[] = {colorBlack, colorOrange, colorPink, colorWhite, colorMagenta, colorCyan, colorYellow};

const uint8_t color0 = magenta;
const uint8_t color1 = cyan;

// Program constants
const uint8_t stripOrder[] = {0, 1, 2, 3, 4, 5, 6, 7};
const int ledsPerStrip = 96;
const int nStrips = 8;
const int nLeds = ledsPerStrip * nStrips;
const int frameDelay = 1000 / frameRate;
const uint8_t STRIP_PIN_0 = 2;
const uint8_t STRIP_PIN_1 = 7;
const uint8_t STRIP_PIN_2 = 14;
const int kMatrixHeight = 8;
const int kMatrixWidth = 96;
const uint8_t panelWidth = 32;
const uint8_t panelHeight = 8;

// Fast LED setup
CRGB ledsBuffer[nLeds + 1];

// LED Buffer
uint32_t buffer[nLeds] = {0};

// Random buffer (noise)
uint8_t randomBuffer[nLeds] = {255};

// Sanity led
boolean sanityLED = true;
uint32_t sanityDelay = 48;
uint32_t sanityNextSwitch;
int sanityPin = 13;

// Agents
const int nAgents = 1600;
Agent agentList[nAgents];
int agentLength = 10;
int agentFrames = 40;
int agentIndex = 0;

// Timing
ulong showTime = millis() + frameDelay;
uint32_t frame = 0;


uint16_t textWidth = 0;
int scrollOffset = 0;
const String message = "Welcome to live practive.                                              ";

void setup() {
  FastLED.addLeds<CHIPSET, STRIP_PIN_0, COLOR_ORDER>(ledsBuffer, 0, 256).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, STRIP_PIN_1, COLOR_ORDER>(ledsBuffer,  256,  256).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, STRIP_PIN_2, COLOR_ORDER>(ledsBuffer, 2 *  256,  256).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);

  setupRandomSeed();
  pinMode(sanityPin, OUTPUT);
  digitalWrite(sanityPin, sanityLED);
  sanityNextSwitch = millis() + sanityDelay;
  initAgentList();


  for (int i = 0; i < message.length(); i++) {
    char c = (char) message[i];
    uint8_t w = disorientFont2017Widths[(int) c];
    textWidth += w + 1;
  }

}

void loop() {
  // Reset
  memset(&buffer[0], 0, sizeof(buffer));
  FastLED.clear();

  // Fill randomBuffer with noise
  if (!(frame % 4)) {
    for (int i = 0; i < nLeds; ++i) {
      randomBuffer[i] = random(256);
    }
  }

  // Agents
  doAgents();

  // Disorient
  //    if (!(frame % 240)) {
  //      cycleDisorient();
  //    }

  // Update the agents
  updateAgents();

  // Glitch
  glitch();

  // Message
//  doMessage();

  // Last
  bufferToLEDs();
  displayLEDs();
  FastLED.show();

  ++frame;
}

