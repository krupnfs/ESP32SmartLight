#include "Arduino.h"
#include "Adafruit_NeoPixel.h"

void drawBrightness(uint8_t temperature, uint8_t brightness, Adafruit_NeoPixel &volumeStrip);
void drawTemperature(uint8_t temperature, uint8_t brightness, Adafruit_NeoPixel &volumeStrip);
void drawNightLight(uint8_t temperature, uint8_t brightness, Adafruit_NeoPixel &mainStrip);
void drawRainbow(uint8_t brightness, uint16_t counter, Adafruit_NeoPixel &mainStrip);
void clearLedStrips(Adafruit_NeoPixel &mainStrip, Adafruit_NeoPixel &volumeStrip);
void clearLedStrip(Adafruit_NeoPixel &strip);