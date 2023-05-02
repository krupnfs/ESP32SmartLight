#include "Arduino.h"
#include "led_draw.h"
#include "colors.h"
#include "Adafruit_NeoPixel.h"

uint8_t calculateBrightness(uint8_t brightness)
{
  return brightness * 32 - 1;
}

byte prevDrbBrightness = -1;
byte prevDrbTemperature = -1;

void drawBrightness(uint8_t temperature, uint8_t brightness, Adafruit_NeoPixel &volumeStrip)
{
  if(prevDrbBrightness == brightness && prevDrbTemperature == temperature)
    return;
  Serial.println("UpdateBrightness");
  prevDrbBrightness = brightness;
  prevDrbTemperature = temperature;
  clearLedStrip(volumeStrip);
  uint32_t initTemperature = 0;
  switch (temperature)
  {
    case 0:
      initTemperature = Candle;
      break;
    case 1:
      initTemperature = Tungsten40W;
      break;
    case 2:
      initTemperature = Tungsten100W;
      break;
    case 3:
      initTemperature = Halogen;
      break;
    case 4:
      initTemperature = CarbonArc;
      break;
    case 5:
      initTemperature = HighNoonSun;
      break;
    case 6:
      initTemperature = DirectSunlight;
      break;
    case 7:
      initTemperature = OvercastSky;
      break;
    default:
      break;
  }
  for (byte i = 0; i < brightness; i++)
  {
    volumeStrip.setPixelColor(i, initTemperature);
    volumeStrip.setBrightness(calculateBrightness(brightness));
  }
  volumeStrip.show();
}

byte prevDrtBrightness = -1;
byte prevDrtTemperature = -1;

void drawTemperature(uint8_t temperature, uint8_t brightness, Adafruit_NeoPixel &volumeStrip)
{
  if(prevDrtBrightness == brightness && prevDrtTemperature == temperature)
    return;
  Serial.println("UpdateTemperature");
  prevDrtBrightness = brightness;
  prevDrtTemperature = temperature;
  clearLedStrip(volumeStrip);
  if(temperature != 0)
    volumeStrip.setPixelColor(0, Candle);
  if(temperature != 1)
    volumeStrip.setPixelColor(1, Tungsten40W);
  if(temperature != 2)
    volumeStrip.setPixelColor(2, Tungsten100W);
  if(temperature != 3)
    volumeStrip.setPixelColor(3, Halogen);
  if(temperature != 4)
    volumeStrip.setPixelColor(4, CarbonArc);
  if(temperature != 5)
    volumeStrip.setPixelColor(5, HighNoonSun);
  if(temperature != 6)
    volumeStrip.setPixelColor(6, DirectSunlight);
  if(temperature != 7)
    volumeStrip.setPixelColor(7, OvercastSky);
  volumeStrip.setBrightness(calculateBrightness(brightness));
  volumeStrip.show();
}

void drawNightLight(uint8_t temperature, uint8_t brightness, Adafruit_NeoPixel &mainStrip)
{
  switch (temperature)
  {
    case 0:
      mainStrip.fill(Candle);
      break;
    case 1:
      mainStrip.fill(Tungsten40W);
      break;
    case 2:
      mainStrip.fill(Tungsten100W);
      break;
    case 3:
      mainStrip.fill(Halogen);
      break;
    case 4:
      mainStrip.fill(CarbonArc);
      break;
    case 5:
      mainStrip.fill(HighNoonSun);
      break;
    case 6:
      mainStrip.fill(DirectSunlight);
      break;
    case 7:
      mainStrip.fill(OvercastSky);
      break;
    default:
      break;
  }
  mainStrip.setBrightness(calculateBrightness(brightness));
  mainStrip.show();
}

void drawRainbow(uint8_t brightness, uint16_t counter, Adafruit_NeoPixel &mainStrip)
{
  mainStrip.rainbow(counter, 1, 255, calculateBrightness(brightness), true);
  mainStrip.setBrightness(calculateBrightness(brightness));
  mainStrip.show();
  delay(5);
}

void clearLedStrips(Adafruit_NeoPixel &mainStrip, Adafruit_NeoPixel &volumeStrip)
{
    clearLedStrip(mainStrip);
    clearLedStrip(volumeStrip);
}

void clearLedStrip(Adafruit_NeoPixel &strip)
{
    strip.clear();
    strip.show();
}