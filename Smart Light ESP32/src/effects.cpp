#include "configuration.h"
//#include <variables.h>
#include "effects.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "WiFi.h"
#include "PubSubClient.h"


// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;        // светодиод начала отсчёта
int TOP_INDEX = int(NUM_MAIN_LEDS / 2);
int EVENODD = NUM_MAIN_LEDS % 2;
//struct CRGB leds[NUM_MAIN_LEDS];
int ledsX[NUM_MAIN_LEDS][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;          //-FX LOOPS DELAY VAR
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;                //-HUE (0-255)
int ibright = 0;             //-BRIGHTNESS (0-255)
int isat = 0;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR

volatile uint32_t btnTimer;
volatile byte modeCounter;
volatile boolean changeFlag;

bool needBreakEffect = false;
WiFiClient espClient;
PubSubClient mqttClient(espClient);


// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------

#pragma region utility




//------------------------------------- UTILITY FXNS --------------------------------------
uint16_t convert_8bit_hue_to_16bit(uint8_t hue)
{
  float _16bitValue = ((float)hue / ((float)255 / 100)) * ((float)65535 / 100);
  return _16bitValue;
}

//---SET THE COLOR OF A SINGLE RGB LED
void set_color_led(Adafruit_NeoPixel &mainStrip, int adex, int cred, int cgrn, int cblu) {
  //leds[adex].setRGB(cred, cgrn, cblu);
  mainStrip.setPixelColor(adex, cred, cgrn, cblu);
}

//---FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {
    return BOTTOM_INDEX;
  }
  if (i == TOP_INDEX && EVENODD == 1) {
    return TOP_INDEX + 1;
  }
  if (i == TOP_INDEX && EVENODD == 0) {
    return TOP_INDEX;
  }
  return NUM_MAIN_LEDS - i;
}

//---FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {
    iN = ( i + TOP_INDEX ) % NUM_MAIN_LEDS;
  }
  return iN;
}

//---FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < NUM_MAIN_LEDS - 1) {
    r = i + 1;
  }
  else {
    r = 0;
  }
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {
    r = i - 1;
  }
  else {
    r = NUM_MAIN_LEDS - 1;
  }
  return r;
}

// Returns the Red component of a 32-bit color
uint8_t NeoPixelColorRed(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t NeoPixelColorGreen(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t NeoPixelColorBlue(uint32_t color)
{
    return color & 0xFF;
}


void copy_led_array(Adafruit_NeoPixel &mainStrip) {
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    uint32_t color = mainStrip.getPixelColor(i);
    ledsX[i][0] = NeoPixelColorRed(color);//leds[i].r;
    ledsX[i][1] = NeoPixelColorGreen(color);//leds[i].g;
    ledsX[i][2] = NeoPixelColorBlue(color);//leds[i].b;
  }
}

void setPixel(Adafruit_NeoPixel &mainStrip, int Pixel, byte red, byte green, byte blue) {
  mainStrip.setPixelColor(Pixel, red, green, blue);
  // leds[Pixel].r = red;
  // leds[Pixel].g = green;
  // leds[Pixel].b = blue;
}

void setAll(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    setPixel(mainStrip, i, red, green, blue);
  }
  mainStrip.show();
}

boolean safeDelay(int delTime) {
  uint32_t thisTime = millis();
  while (millis() - thisTime <= delTime) {
    if (changeFlag) {
      changeFlag = false;
      return true;
    }
  }
  return false;
}
#pragma endregion


//------------------------LED EFFECT FUNCTIONS------------------------
void one_color_allHSV(Adafruit_NeoPixel &mainStrip, int ahue) {    //-SET ALL LEDS TO ONE COLOR (HSV)
  for (int i = 0 ; i < NUM_MAIN_LEDS; i++ ) {
    mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ahue), thissat, 255));
    //leds[i] = CHSV(ahue, thissat, 255);
  }
}

void rainbow_fade(Adafruit_NeoPixel &mainStrip) {                         //-m2-FADE ALL LEDS THROUGH HSV RAINBOW
  ihue++;
  if (ihue > 255) {
    ihue = 0;
  }
  uint32_t chsv = Adafruit_NeoPixel::ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255);
  for (int idex = 0; idex < NUM_MAIN_LEDS; idex++ ) {
    mainStrip.setPixelColor(idex, chsv);//mainStrip.ColorHSV(ihue, thissat, 255));
  }
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void rainbow_loop(Adafruit_NeoPixel &mainStrip) {                        //-m3-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + thisstep;
  if (idex >= NUM_MAIN_LEDS) {
    idex = 0;
  }
  if (ihue > 255) {
    ihue = 0;
  }
  // leds[idex] = CHSV(ihue, thissat, 255);
  // LEDS.show();
  mainStrip.setPixelColor(idex, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void random_burst(Adafruit_NeoPixel &mainStrip) {                         //-m4-RANDOM INDEX/COLOR
  idex = random(0, NUM_MAIN_LEDS);
  ihue = random(0, 255);
  //leds[idex] = CHSV(ihue, thissat, 255);
  mainStrip.setPixelColor(idex, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

int color_bounce_hue = 0;

void color_bounce(Adafruit_NeoPixel &mainStrip) {                        //-m5-BOUNCE COLOR (SINGLE LED)
  color_bounce_hue++;
  if(color_bounce_hue > 255)
    color_bounce_hue = 0;
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_MAIN_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if (i == idex) {
      //leds[i] = CHSV(thishue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 255));
    }
    else {
      //leds[i] = CHSV(0, 0, 0);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(0), 0, 0));
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void color_bounceFADE(Adafruit_NeoPixel &mainStrip) {                    //-m6-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  color_bounce_hue++;
  if(color_bounce_hue > 255)
    color_bounce_hue = 0;
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == NUM_MAIN_LEDS) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2);
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if (i == idex) {
      //leds[i] = CHSV(thishue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 255));
    }
    else if (i == iL1) {
      //leds[i] = CHSV(thishue, thissat, 150);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 150));
    }
    else if (i == iL2) {
      //leds[i] = CHSV(thishue, thissat, 80);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 80));
    }
    else if (i == iL3) {
      //leds[i] = CHSV(thishue, thissat, 20);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 20));
    }
    else if (i == iR1) {
      //leds[i] = CHSV(thishue, thissat, 150);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 150));
    }
    else if (i == iR2) {
      //leds[i] = CHSV(thishue, thissat, 80);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 80));
    }
    else if (i == iR3) {
      //leds[i] = CHSV(thishue, thissat, 20);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, 20));
    }
    else {
      //leds[i] = CHSV(0, 0, 0);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(0), 0, 0));
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void ems_lightsONE(Adafruit_NeoPixel &mainStrip) {                    //-m7-EMERGENCY LIGHTS (TWO COLOR SINGLE LED)
  idex++;
  if (idex >= NUM_MAIN_LEDS) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  int thathue = (thishue + 160) % 255;
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if (i == idexR) {
      //leds[i] = CHSV(thishue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thathue), thissat, 255));
    }
    else if (i == idexB) {
      //leds[i] = CHSV(thathue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thathue), thissat, 255));
    }
    else {
      //leds[i] = CHSV(0, 0, 0);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(0), 0, 0));
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void ems_lightsALL(Adafruit_NeoPixel &mainStrip) {                  //-m8-EMERGENCY LIGHTS (TWO COLOR SOLID)
  idex++;
  if (idex >= NUM_MAIN_LEDS) {
    idex = 0;
  }
  int idexR = idex;
  int idexB = antipodal_index(idexR);
  int thathue = (thishue + 160) % 255;
  mainStrip.setPixelColor(idexR, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thathue), thissat, 255));
  mainStrip.setPixelColor(idexB, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thathue), thissat, 255));
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void flicker(Adafruit_NeoPixel &mainStrip) {                          //-m9-FLICKER EFFECT
  int random_bright = random(0, 255);
  int random_delay = random(10, 100);
  int random_bool = random(0, random_bright);
  if (random_bool < 10) {
    for (int i = 0 ; i < NUM_MAIN_LEDS; i++ ) {
      //leds[i] = CHSV(thishue, thissat, random_bright);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, random_bright));
    }
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(random_delay)) return;
  }
}

void pulse_one_color_all(Adafruit_NeoPixel &mainStrip) {              //-m10-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR
color_bounce_hue++;
  if(color_bounce_hue > 255)
    color_bounce_hue = 0;
  if (bouncedirection == 0) {
    ibright++;
    if (ibright >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= 1) {
      bouncedirection = 0;
    }
  }
  for (int idex = 0 ; idex < NUM_MAIN_LEDS; idex++ ) {
    //leds[idex] = CHSV(thishue, thissat, ibright);
    mainStrip.setPixelColor(idex, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), thissat, ibright));
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void pulse_one_color_all_rev(Adafruit_NeoPixel &mainStrip) {           //-m11-PULSE SATURATION ON ALL LEDS TO ONE COLOR
color_bounce_hue++;
  if(color_bounce_hue > 255)
    color_bounce_hue = 0;
  if (bouncedirection == 0) {
    isat++;
    if (isat >= 255) {
      bouncedirection = 1;
    }
  }
  if (bouncedirection == 1) {
    isat = isat - 1;
    if (isat <= 1) {
      bouncedirection = 0;
    }
  }
  for (int idex = 0 ; idex < NUM_MAIN_LEDS; idex++ ) {
    //leds[idex] = CHSV(thishue, isat, 255);
    mainStrip.setPixelColor(idex, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(color_bounce_hue), isat, 255));
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void fade_vertical(Adafruit_NeoPixel &mainStrip) {                    //-m12-FADE 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {
    idex = 0;
  }
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  ibright = ibright + 10;
  if (ibright > 255) {
    ibright = 0;
  }
  //leds[idexA] = CHSV(thishue, thissat, ibright);
  mainStrip.setPixelColor(idexA, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
  //leds[idexB] = CHSV(thishue, thissat, ibright);
  mainStrip.setPixelColor(idexB, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void random_red(Adafruit_NeoPixel &mainStrip) {                       //QUICK 'N DIRTY RANDOMIZE TO GET CELL AUTOMATA STARTED
  int temprand;
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    temprand = random(0, 100);
    if (temprand > 50) {
      //leds[i].r = 255;
      mainStrip.setPixelColor(i, 255, 0, 0);
    }
    if (temprand <= 50) {
      //leds[i].r = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
    // leds[i].b = 0; 
    // leds[i].g = 0;
  }
  //LEDS.show(); 
  mainStrip.show();
}

void rule30(Adafruit_NeoPixel &mainStrip) {                          //-m13-1D CELLULAR AUTOMATA - RULE 30 (RED FOR NOW)
  if (bouncedirection == 0) {
    random_red(mainStrip);
    bouncedirection = 1;
  }
  copy_led_array(mainStrip);
  int iCW;
  int iCCW;
  int y = 100;
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    iCW = adjacent_cw(i);
    iCCW = adjacent_ccw(i);
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] > y) {
      //leds[i].r = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
    if (ledsX[iCCW][0] > y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {
      //leds[i].r = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {
      //leds[i].r = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
    if (ledsX[iCCW][0] > y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {
      //leds[i].r = 255;
      mainStrip.setPixelColor(i, 255, 0, 0);
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] > y) {
      //leds[i].r = 255;
      mainStrip.setPixelColor(i, 255, 0, 0);
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] > y && ledsX[iCW][0] <= y) {
      //leds[i].r = 255;
      mainStrip.setPixelColor(i, 255, 0, 0);
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] > y) {
      //leds[i].r = 255;
      mainStrip.setPixelColor(i, 255, 0, 0);
    }
    if (ledsX[iCCW][0] <= y && ledsX[i][0] <= y && ledsX[iCW][0] <= y) {
      //leds[i].r = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void random_march(Adafruit_NeoPixel &mainStrip) {                   //-m14-RANDOM MARCH CCW
  copy_led_array(mainStrip);
  int iCCW;
  //leds[0] = CHSV(random(0, 255), 255, 255);
  mainStrip.setPixelColor(0, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(random(0, 255)), 255, 255));
  for (int idex = 1; idex < NUM_MAIN_LEDS ; idex++ ) {
    iCCW = adjacent_ccw(idex);
    //leds[idex].r = ledsX[iCCW][0];
    //leds[idex].g = ledsX[iCCW][1];
    //leds[idex].b = ledsX[iCCW][2];
    mainStrip.setPixelColor(idex, ledsX[iCCW][0], ledsX[iCCW][1], ledsX[iCCW][2]);
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void rwb_march(Adafruit_NeoPixel &mainStrip) {                    //-m15-R,W,B MARCH CCW
  copy_led_array(mainStrip);
  int iCCW;
  idex++;
  if (idex > 2) {
    idex = 0;
  }
  switch (idex) {
    case 0:
      // leds[0].r = 255;
      // leds[0].g = 0;
      // leds[0].b = 0;
      mainStrip.setPixelColor(0, 255, 0, 0);
      break;
    case 1:
      // leds[0].r = 255;
      // leds[0].g = 255;
      // leds[0].b = 255;
      mainStrip.setPixelColor(0, 255, 255, 255);
      break;
    case 2:
      // leds[0].r = 0;
      // leds[0].g = 0;
      // leds[0].b = 255;
      mainStrip.setPixelColor(0, 0, 0, 255);
      break;
  }
  for (int i = 1; i < NUM_MAIN_LEDS; i++ ) {
    iCCW = adjacent_ccw(i);
    // leds[i].r = ledsX[iCCW][0];
    // leds[i].g = ledsX[iCCW][1];
    // leds[i].b = ledsX[iCCW][2];
    mainStrip.setPixelColor(i, ledsX[iCCW][0], ledsX[iCCW][1], ledsX[iCCW][2]);
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void radiation(Adafruit_NeoPixel &mainStrip) {                   //-m16-SORT OF RADIATION SYMBOLISH-
  int N3  = int(NUM_MAIN_LEDS / 3);
  int N6  = int(NUM_MAIN_LEDS / 6);
  int N12 = int(NUM_MAIN_LEDS / 12);
  for (int i = 0; i < N6; i++ ) {    //-HACKY, I KNOW...
    tcount = tcount + .02;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount) * 255);
    int j0 = (i + NUM_MAIN_LEDS - N12) % NUM_MAIN_LEDS;
    int j1 = (j0 + N3) % NUM_MAIN_LEDS;
    int j2 = (j1 + N3) % NUM_MAIN_LEDS;
    //leds[j0] = CHSV(thishue, thissat, ibright);
    mainStrip.setPixelColor(j0, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
    //leds[j1] = CHSV(thishue, thissat, ibright);
    mainStrip.setPixelColor(j1, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
    //leds[j2] = CHSV(thishue, thissat, ibright);
    mainStrip.setPixelColor(j2, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void color_loop_vardelay(Adafruit_NeoPixel &mainStrip) {                    //-m17-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  idex++;
  if (idex > NUM_MAIN_LEDS) {
    idex = 0;
  }
  int di = abs(TOP_INDEX - idex);
  int t = 0;
  if(di != 0)
    t = constrain((10 / di) * 10, 10, 500);
  else
    t = constrain(0, 10, 500);
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if (i == idex) {
      //leds[i] = CHSV(0, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(0), thissat, 255));
    }
    else {
      // leds[i].r = 0; 
      // leds[i].g = 0; 
      // leds[i].b = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(t)) return;
}

void white_temps(Adafruit_NeoPixel &mainStrip) {                            //-m18-SHOW A SAMPLE OF BLACK BODY RADIATION COLOR TEMPERATURES
  int N9 = int(NUM_MAIN_LEDS / 9);
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if (i >= 0 && i < N9) {
      // leds[i].r = 255;  //-CANDLE - 1900
      // leds[i].g = 147;
      // leds[i].b = 41;
      mainStrip.setPixelColor(i, 255, 147, 41);
    }
    if (i >= N9 && i < N9 * 2) {
      // leds[i].r = 255;  //-40W TUNG - 2600
      // leds[i].g = 197;
      // leds[i].b = 143;
      mainStrip.setPixelColor(i, 255, 197, 143);
    }
    if (i >= N9 * 2 && i < N9 * 3) {
      // leds[i].r = 255;  //-100W TUNG - 2850
      // leds[i].g = 214;
      // leds[i].b = 170;
      mainStrip.setPixelColor(i, 255, 214, 170);
    }
    if (i >= N9 * 3 && i < N9 * 4) {
      // leds[i].r = 255;  //-HALOGEN - 3200
      // leds[i].g = 241;
      // leds[i].b = 224;
      mainStrip.setPixelColor(i, 255, 241, 224);
    }
    if (i >= N9 * 4 && i < N9 * 5) {
      // leds[i].r = 255;  //-CARBON ARC - 5200
      // leds[i].g = 250;
      // leds[i].b = 244;
      mainStrip.setPixelColor(i, 255, 250, 244);
    }
    if (i >= N9 * 5 && i < N9 * 6) {
      // leds[i].r = 255;  //-HIGH NOON SUN - 5400
      // leds[i].g = 255;
      // leds[i].b = 251;
      mainStrip.setPixelColor(i, 255, 255, 251);
    }
    if (i >= N9 * 6 && i < N9 * 7) {
      // leds[i].r = 255;  //-DIRECT SUN - 6000
      // leds[i].g = 255;
      // leds[i].b = 255;
      mainStrip.setPixelColor(i, 255, 255, 255);
    }
    if (i >= N9 * 7 && i < N9 * 8) {
      // leds[i].r = 201;  //-OVERCAST SKY - 7000
      // leds[i].g = 226;
      // leds[i].b = 255;
      mainStrip.setPixelColor(i, 201, 226, 255);
    }
    if (i >= N9 * 8 && i < NUM_MAIN_LEDS) {
      // leds[i].r = 64;  //-CLEAR BLUE SKY - 20000
      // leds[i].g = 156;
      // leds[i].b = 255;
      mainStrip.setPixelColor(i, 64, 156, 255);
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  delay(100);
}

void sin_bright_wave(Adafruit_NeoPixel &mainStrip) {        //-m19-BRIGHTNESS SINE WAVE
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if(needBreakEffect)
      return;
    tcount = tcount + .1;
    if (tcount > 3.14) {
      tcount = 0.0;
    }
    ibright = int(sin(tcount) * 255);
    //leds[i] = CHSV(thishue, thissat, ibright);
    mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
    //LEDS.show(); 
    mainStrip.show();
    if (safeDelay(thisdelay) || needBreakEffect) return;
  }
}

void pop_horizontal(Adafruit_NeoPixel &mainStrip) {        //-m20-POP FROM LEFT TO RIGHT UP THE RING
  int ix;
  if (bouncedirection == 0) {
    bouncedirection = 1;
    ix = idex;
  }
  else if (bouncedirection == 1) {
    bouncedirection = 0;
    ix = horizontal_index(idex);
    idex++;
    if (idex > TOP_INDEX) {
      idex = 0;
    }
  }
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    if (i == ix) {
      //leds[i] = CHSV(thishue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
    }
    else {
      // leds[i].r = 0; 
      // leds[i].g = 0; 
      // leds[i].b = 0;
      mainStrip.setPixelColor(i, 0, 0, 0);
    }
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void quad_bright_curve(Adafruit_NeoPixel &mainStrip) {      //-m21-QUADRATIC BRIGHTNESS CURVER
  int ax;
  for (int x = 0; x < NUM_MAIN_LEDS; x++ ) {
    if (x <= TOP_INDEX) {
      ax = x;
    }
    else if (x > TOP_INDEX) {
      ax = NUM_MAIN_LEDS - x;
    }
    int a = 1; int b = 1; int c = 0;
    int iquad = -(ax * ax * a) + (ax * b) + c; //-ax2+bx+c
    int hquad = -(TOP_INDEX * TOP_INDEX * a) + (TOP_INDEX * b) + c;
    ibright = int((float(iquad) / float(hquad)) * 255);
    //leds[x] = CHSV(thishue, thissat, ibright);
    mainStrip.setPixelColor(x, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, ibright));
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void flame(Adafruit_NeoPixel &mainStrip) {                                    //-m22-FLAMEISH EFFECT
  int idelay = random(0, 35);
  float hmin = 0.1; float hmax = 45.0;
  float hdif = hmax - hmin;
  int randtemp = random(0, 3);
  float hinc = (hdif / float(TOP_INDEX)) + randtemp;
  int ihue = hmin;
  for (int i = 0; i <= TOP_INDEX; i++ ) {
    ihue = ihue + hinc;
    //leds[i] = CHSV(ihue, thissat, 255);
    mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
    int ih = horizontal_index(i);
    //leds[ih] = CHSV(ihue, thissat, 255);
    mainStrip.setPixelColor(ih, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
    // leds[TOP_INDEX].r = 255; 
    // leds[TOP_INDEX].g = 255; 
    // leds[TOP_INDEX].b = 255;
    mainStrip.setPixelColor(TOP_INDEX, 255, 255, 255);
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(idelay)) return;
  }
}

void rainbow_vertical(Adafruit_NeoPixel &mainStrip) {                        //-m23-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {
    idex = 0;
  }
  ihue = ihue + thisstep;
  if (ihue > 255) {
    ihue = 0;
  }
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  //leds[idexA] = CHSV(ihue, thissat, 255);
  mainStrip.setPixelColor(idexA, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
  //leds[idexB] = CHSV(ihue, thissat, 255);
  mainStrip.setPixelColor(idexB, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void pacman(Adafruit_NeoPixel &mainStrip) {                                  //-m24-REALLY TERRIBLE PACMAN CHOMPING EFFECT
  int s = int(NUM_MAIN_LEDS / 4);
  lcount++;
  if (lcount > 5) {
    lcount = 0;
  }
  if (lcount == 0) {
    for (int i = 0 ; i < NUM_MAIN_LEDS; i++ ) {
      set_color_led(mainStrip, i, 255, 255, 0);
    }
  }
  if (lcount == 1 || lcount == 5) {
    for (int i = 0 ; i < NUM_MAIN_LEDS; i++ ) {
      set_color_led(mainStrip, i, 255, 255, 0);
    }
    // leds[s].r = 0; 
    // leds[s].g = 0; 
    // leds[s].b = 0;
    mainStrip.setPixelColor(s, 0, 0, 0);
  }
  if (lcount == 2 || lcount == 4) {
    for (int i = 0 ; i < NUM_MAIN_LEDS; i++ ) {
      set_color_led(mainStrip, i, 255, 255, 0);
    }
    // leds[s - 1].r = 0; 
    // leds[s - 1].g = 0; 
    // leds[s - 1].b = 0;
    mainStrip.setPixelColor(s - 1, 0, 0, 0);
    
    // leds[s].r = 0; 
    // leds[s].g = 0; 
    // leds[s].b = 0;
    mainStrip.setPixelColor(s, 0, 0, 0);
    
    // leds[s + 1].r = 0; 
    // leds[s + 1].g = 0; 
    // leds[s + 1].b = 0;
    mainStrip.setPixelColor(s + 1, 0, 0, 0);
  }
  if (lcount == 3) {
    for (int i = 0 ; i < NUM_MAIN_LEDS; i++ ) {
      set_color_led(mainStrip, i, 255, 255, 0);
    }
    // leds[s - 2].r = 0; 
    // leds[s - 2].g = 0; 
    // leds[s - 2].b = 0;
    mainStrip.setPixelColor(s - 2, 0, 0, 0);
    
    // leds[s - 1].r = 0; 
    // leds[s - 1].g = 0; 
    // leds[s - 1].b = 0;
    mainStrip.setPixelColor(s - 1, 0, 0, 0);
    
    // leds[s].r = 0; 
    // leds[s].g = 0; 
    // leds[s].b = 0;
    mainStrip.setPixelColor(s, 0, 0, 0);
    
    // leds[s + 1].r = 0; 
    // leds[s + 1].g = 0; 
    // leds[s + 1].b = 0;
    mainStrip.setPixelColor(s + 1, 0, 0, 0);
    
    // leds[s + 2].r = 0; 
    // leds[s + 2].g = 0; 
    // leds[s + 2].b = 0;
    mainStrip.setPixelColor(s + 2, 0, 0, 0);
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void random_color_pop(Adafruit_NeoPixel &mainStrip) {                         //-m25-RANDOM COLOR POP
  idex = random(0, NUM_MAIN_LEDS);
  ihue = random(0, 255);
  //one_color_all(0, 0, 0);
  //mainStrip.fill(0, 0, 0);
  setAll(mainStrip, 0, 0, 0);
  //leds[idex] = CHSV(ihue, thissat, 255);
  mainStrip.setPixelColor(idex, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ihue), thissat, 255));
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void ems_lightsSTROBE(Adafruit_NeoPixel &mainStrip) {                  //-m26-EMERGENCY LIGHTS (STROBE LEFT/RIGHT)
  int thishue = 0;
  int thathue = (thishue + 160) % 255;
  for (int x = 0 ; x < 5; x++ ) {
    for (int i = 0 ; i < TOP_INDEX; i++ ) {
      //leds[i] = CHSV(thishue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
    }
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(thisdelay)) return;
    //one_color_all(0, 0, 0);
    //mainStrip.fill(0, 0, 0);
    setAll(mainStrip, 0, 0, 0);
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(thisdelay)) return;
  }
  for (int x = 0 ; x < 5; x++ ) {
    for (int i = TOP_INDEX ; i < NUM_MAIN_LEDS; i++ ) {
      //leds[i] = CHSV(thathue, thissat, 255);
      mainStrip.setPixelColor(i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
    }
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(thisdelay)) return;
    //one_color_all(0, 0, 0);
    //mainStrip.fill(0, 0, 0);
    setAll(mainStrip, 0, 0, 0);
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(thisdelay)) return;
  }
}

void rgb_propeller(Adafruit_NeoPixel &mainStrip) {                           //-m27-RGB PROPELLER
  idex++;
  int ghue = (thishue + 80) % 255;
  int bhue = (thishue + 160) % 255;
  int N3  = int(NUM_MAIN_LEDS / 3);
  int N6  = int(NUM_MAIN_LEDS / 6);
  int N12 = int(NUM_MAIN_LEDS / 12);
  for (int i = 0; i < N3; i++ ) {
    int j0 = (idex + i + NUM_MAIN_LEDS - N12) % NUM_MAIN_LEDS;
    int j1 = (j0 + N3) % NUM_MAIN_LEDS;
    int j2 = (j1 + N3) % NUM_MAIN_LEDS;
    //leds[j0] = CHSV(thishue, thissat, 255);
    mainStrip.setPixelColor(j0, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
    //leds[j1] = CHSV(ghue, thissat, 255);
    mainStrip.setPixelColor(j1, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(ghue), thissat, 255));
    //leds[j2] = CHSV(bhue, thissat, 255);
    mainStrip.setPixelColor(j2, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(bhue), thissat, 255));
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void kitt(Adafruit_NeoPixel &mainStrip) {                                     //-m28-KNIGHT INDUSTIES 2000
  int rand = random(0, TOP_INDEX);
  for (int i = 0; i < rand; i++ ) {
    //leds[TOP_INDEX + i] = CHSV(thishue, thissat, 255);
    mainStrip.setPixelColor(TOP_INDEX + i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
    //leds[TOP_INDEX - i] = CHSV(thishue, thissat, 255);
    mainStrip.setPixelColor(TOP_INDEX - i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(thisdelay / rand)) return;
  }
  for (int i = rand; i > 0; i-- ) {
    //leds[TOP_INDEX + i] = CHSV(thishue, thissat, 0);
    mainStrip.setPixelColor(TOP_INDEX + i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 0));
    //leds[TOP_INDEX - i] = CHSV(thishue, thissat, 0);
    mainStrip.setPixelColor(TOP_INDEX - i, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 0));
    //LEDS.show(); 
  mainStrip.show();
    if (safeDelay(thisdelay / rand)) return;
  }
}

void matrix(Adafruit_NeoPixel &mainStrip) {                                   //-m29-ONE LINE MATRIX
  int rand = random(0, 100);
  if (rand > 90) {
    // leds[0] = CHSV(thishue, thissat, 255);
    mainStrip.setPixelColor(0, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 255));
  }
  else {
    //leds[0] = CHSV(thishue, thissat, 0);
    mainStrip.setPixelColor(0, mainStrip.ColorHSV(convert_8bit_hue_to_16bit(thishue), thissat, 0));
  }
  copy_led_array(mainStrip);
  for (int i = 1; i < NUM_MAIN_LEDS; i++ ) {
    // leds[i].r = ledsX[i - 1][0];
    // leds[i].g = ledsX[i - 1][1];
    // leds[i].b = ledsX[i - 1][2];
    mainStrip.setPixelColor(i, ledsX[i - 1][0], ledsX[i - 1][1], ledsX[i - 1][2]);
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void strip_march_cw(Adafruit_NeoPixel &mainStrip) {                        //-m50-MARCH STRIP CW
  copy_led_array(mainStrip);
  int iCW;
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    iCW = adjacent_cw(i);
    // leds[i].r = ledsX[iCW][0];
    // leds[i].g = ledsX[iCW][1];
    // leds[i].b = ledsX[iCW][2];
    mainStrip.setPixelColor(i, ledsX[iCW][0], ledsX[iCW][1], ledsX[iCW][2]);
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

void strip_march_ccw(Adafruit_NeoPixel &mainStrip) {                        //-m51-MARCH STRIP CCW
  copy_led_array(mainStrip);
  int iCCW;
  for (int i = 0; i < NUM_MAIN_LEDS; i++ ) {
    iCCW = adjacent_ccw(i);
    // leds[i].r = ledsX[iCCW][0];
    // leds[i].g = ledsX[iCCW][1];
    // leds[i].b = ledsX[iCCW][2];
    mainStrip.setPixelColor(i, ledsX[iCCW][0], ledsX[iCCW][1], ledsX[iCCW][2]);
  }
  //LEDS.show(); 
  mainStrip.show();
  if (safeDelay(thisdelay)) return;
}

// void new_rainbow_loop(Adafruit_NeoPixel &mainStrip) {                      //-m88-RAINBOW FADE FROM FAST_SPI2
//   ihue -= 1;
//   fill_rainbow( leds, NUM_MAIN_LEDS, ihue );
//   //LEDS.show(); 
//   mainStrip.show();
//   if (safeDelay(thisdelay)) return;
// }

void demo_modeA(Adafruit_NeoPixel &mainStrip) {
  int r = 10;
  thisdelay = 20; thisstep = 10; thishue = 0; thissat = 255;
  setAll(mainStrip, 255, 255, 255);
  mainStrip.show();
  if (safeDelay(1200)) return;
  for (int i = 0; i < r * 25; i++) {
    rainbow_fade(mainStrip);
  }
  for (int i = 0; i < r * 20; i++) {
    rainbow_loop(mainStrip);
  }
  for (int i = 0; i < r * 20; i++) {
    random_burst(mainStrip);
  }
  for (int i = 0; i < r * 12; i++) {
    color_bounce(mainStrip);
  }

  thisdelay = 40;
  for (int i = 0; i < r * 12; i++) {
    color_bounceFADE(mainStrip);
  }
  for (int i = 0; i < r * 6; i++) {
    ems_lightsONE(mainStrip);
  }
  for (int i = 0; i < r * 5; i++) {
    ems_lightsALL(mainStrip);
  }
  thishue = 160; thissat = 50;
  for (int i = 0; i < r * 40; i++) {
    flicker(mainStrip);
  }

  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();

  thisdelay = 15; 
  thishue = 0; 
  thissat = 255;

  for (int i = 0; i < r * 50; i++) {
    pulse_one_color_all(mainStrip);
  }
  for (int i = 0; i < r * 40; i++) {
    pulse_one_color_all_rev(mainStrip);
  }
  thisdelay = 60; thishue = 180;
  for (int i = 0; i < r * 5; i++) {
    fade_vertical(mainStrip);
  }
  random_red(mainStrip);
  thisdelay = 100;
  for (int i = 0; i < r * 5; i++) {
    rule30(mainStrip);
  }
  thisdelay = 40;
  for (int i = 0; i < r * 8; i++) {
    random_march(mainStrip);
  }
  thisdelay = 80;
  for (int i = 0; i < r * 5; i++) {
    rwb_march(mainStrip);
  }
  
  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();

  thisdelay = 60; 
  thishue = 95;
  for (int i = 0; i < r * 15; i++) {
    radiation(mainStrip);
  }
  for (int i = 0; i < r * 15; i++) {
    color_loop_vardelay(mainStrip);
  }
  for (int i = 0; i < r * 5; i++) {
    white_temps(mainStrip);
  }
  thisdelay = 35; thishue = 180;
  for (int i = 0; i < r; i++) {
    sin_bright_wave(mainStrip);
  }
  thisdelay = 100; thishue = 0;
  for (int i = 0; i < r * 5; i++) {
    pop_horizontal(mainStrip);
  }
  thisdelay = 100; thishue = 180;
  for (int i = 0; i < r * 4; i++) {
    quad_bright_curve(mainStrip);
  }

  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();

  for (int i = 0; i < r * 3; i++) {
    flame(mainStrip);
  }
  thisdelay = 50;
  for (int i = 0; i < r * 10; i++) {
    pacman(mainStrip);
  }
  thisdelay = 50; thisstep = 15;
  for (int i = 0; i < r * 12; i++) {
    rainbow_vertical(mainStrip);
  }
  thisdelay = 100;
  for (int i = 0; i < r * 3; i++) {
    strip_march_ccw(mainStrip);
  }
  for (int i = 0; i < r * 3; i++) {
    strip_march_cw(mainStrip);
  }

  demo_modeB(mainStrip);

  thisdelay = 5;
  setAll(mainStrip, 255, 0, 0);
  mainStrip.show(); 

  if (safeDelay(1200)) return;
  setAll(mainStrip, 0, 255, 0);
  mainStrip.show();

  if (safeDelay(1200)) return;
  setAll(mainStrip, 0, 0, 255);
  mainStrip.show();
  
  if (safeDelay(1200)) return;
  setAll(mainStrip, 255, 255, 0);
  mainStrip.show();

  if (safeDelay(1200)) return;
  setAll(mainStrip, 0, 255, 255);
  mainStrip.show();

  if (safeDelay(1200)) return;
  setAll(mainStrip, 255, 0, 255);
  mainStrip.show();

  if (safeDelay(1200)) return;
}

void demo_modeB(Adafruit_NeoPixel &mainStrip) {
  int r = 10;
  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();
  thisdelay = 35;
  for (int i = 0; i < r * 10; i++) {
    random_color_pop(mainStrip);
  }
  for (int i = 0; i < r / 2; i++) {
    ems_lightsSTROBE(mainStrip);
  }
  thisdelay = 50;
  for (int i = 0; i < r * 10; i++) {
    rgb_propeller(mainStrip);
  }
  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();
  thisdelay = 100; thishue = 0;
  for (int i = 0; i < r * 3; i++) {
    kitt(mainStrip);
  }
  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();
  thisdelay = 30; thishue = 95;
  for (int i = 0; i < r * 25; i++) {
    matrix(mainStrip);
  }
  setAll(mainStrip, 0, 0, 0);
  mainStrip.show();
}

//-----------------------------плавное заполнение цветом-----------------------------------------
void colorWipe(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int SpeedDelay) {
  for (uint16_t i = 0; i < NUM_MAIN_LEDS; i++) {
    setPixel(mainStrip, i, red, green, blue);
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }
}
//-----------------------------------бегающие светодиоды-----------------------------------
void CylonBounce(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < NUM_MAIN_LEDS - EyeSize - 2; i++) {
    setAll(mainStrip, 0, 0, 0);
    setPixel(mainStrip, i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip, i + j, red, green, blue);
    }
    setPixel(mainStrip, i + EyeSize + 1, red / 10, green / 10, blue / 10);
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }

  if (safeDelay(ReturnDelay)) return;

  for (int i = NUM_MAIN_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(mainStrip, 0, 0, 0);
    setPixel(mainStrip, i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip, i + j, red, green, blue);
    }
    setPixel(mainStrip, i + EyeSize + 1, red / 10, green / 10, blue / 10);
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }

  if (safeDelay(ReturnDelay)) return;
}

//---------------------------------линейный огонь-------------------------------------
void Fire(Adafruit_NeoPixel &mainStrip, int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_MAIN_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_MAIN_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_MAIN_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_MAIN_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_MAIN_LEDS; j++) {
    setPixelHeatColor(mainStrip, j, heat[j]);
  }

  mainStrip.show();
  if (safeDelay(SpeedDelay)) return;
}

void setPixelHeatColor (Adafruit_NeoPixel &mainStrip,int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(mainStrip, Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(mainStrip, Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(mainStrip, Pixel, heatramp, 0, 0);
  }
}

//-------------------------------newKITT---------------------------------------
void CenterToOutside(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) 
{
  for (int i = ((NUM_MAIN_LEDS - EyeSize) / 2); i >= 0; i--) 
  {
    setAll(mainStrip, 0, 0, 0);
    setPixel(mainStrip, i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip, i + j, red, green, blue);
    }
    setPixel(mainStrip, i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(mainStrip, NUM_MAIN_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip, NUM_MAIN_LEDS - i - j, red, green, blue);
    }
    setPixel(mainStrip, NUM_MAIN_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }
  if (safeDelay(ReturnDelay)) return;
}

void OutsideToCenter(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i <= ((NUM_MAIN_LEDS - EyeSize) / 2); i++) {
    setAll(mainStrip,0, 0, 0);

    setPixel(mainStrip,i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip,i + j, red, green, blue);
    }
    setPixel(mainStrip,i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(mainStrip, NUM_MAIN_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip, NUM_MAIN_LEDS - i - j, red, green, blue);
    }
    setPixel(mainStrip, NUM_MAIN_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }
  if (safeDelay(ReturnDelay)) return;
}

void LeftToRight(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i < NUM_MAIN_LEDS - EyeSize - 2; i++) {
    setAll(mainStrip,0, 0, 0);
    setPixel(mainStrip,i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip,i + j, red, green, blue);
    }
    setPixel(mainStrip,i + EyeSize + 1, red / 10, green / 10, blue / 10);
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }
  if (safeDelay(ReturnDelay)) return;
}

void RightToLeft(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = NUM_MAIN_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(mainStrip, 0, 0, 0);
    setPixel(mainStrip, i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(mainStrip,i + j, red, green, blue);
    }
    setPixel(mainStrip,i + EyeSize + 1, red / 10, green / 10, blue / 10);
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }
  if (safeDelay(ReturnDelay)) return;
}


void NewKITT(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  RightToLeft(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(mainStrip, red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}
//-------------------------------newKITT---------------------------------------
void rainbowCycle(Adafruit_NeoPixel &mainStrip, int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_MAIN_LEDS; i++) {
      c = Wheel(((i * 256 / NUM_MAIN_LEDS) + j) & 255);
      setPixel(mainStrip, i, *c, *(c + 1), *(c + 2));
    }
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

//-------------------------------TwinkleRandom---------------------------------------
void TwinkleRandom(Adafruit_NeoPixel &mainStrip, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(mainStrip, 0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(mainStrip, random(NUM_MAIN_LEDS), random(0, 255), random(0, 255), random(0, 255));
    mainStrip.show();
    if (safeDelay(SpeedDelay)) return;
    if (OnlyOne) {
      setAll(mainStrip, 0, 0, 0);
    }
  }

  if (safeDelay(SpeedDelay)) return;
}

//-------------------------------RunningLights---------------------------------------
void RunningLights(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int WaveDelay) {
  int Position = 0;

  for (int i = 0; i < NUM_MAIN_LEDS * 2; i++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_MAIN_LEDS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(mainStrip, i, ((sin(i + Position) * 127 + 128) / 255)*red,
               ((sin(i + Position) * 127 + 128) / 255)*green,
               ((sin(i + Position) * 127 + 128) / 255)*blue);
    }

    mainStrip.show();
    if (safeDelay(WaveDelay)) return;
  }
}

//-------------------------------Sparkle---------------------------------------
void Sparkle(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_MAIN_LEDS);
  setPixel(mainStrip, Pixel, red, green, blue);
  mainStrip.show();
  if (safeDelay(SpeedDelay)) return;
  setPixel(mainStrip, Pixel, 0, 0, 0);
}

//-------------------------------SnowSparkle---------------------------------------
void SnowSparkle(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(mainStrip, red, green, blue);

  int Pixel = random(NUM_MAIN_LEDS);
  setPixel(mainStrip, Pixel, 0xff, 0xff, 0xff);
  mainStrip.show();
  if (safeDelay(SparkleDelay)) return;
  setPixel(mainStrip, Pixel, red, green, blue);
  mainStrip.show();
  if (safeDelay(SpeedDelay)) return;
}

//-------------------------------theaterChase---------------------------------------
void theaterChase(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int SpeedDelay) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_MAIN_LEDS; i = i + 3) {
        setPixel(mainStrip, i + q, red, green, blue);  //turn every third pixel on
      }
      mainStrip.show();
      if (safeDelay(SpeedDelay)) return;
      for (int i = 0; i < NUM_MAIN_LEDS; i = i + 3) {
        setPixel(mainStrip, i + q, 0, 0, 0);    //turn every third pixel off
      }
    }
  }
}

//-------------------------------theaterChaseRainbow---------------------------------------
void theaterChaseRainbow(Adafruit_NeoPixel &mainStrip, int SpeedDelay) {
  byte *c;

  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < NUM_MAIN_LEDS; i = i + 3) {
        c = Wheel( (i + j) % 255);
        setPixel(mainStrip, i + q, *c, *(c + 1), *(c + 2)); //turn every third pixel on
      }
      mainStrip.show();
      if (safeDelay(SpeedDelay)) return;
      for (int i = 0; i < NUM_MAIN_LEDS; i = i + 3) {
        setPixel(mainStrip, i + q, 0, 0, 0);    //turn every third pixel off
      }
    }
  }
}

//-------------------------------Strobe---------------------------------------
void Strobe(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
  for (int j = 0; j < StrobeCount; j++) {
    setAll(mainStrip, red, green, blue);
    mainStrip.show();
    if (safeDelay(FlashDelay)) return;
    setAll(mainStrip, 0, 0, 0);
    mainStrip.show();
    if (safeDelay(FlashDelay)) return;
  }
  if (safeDelay(EndPause)) return;
}

//-------------------------------BouncingBalls---------------------------------------
void BouncingBalls(Adafruit_NeoPixel &mainStrip, byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
  }

  while (true) {
    if (changeFlag) {
      changeFlag = false;
      return;
    }
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_MAIN_LEDS - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(mainStrip, Position[i], red, green, blue);
    }
    mainStrip.show();
    setAll(mainStrip, 0, 0, 0);
  }
}

//-------------------------------BouncingColoredBalls---------------------------------------
void BouncingColoredBalls(Adafruit_NeoPixel &mainStrip, int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
  }

  while (true) {
    if (changeFlag) {
      changeFlag = false;
      return;
    }
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_MAIN_LEDS - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(mainStrip, Position[i], colors[i][0], colors[i][1], colors[i][2]);
    }
    mainStrip.show();
    setAll(mainStrip, 0, 0, 0);
  }
}

void show_fx(Adafruit_NeoPixel &mainStrip, uint8_t effectId)
{
  int r = 10;
  thisdelay = 20; thisstep = 10; thishue = 0; thissat = 255;
  switch(effectId)
  {
    case 0: for (int i = 0; i < r * 25; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } rainbow_fade(mainStrip); } return;
    case 1: for (int i = 0; i < r * 20; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } rainbow_loop(mainStrip); } return;
    case 2: for (int i = 0; i < r * 20; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } random_burst(mainStrip); } return;
    case 3: for (int i = 0; i < r * 12; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } color_bounce(mainStrip); } return;
  }

  // if(effectId != 10
  //   || effectId != 12
  //   || effectId != 13
  //   || effectId != 14)
  // {
  //   setAll(mainStrip, 0, 0, 0);
  //   mainStrip.show();
  // }
  thisdelay = 40;
  switch (effectId)
  {
    case 4: for (int i = 0; i < r * 12; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } color_bounceFADE(mainStrip); } return;
    case 5: for (int i = 0; i < r * 6; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } ems_lightsONE(mainStrip); } return;
    case 6: setAll(mainStrip, 0, 0, 0); mainStrip.show(); for (int i = 0; i < r * 5; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } ems_lightsALL(mainStrip); } return;
  }

  thishue = 160; thissat = 50;
  switch (effectId)
  {
    case 7: for (int i = 0; i < r * 40; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } flicker(mainStrip); } return;
  }

  thisdelay = 15; 
  thishue = 0; 
  thissat = 255;
  switch (effectId)
  {
    case 8: for (int i = 0; i < r * 50; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } pulse_one_color_all(mainStrip); } return;
    case 9: for (int i = 0; i < r * 40; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } pulse_one_color_all_rev(mainStrip); } return;
  }
  
  thisdelay = 60; 
  thishue = 180;
  switch (effectId)
  {
    case 10: for (int i = 0; i < r * 5; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } fade_vertical(mainStrip); } return;
    case 11: mqttClient.loop(); random_red(mainStrip); return;
    case 12: thisdelay = 100; while(true) /*for (int i = 0; i < r * 5; i++)*/ { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } rule30(mainStrip); } return;
    case 13: thisdelay = 40; while(true) /*for (int i = 0; i < r * 8; i++)*/ { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } random_march(mainStrip); } return;
    case 14: thisdelay = 80; while(true) /*for (int i = 0; i < r * 6; i++)*/ { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } rwb_march(mainStrip); } return;
  }
  
  //setAll(mainStrip, 0, 0, 0);
  //mainStrip.show();

  thisdelay = 60; 
  thishue = 95;
  switch (effectId)
  {
    case 15: setAll(mainStrip, 0, 0, 0); mainStrip.show(); for (int i = 0; i < r * 15; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } radiation(mainStrip); } return;
    case 16: for (int i = 0; i < r * 15; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } color_loop_vardelay(mainStrip); }return;
    case 17: for (int i = 0; i < r * 5; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } white_temps(mainStrip); } return;
    //case 18: setAll(mainStrip, 0, 0, 0); mainStrip.show(); thisdelay = 35; thishue = 180; for (int i = 0; i < r; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } sin_bright_wave(mainStrip); } return;
    case 18: thisdelay = 100; thishue = 0; for (int i = 0; i < r * 5; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } pop_horizontal(mainStrip); } return;
    case 19: setAll(mainStrip, 0, 0, 0); mainStrip.show(); thisdelay = 100; thishue = 180; for (int i = 0; i < r * 4; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } quad_bright_curve(mainStrip); } return;
  }
  thisdelay = 100; thishue = 180;
  //setAll(mainStrip, 0, 0, 0);
  //mainStrip.show();
  
  switch (effectId)
  {
    case 20: for (int i = 0; i < r * 3; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } flame(mainStrip); } return;
    case 21: thisdelay = 50; for (int i = 0; i < r * 10; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } pacman(mainStrip); } return;
    case 22: thisdelay = 50; thisstep = 15; for (int i = 0; i < r * 12; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } rainbow_vertical(mainStrip); } return;
    case 23: thisdelay = 35; for (int i = 0; i < r * 10; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } random_color_pop(mainStrip); } return;
    case 24: thisdelay = 35; for (int i = 0; i < r / 2; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } ems_lightsSTROBE(mainStrip); } return;
    case 25: thisdelay = 50; for (int i = 0; i < r * 10; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; }rgb_propeller(mainStrip); } return;
    case 26: thisdelay = 100; thishue = 0; for (int i = 0; i < r * 3; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } kitt(mainStrip); } return;
    case 27: thisdelay = 30; thishue = 95; for (int i = 0; i < r * 25; i++) { mqttClient.loop(); if(needBreakEffect) { needBreakEffect = false; break; } matrix(mainStrip); }
  }  
}