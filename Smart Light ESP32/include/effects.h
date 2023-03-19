#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "PubSubClient.h"
//#include "variables.h"

extern bool needBreakEffect;
extern PubSubClient mqttClient;

void set_color_led(Adafruit_NeoPixel &mainStrip, int adex, int cred, int cgrn, int cblu);
int horizontal_index(int i);
int antipodal_index(int i);
int adjacent_cw(int i);
int adjacent_ccw(int i);
void copy_led_array(Adafruit_NeoPixel &mainStrip);
void setPixel(Adafruit_NeoPixel &mainStrip, int Pixel, byte red, byte green, byte blue);
void setAll(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue);
uint16_t convert_8bit_hue_to_16bit(uint8_t hue);
boolean safeDelay(int delTime);


void setPixelHeatColor (Adafruit_NeoPixel &mainStrip, int Pixel, byte temperature);
byte * Wheel(byte WheelPos);

#pragma region Effects
void one_color_allHSV(Adafruit_NeoPixel &mainStrip, int ahue);
void rainbow_fade(Adafruit_NeoPixel &mainStrip);
void rainbow_loop(Adafruit_NeoPixel &mainStrip);
void random_burst(Adafruit_NeoPixel &mainStrip);
void color_bounce(Adafruit_NeoPixel &mainStrip);
void color_bounceFADE(Adafruit_NeoPixel &mainStrip);
void ems_lightsONE(Adafruit_NeoPixel &mainStrip);
void ems_lightsALL(Adafruit_NeoPixel &mainStrip);
void flicker(Adafruit_NeoPixel &mainStrip);
void pulse_one_color_all(Adafruit_NeoPixel &mainStrip);
void pulse_one_color_all_rev(Adafruit_NeoPixel &mainStrip);
void fade_vertical(Adafruit_NeoPixel &mainStrip);
void random_red(Adafruit_NeoPixel &mainStrip);
void rule30(Adafruit_NeoPixel &mainStrip);
void random_march(Adafruit_NeoPixel &mainStrip);
void rwb_march(Adafruit_NeoPixel &mainStrip);
void radiation(Adafruit_NeoPixel &mainStrip);
void color_loop_vardelay(Adafruit_NeoPixel &mainStrip);
void white_temps(Adafruit_NeoPixel &mainStrip);
void sin_bright_wave(Adafruit_NeoPixel &mainStrip);
void pop_horizontal(Adafruit_NeoPixel &mainStrip);
void quad_bright_curve(Adafruit_NeoPixel &mainStrip);
void flame(Adafruit_NeoPixel &mainStrip);
void rainbow_vertical(Adafruit_NeoPixel &mainStrip);
void pacman(Adafruit_NeoPixel &mainStrip);
void random_color_pop(Adafruit_NeoPixel &mainStrip);
void ems_lightsSTROBE(Adafruit_NeoPixel &mainStrip);
void rgb_propeller(Adafruit_NeoPixel &mainStrip);
void kitt(Adafruit_NeoPixel &mainStrip);
void matrix(Adafruit_NeoPixel &mainStrip);
void strip_march_cw(Adafruit_NeoPixel &mainStrip);
void strip_march_ccw(Adafruit_NeoPixel &mainStrip);

void demo_modeA(Adafruit_NeoPixel &mainStrip);
void demo_modeB(Adafruit_NeoPixel &mainStrip);

void show_fx(Adafruit_NeoPixel &mainStrip, uint8_t effectId);
#pragma endregion