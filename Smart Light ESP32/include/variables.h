#include "Arduino.h"
#include "consts.h"

unsigned long onOffMillis = 0;
unsigned long downMillis = 0;
unsigned long upMillis = 0;

bool onOffPressed = false;
bool downPressed = false;
bool upPressed = false;

bool showConnected = false;

bool mqttInitialized = false;

bool onOffChanged = false;
bool lightModeChanged = false;
bool effectIdChanged = false;
bool brightnessChanged = false;
bool temperatureChanged = false;
bool warmChanged = false;
bool coldChanged = false;
bool enableAdditionNightLightChanged = false;

uint32_t publishTimer;

//для NeoPixel радуги
//uint16_t counter;

bool LEDS_ON = false;

uint16_t clickTimeOut = 200;

uint8_t light_mode = NIGHT_LIGHT;

bool NIGHT_LIGHT_BRIGHTNESS_MODE = false;
uint8_t brightness = 4;
uint8_t temperature = 3;

bool READ_LIGHT_WARM_MODE = false;
uint8_t warm_value = 4;
uint8_t cold_value = 4;

bool enableAdditionNightLight = false;

uint8_t effectId = 0;

String wifi_ssid = "";
String wifi_password ="";

String mqtt_client_id = "esp32sl_totoro";
String mqtt_server = "m4.wqtt.ru";
uint16_t mqtt_server_port = 7174;
String mqtt_server_user = "u_M31GZB";
String mqtt_server_password = "pXtRs7BF";

bool useTopicPrefix = false;
String mqtt_topic_prefix = "/" + mqtt_server_user + "/";