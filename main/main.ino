#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define DEMO

const int pinBtnSelect = 9;
const int pinBtnLeft = 8;
const int pinBtnRight = 10;
const int pinAnalogIn = A0;
#ifdef DEMO
const int pinPwm25Out = 3;
#endif
const int bufferSize = 128;
const int minMode = 1;
const int maxMode = 7 + 500;
const int defaultMode = 4; // DIV16

Adafruit_SSD1306 display(-1);
byte buffer[bufferSize];
int mode = defaultMode;
//bool lastClkValue;

void drawScreen(unsigned long duration) {
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  for (int v = 5; v >= 0; v --) {
    int tickY;
    if (v == 5)
      tickY = 0;
    else if (! v)
      tickY = display.height() - 1;
    else
      tickY = display.height() - display.height() * v / 5;
    for (int x = 0; x < display.width(); x += 8)
      display.drawPixel(x, tickY, WHITE);
  }
  for (int x = 0; x < bufferSize; x ++) {
    if (! x)
      display.drawPixel(x, buffer[x], WHITE);
    else
      display.drawLine(x - 1, buffer[x - 1], x, buffer[x], WHITE);
  }
  display.setCursor(0, 0);
  display.print("5V");
  display.setCursor(0, display.height() - 7);
  display.print(duration);
  display.print(" us");
  display.display();
}

void setup() {
  pinMode(pinBtnSelect, INPUT_PULLUP);
  pinMode(pinBtnLeft, INPUT_PULLUP);
  pinMode(pinBtnRight, INPUT_PULLUP);
  
  pinMode(pinAnalogIn, INPUT);
#ifdef SELFDEMO
  pinMode(pinPwm25Out, OUTPUT);
  analogWrite(pinPwm25Out, 63);
#endif
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D, false);
  display.setTextSize(1);
  display.clearDisplay();
}

void loop() {
  if (digitalRead(pinBtnSelect) == HIGH)
    mode = defaultMode;
  if (mode > minMode && digitalRead(pinBtnLeft) == HIGH)
      mode --;
  if (mode < maxMode && digitalRead(pinBtnRight) == HIGH)
      mode ++;

  if (mode < 7) {
    if (mode & 1)
      sbi(ADCSRA, ADPS0);
    else
      cbi(ADCSRA, ADPS0);
    if (mode & 2)
      sbi(ADCSRA, ADPS1);
    else
      cbi(ADCSRA, ADPS1);
    if (mode & 4)
      sbi(ADCSRA, ADPS2);
    else
      cbi(ADCSRA, ADPS2);
  } else {
    sbi(ADCSRA, ADPS0);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS2);
  }
  unsigned long delayUs = (mode > 7) ? (mode - 7) * 20 : 0;
  unsigned long start = micros();
  for (int x = 0; x < bufferSize; x ++) {
    buffer[x] = map(analogRead(pinAnalogIn), 0, 1023, display.height() - 1, 0);
    if (delayUs)
      delayMicroseconds(delayUs);
  }
  drawScreen(micros() - start);
}
