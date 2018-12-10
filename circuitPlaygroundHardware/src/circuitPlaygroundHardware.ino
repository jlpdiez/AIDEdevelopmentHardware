#include <Adafruit_Circuit_Playground.h>
#include <Wire.h>
#include <SPI.h>

// *********************************************
// DEFINITIONS AND OBJECT CREATION
// *********************************************
// Color gamma correction table, this makes the hues of the NeoPixels
// a little more accurate by accounting for our eye's non-linear color
// sensitivity.  See this great guide for more details:
//   https://learn.adafruit.com/led-tricks-gamma-correction/the-issue
const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

//Random frequencies and phases will be generated for each pixel to
//define an organic but random looking flicker effect.
float frequencies[10] = {0};     
float phases[10] = {0};          
//The value 35 degrees is a nice orange in the middle of red and yellow 
//that will look like a flame flickering as the hues animate.
const uint8_t FLAMEHUE = 35;

//Data for speaking
const uint8_t spDANGER[] PROGMEM = {0x2D,0xBF,0x21,0x92,0x59,0xB4,0x9F,0xA2,0x87,0x10,0x8E,0xDC,0x72,0xAB,0x5B,0x9D,0x62,0xA6,0x42,0x9E,0x9C,0xB8,0xB3,0x95,0x0D,0xAF,0x14,0x15,0xA5,0x47,0xDE,0x1D,0x7A,0x78,0x3A,0x49,0x65,0x55,0xD0,0x5E,0xAE,0x3A,0xB5,0x53,0x93,0x88,0x65,0xE2,0x00,0xEC,0x9A,0xEA,0x80,0x65,0x82,0xC7,0xD8,0x63,0x0A,0x9A,0x65,0x5D,0x53,0xC9,0x49,0x5C,0xE1,0x7D,0x2F,0x73,0x2F,0x47,0x59,0xC2,0xDE,0x9A,0x27,0x5F,0xF1,0x8B,0xDF,0xFF,0x03};
const uint8_t spMAYDAY[] PROGMEM = {0x66,0xB7,0x52,0x7A,0xCC,0x9D,0x84,0xDD,0x4B,0xA9,0x36,0x77,0x12,0x96,0xC8,0x54,0x27,0x2C,0x5E,0x1A,0x8A,0x10,0xEE,0xB4,0x78,0xAB,0xE9,0xD1,0x4C,0x43,0x57,0xAF,0xA2,0x27,0x57,0xF1,0x58,0xB5,0xD2,0x91,0x82,0xCC,0x73,0xF1,0x4A,0x46,0x76,0xB4,0xA8,0x25,0x2B,0x1E,0xCD,0x50,0xBD,0x66,0xAF,0x78,0x24,0x26,0xF5,0x6E,0xBC,0x63,0x02,0x14,0x18,0xD6,0xFA,0x63,0xD3,0xD4,0x39,0xE6,0xAA,0xBB,0x57,0x36,0xA9,0xC5,0xA7,0x6D,0x5E,0xD9,0xAD,0x97,0x9C,0xA1,0xB9,0x60,0x8B,0x58,0x7D,0xC6,0xEE,0x92,0x2D,0x7C,0xCB,0x99,0xBB,0x29,0xD6,0xF2,0xCD,0x77,0x19,0xEB,0xD4,0x25,0x5C,0xB6,0x79,0x6C,0x3D,0x26,0x69,0xF8,0xAA,0xB4,0x57,0x63,0x14,0x93,0x0B,0xD3,0xD9,0x93,0x92,0x45,0xAE,0x49,0xF7,0xB0,0x81,0x32,0xB9,0x38,0xDD,0xCD,0x28,0xE6,0xD4,0xA2,0xF4,0x34,0x69,0xD8,0x5B,0x8B,0xC2,0xDB,0xAB,0x90,0x7B,0xAD,0x36,0x6F,0xEF,0x22,0xA6,0xD5,0xF8,0xFF,0x01};
const uint8_t spWARNING[] PROGMEM = {0x6A,0x15,0x71,0x5C,0xDC,0x23,0x89,0x8D,0xD5,0x36,0xB5,0x8C,0x2D,0x36,0xDA,0xC6,0xCC,0xDD,0x91,0x5A,0xB9,0xBF,0x51,0xC7,0xCC,0x69,0x76,0x6A,0xA6,0x12,0xD2,0xB6,0xCE,0xDB,0x6B,0x0F,0xCC,0xBA,0xAA,0x10,0x2E,0xBC,0x29,0xED,0x2A,0xA2,0x5B,0xF3,0xA5,0x2C,0x2B,0x4F,0xA9,0x25,0x9A,0xBC,0xAE,0xBC,0x74,0xD3,0xF0,0x70,0xD6,0x8A,0xE4,0xC4,0x8A,0x3D,0xEE,0x6A,0x9B,0x57,0x35,0x8F,0xD4,0x6B,0xE8,0x3E,0xD8,0x22,0x16,0xAE,0x71,0xF8,0x20,0x4D,0x5F,0xB4,0xE6,0xE1,0x83,0xB4,0x7C,0xD6,0x58,0x46,0x0A,0x94,0xF1,0x99,0x73,0x09,0x47,0xAF,0x28,0x9D,0x16,0x34,0x1C,0xD5,0x61,0xA5,0xAA,0x13,0x73,0x65,0x43,0x15,0xE2,0x71,0xD2,0x95,0x0D,0x67,0x72,0xD8,0x4E,0x77,0xB1,0x5C,0xCE,0xEE,0xA4,0x3C,0x2D,0xBA,0xA4,0x5A,0xE5,0xF2,0x74,0x57,0x6C,0xA9,0xAB,0xFE,0x1F};
const uint8_t spEMERGENCY[] PROGMEM = {0xA3,0x5B,0xCE,0x18,0x23,0x9F,0xAC,0x76,0x79,0x13,0x88,0x7C,0xB2,0xAA,0x19,0x8C,0x21,0x72,0xF5,0xAA,0x5A,0x50,0xD2,0xB0,0xD5,0xA9,0x76,0x52,0xB2,0x53,0xAD,0x95,0x26,0x72,0xD5,0x1A,0x8D,0xD6,0xDA,0x24,0xC5,0x22,0xC2,0x5E,0xAB,0x93,0xD6,0xB0,0x54,0xA5,0xAB,0x88,0x31,0xCD,0x93,0x92,0xAD,0x3C,0xDB,0x56,0x59,0xF6,0xB6,0x92,0x5C,0x83,0x6C,0x30,0xEB,0x0A,0x4A,0x5D,0xC6,0x45,0xAF,0x23,0xA8,0x53,0xC8,0xC2,0xF2,0xAA,0x24,0x3B,0x12,0xD3,0x6E,0xED,0xF2,0xB3,0x0C,0x99,0xDD,0x4B,0x00,0xB6,0x13,0x3A,0xE5,0xD4,0xA6,0xAC,0x91,0xE7,0x54,0xC3,0xA4,0xA1,0xE9,0xB6,0xD3,0x4E,0x19,0x46,0xA1,0xFD,0x46,0x3B,0xB9,0x8A,0x8E,0xB4,0x29,0x43,0x15,0xC2,0x93,0x1C,0xBD,0x8C,0x45,0xB0,0x8D,0x71,0xBD,0x36,0x6D,0xA6,0xA6,0xA2,0x69,0x0D,0xF0,0x8C,0xAB,0x02,0x7E,0x4A,0x31,0xC0,0x6F,0xA5,0x16,0x18,0x55,0x37,0x2E,0xEA,0xFE,0x65,0x0C,0x43,0xBB,0x50,0xFA,0x96,0x36,0x4E,0x93,0x82,0xE5,0x7B,0xDB,0x32,0x94,0x33,0x6E,0xFC,0x19,0xDB,0x74,0xC1,0x58,0xB1,0xA6,0xEC,0xCB,0x17,0x43,0xFA,0x9A,0x70,0x0C,0x6F,0x60,0x55,0xAB,0xFF,0x1F};
const uint8_t spEVACUATION[] PROGMEM = {0xA5,0xEF,0x4C,0x45,0xB6,0x16,0x95,0x71,0x6A,0x67,0xEC,0x5C,0xDD,0xC6,0x1E,0x9D,0xA8,0x6C,0x4B,0x58,0xAB,0x30,0x95,0x52,0xAF,0xE6,0x48,0x5C,0xD5,0xC3,0x93,0xB8,0xA3,0x6B,0x57,0x66,0x75,0xBA,0x86,0xEA,0x42,0xCD,0xB4,0xCD,0xEA,0x9A,0x2E,0xF1,0xB0,0xCD,0xAB,0xAB,0xAE,0x34,0x3D,0x56,0xAF,0xA6,0x86,0xD4,0xB2,0x5A,0xB4,0xAA,0x9A,0xCC,0xD2,0xAB,0xF6,0x2C,0x10,0x38,0x45,0xA9,0x0A,0xD5,0x00,0x03,0xAC,0xE8,0x6A,0x80,0x93,0x47,0x0C,0xB0,0x53,0x4A,0x2B,0x7A,0x74,0xE0,0xB4,0x35,0xAD,0x2A,0x34,0xC4,0x86,0xB7,0xA4,0xDA,0xF3,0x0D,0x09,0xDC,0x5C,0xDA,0xC0,0xAF,0x38,0xB8,0xF3,0xAA,0x93,0xD8,0xD0,0xA0,0xDE,0xAB,0x2A,0xB2,0xC5,0x92,0x37,0xAF,0xB2,0xF9,0x22,0x73,0xD9,0xBC,0x8A,0x1E,0x0D,0xDD,0x7D,0x75,0xA8,0x5A,0x20,0xF6,0xCA,0x45,0x0C,0x38,0x71,0x54,0x01,0xBB,0x6D,0x19,0x60,0x97,0xEB,0x00,0xEC,0xB2,0x16,0x80,0x95,0x2F,0xD6,0xD4,0x54,0xA8,0x94,0x77,0x6E,0x73,0xD3,0xA1,0x1E,0xB2,0xBA,0xAD,0xDD,0xB1,0x94,0x8B,0xB5,0xB1,0x77,0xCB,0x52,0xAE,0xD6,0xCA,0x31,0x9C,0x48,0xA8,0x45,0x4D,0xE7,0x70,0x46,0x61,0x12,0x2D,0x5D,0x53,0x3B,0x99,0x73,0xB4,0x74,0x0F,0xED,0x92,0xAA,0x51,0xD3,0xD3,0x5C,0xBA,0xAB,0xA5,0xF9,0x7F};

const uint8_t spPAPA[] PROGMEM = {0x0A,0x28,0x56,0xB9,0xA5,0x45,0x55,0x84,0x49,0xCC,0x93,0x66,0xD7,0x19,0x26,0x4B,0x4E,0x96,0xDD,0x44,0xBA,0xAE,0xBE,0xD9,0xCC,0x10,0x28,0x42,0xB9,0x8B,0xC8,0x06,0x60,0x80,0xF1,0xE9,0xAB,0xCA,0xA6,0x23,0xD4,0x36,0xDF,0xE1,0x8C,0x55,0x74,0x86,0x6B,0x9F,0xB1,0x67,0xBD,0xE1,0xE6,0xBB,0xDB,0x97,0x53,0x45,0x88,0xCF,0xAE,0xDF,0xFF,0x03};
const uint8_t spCHARLIE[] PROGMEM = {0x06,0xD8,0x2D,0x2C,0x01,0x33,0xB7,0x67,0xA0,0xC5,0xC5,0x8D,0xC8,0xB2,0xB7,0x96,0x17,0xBF,0x26,0x87,0xF9,0x5A,0x91,0xF3,0xB0,0x1F,0x75,0x19,0x45,0xCE,0xCD,0x71,0xD4,0x65,0x54,0x39,0x8E,0xDA,0x71,0xB6,0xD1,0xC5,0x30,0xE9,0x6E,0x69,0xCB,0x5C,0x51,0x8F,0x15,0x66,0x49,0xFB,0x80,0x31,0xA2,0xE6,0x7D,0x9D,0x55,0xB6,0x59,0xD8,0xEA,0x71,0x0F,0xDE,0xCA,0xE5,0x9B,0xC7,0xD3,0x65,0x92,0x55,0x6C,0x2E,0xEF,0x30,0x49,0x92,0xB1,0x3A,0xBD,0xC3,0x04,0x72,0xE7,0xE6,0xF0,0x75,0xEB,0xA8,0x93,0xAB,0xFF,0x1F};
const uint8_t spOSCAR[] PROGMEM = {0x6B,0xC8,0xE2,0xB2,0x42,0x3A,0xDF,0xFA,0x16,0x27,0x4F,0xAE,0x7D,0xC4,0x17,0xB7,0x2C,0x45,0xAF,0xA4,0xB6,0x6D,0x80,0x03,0xD8,0x0C,0xF0,0xA7,0x9B,0x07,0x3C,0xE0,0x80,0xEB,0xB5,0x11,0x38,0x45,0xA9,0x0A,0xD5,0x00,0x02,0x34,0x75,0x65,0xB6,0xA6,0xAE,0xA2,0x34,0x6E,0xEA,0x8B,0x9B,0x24,0x13,0xA5,0xAD,0xCD,0xA9,0xC8,0x86,0xF3,0xCE,0x7E,0x8E,0x6D,0xC9,0xCD,0xB0,0x5A,0x7A,0xCF,0xAD,0xEE,0xF5,0x4C,0x77,0x89,0x83,0x3A,0xA9,0x37,0x7F,0xE1,0x2F,0xAD,0x48,0x87,0xB3,0xFC,0x3F};
const uint8_t spJULIET[] PROGMEM = {0x61,0x5D,0x96,0x49,0x34,0xD2,0x06,0x60,0xC7,0x90,0x0C,0x8C,0x66,0xF6,0x15,0x22,0x4D,0x37,0xAA,0x6A,0xC8,0x2C,0x6D,0xCD,0x28,0xB2,0x15,0x8B,0xE4,0x35,0xB3,0x68,0x79,0x51,0xE6,0xDA,0x9C,0xBE,0x15,0x43,0x89,0xF0,0xA2,0xDB,0x95,0x77,0xA7,0xA6,0x66,0x49,0x77,0xB1,0x9A,0x9E,0x0A,0xD5,0x75,0xEB,0xEE,0xF6,0xB0,0xC6,0xE6,0x83,0xD2,0xE3,0xEB,0x5E,0xD7,0xDA,0x5C,0x48,0x87,0x6D,0x9E,0x7B,0xDF,0xF3,0x89,0x40,0x11,0xCA,0x5D,0x44,0x36,0x00,0x38,0x60,0xEA,0x8C,0x00,0x2C,0xB3,0x6D,0x01,0x01,0x14,0x5F,0x8E,0x81,0xFF,0x07};
const uint8_t spIS[] PROGMEM = {0xA3,0xED,0xC6,0x30,0x3D,0x57,0xAD,0x7E,0xA8,0x42,0xA9,0x5C,0xB5,0xFA,0xA9,0x8A,0xB8,0x62,0xF3,0xEA,0x86,0x48,0xE6,0x8A,0x57,0xAB,0xEB,0x22,0x58,0x23,0x5E,0xAF,0xAE,0xCA,0x64,0xF5,0x7C,0x3C,0xBA,0xCA,0x93,0xD5,0xE3,0x76,0xEB,0x3B,0x4E,0x55,0xB3,0x4D,0x65,0xB8,0x58,0x5D,0xDD,0x72,0x97,0xE9,0x1B,0x55,0x27,0x4D,0xD3,0xE6,0x85,0xD5,0x4D,0x3D,0x6B,0xF9,0x5F,0x50,0x1B,0x26,0x27,0x0A,0xF8,0xAD,0x54,0x01,0xBF,0xBA,0x0B,0xE0,0xA7,0xF4,0xFF,0x07};
const uint8_t spUP[] PROGMEM = {0x69,0x2C,0x6A,0x32,0xCC,0x97,0xAC,0xA1,0xC8,0x4D,0x37,0x5F,0xB3,0xFA,0xA2,0x36,0x42,0x63,0xC9,0x6A,0x93,0x9B,0x08,0xCD,0x25,0xAB,0xCD,0x76,0x32,0x25,0x96,0x94,0x21,0x96,0x4A,0x97,0xB9,0xE3,0xA6,0xD0,0x3D,0x2C,0xF6,0x0A,0x02,0xA7,0x28,0x55,0xA1,0x1A,0x00,0x00,0x04,0x70,0x2E,0x44,0x00,0x39,0xB9,0x23,0x20,0x35,0xD7,0xFF,0x07};

//Stores last time we checked buttons
uint32_t lastTouchMillis = 0;
boolean leftToggle = false;
boolean rightToggle = false;
//Stores info on alarm
boolean alarmTriggered = false;

// *********************************************
// SETUP
// *********************************************
void setup() {
    CircuitPlayground.begin();
    CircuitPlayground.redLED(HIGH);

    Serial.begin(115200);
    Serial.println();
    Serial.println("CircuitPlaygroundExpress is Alive!");

    // Read the sound sensor and use it to initialize the random number generator.
    randomSeed(CircuitPlayground.soundSensor());
    // Precompute random frequency and phase values for each pixel.
    // This gives the flicker an organic but random looking appearance.
    for (int i=0; i<10; ++i) {
        // Generate random floating point frequency values between 1.0 and 4.0.
        frequencies[i] = random(1000, 4000)/1000.0;
        // Generate random floating point phase values between 0 and 2*PI.
        phases[i] = random(1000)/1000.0 * 2.0 * PI;
    }

    CircuitPlayground.redLED(LOW);
}

// *********************************************
// MAIN LOOP LOGIC
// *********************************************
void loop() {
    uint32_t millisNow = millis();
    //checkButtons(millisNow);
    checkSerial();

    //Check for left button press and enable/disable light
    if (alarmTriggered) {
        showLitFlicker(millisNow);
    } else {
        CircuitPlayground.strip.clear();
        CircuitPlayground.strip.show();
    }

    //Check for right button press
    if (rightToggle) {
        //showLitFlicker(millisNow);
    } else {
        //CircuitPlayground.strip.clear();
        //CircuitPlayground.strip.show();
    }
}

void checkSerial() {
  if (Serial.available()) {
    char inChar = Serial.read();
    if (inChar == 'a' || inChar == 'A') {
      triggerAlarm();
    } else if (inChar == 'd' || inChar == 'D') {
      deactivateAlarm();
    }
    //Clear buffer
    while (Serial.available()) {
      Serial.read();
    }
  }
}

void triggerAlarm() {
  /*CircuitPlayground.speaker.say(spEMERGENCY);
  delay(25);
  CircuitPlayground.speaker.say(spJULIET);
  delay(25);
  CircuitPlayground.speaker.say(spIS);
  delay(25);
  CircuitPlayground.speaker.say(spUP);
  delay(25);
  CircuitPlayground.speaker.say(spEMERGENCY);*/
  alarmTriggered = true;
}

void deactivateAlarm() {
  alarmTriggered = false;
}

//Check button presses
void checkButtons(uint32_t millisNow) {
    if (millisNow - lastTouchMillis > 250) {
        lastTouchMillis = millisNow;
        
        if (CircuitPlayground.leftButton()) {
            Serial.println("Left button pressed!");
            leftToggle = !leftToggle;
        }
        if (CircuitPlayground.rightButton()) {
            Serial.println("Right button pressed!");

            rightToggle = !rightToggle;
        }
    } 
}

// *********************************************
// CANDLE EFFECT FUNCTIONS
// *********************************************
// Helper to change the color of a NeoPixel on the Circuit Playground board.
// Will automatically convert from HSV color space to RGB and apply gamma
// correction.
void setPixelHSV(int i, float h, float s, float v) {
  // Convert HSV to RGB
  float r, g, b = 0.0;
  HSVtoRGB(&r, &g, &b, h, s, v);
  // Lookup gamma correct RGB colors (also convert from 0...1.0 RGB range to 0...255 byte range).
  uint8_t r1 = pgm_read_byte(&gamma8[int(r*255.0)]);
  uint8_t g1 = pgm_read_byte(&gamma8[int(g*255.0)]);
  uint8_t b1 = pgm_read_byte(&gamma8[int(b*255.0)]);
  // Set the color of the pixel.
  CircuitPlayground.strip.setPixelColor(i, r1, g1, b1);
}

// Flickering candle animation effect.
// Uses a noisey sine wave to modulate the hue of each pixel within a range
// of flame colors.  The sine wave has some low and high frequency components
// which give it an organice and seemingly random appearance.
void showLitFlicker(uint32_t current) {
    // First determine the low and high bounds of the flicker hues.
    // These are +/- 10 degrees of the specified target hue and will
    // wrap around to the start/end as appropriate.
    float lowHue = fmod(FLAMEHUE - 10.0, 360.0);
    float highHue = fmod(FLAMEHUE + 10.0, 360.0);
    // Convert time from milliseconds to seconds.
    float t = current/1000.0;
    // Loop through each pixel and compute its color.
    for (int i=0; i<10; ++i) {
        // This pixel should be lit, so compute its hue by composing
        // a low frequency / slowly changing sine wave with a high
        // frequency / fast changing cosine wave.  This means the candle will
        // pulse and jump around in an organice but random looking way.
        // The frequencies and phases of the waves are randomly generated at
        // startup in the setup function.
        // Low frequency wave is a sine wave with random freuqency between 1 and 4,
        // and offset by a random phase to keep pixels from all starting at the same
        // color:
        float lowFreq  = sin(2.0*PI*frequencies[i]*t + phases[i]);
        // High frequency is a faster changing cosine wave that uses a different
        // pixel's random frequency.
        float highFreq = cos(3.0*PI*frequencies[(i+5)%10]*t);
        // Add the low and high frequency waves together, then interpolate their value
        // to a hue that's +/-20% of the configured target hue.
        float h = lerp(lowFreq+highFreq, -2.0, 2.0, lowHue, highHue);
        setPixelHSV(i, h, 1.0, 1.0);
    }
    CircuitPlayground.strip.show();
}

// HSV to RGB color space conversion function taken directly from:
//  https://www.cs.rit.edu/~ncs/color/t_convert.html
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v ) {
  int i;
  float f, p, q, t;
  if( s == 0 ) {
    // achromatic (grey)
    *r = *g = *b = v;
    return;
  }
  h /= 60;      // sector 0 to 5
  i = floor( h );
  f = h - i;      // factorial part of h
  p = v * ( 1 - s );
  q = v * ( 1 - s * f );
  t = v * ( 1 - s * ( 1 - f ) );
  switch( i ) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    default:    // case 5:
      *r = v;
      *g = p;
      *b = q;
      break;
  }
}

// Linear interpolation of value y within y0...y1 given x and x0...x1.
float lerp(float x, float x0, float x1, float y0, float y1) {
  return y0 + (x - x0) * ((y1 - y0) / (x1 - x0));
}

  /************* TEST CAPTOUCH */
  // Serial.print("Capsense #3: "); Serial.println(CircuitPlayground.readCap(3));
  // Serial.print("Capsense #2: "); Serial.println(CircuitPlayground.readCap(2));
  // if (! CircuitPlayground.isExpress()) {  // CPX does not have this captouch pin
  //   Serial.print("Capsense #0: "); Serial.println(CircuitPlayground.readCap(0));
  // }
  // Serial.print("Capsense #1: "); Serial.println(CircuitPlayground.readCap(1));
  // Serial.print("Capsense #12: "); Serial.println(CircuitPlayground.readCap(12));
  // Serial.print("Capsense #6: "); Serial.println(CircuitPlayground.readCap(6));
  // Serial.print("Capsense #9: "); Serial.println(CircuitPlayground.readCap(9));
  // Serial.print("Capsense #10: "); Serial.println(CircuitPlayground.readCap(10));
  
  /************* TEST SLIDE SWITCH */
  // if (CircuitPlayground.slideSwitch()) {
  //   Serial.println("Slide to the left");
  // } else {
  //   Serial.println("Slide to the right");
  //   //CircuitPlayground.playTone(500 + pixeln * 500, 100);
  // }

  // /************* TEST 10 NEOPIXELS */
  // CircuitPlayground.setPixelColor(pixeln++, CircuitPlayground.colorWheel(25 * pixeln));
  // if (pixeln == 11) {
  //   pixeln = 0;
  //   CircuitPlayground.clearPixels();
  // }

  // /************* TEST BOTH BUTTONS */
  // if (CircuitPlayground.leftButton()) {
  //   Serial.println("Left button pressed!");
  // }
  // if (CircuitPlayground.rightButton()) {
  //   Serial.println("Right button pressed!");
  // }

  // /************* TEST LIGHT SENSOR */
  // Serial.print("Light sensor: ");
  // Serial.println(CircuitPlayground.lightSensor());

  // /************* TEST SOUND SENSOR */
  // Serial.print("Sound sensor: ");
  // Serial.println(CircuitPlayground.mic.soundPressureLevel(10));

  // /************* TEST ACCEL */
  // // Display the results (acceleration is measured in m/s*s)
  // Serial.print("X: "); Serial.print(CircuitPlayground.motionX());
  // Serial.print(" \tY: "); Serial.print(CircuitPlayground.motionY());
  // Serial.print(" \tZ: "); Serial.print(CircuitPlayground.motionZ());
  // Serial.println(" m/s^2");

  // /************* TEST THERMISTOR */
  // Serial.print("Temperature ");
  // Serial.print(CircuitPlayground.temperature());
  // Serial.println(" *C");
