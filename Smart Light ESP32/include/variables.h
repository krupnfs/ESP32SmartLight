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

bool additionalLightShowing = false;
bool readLightWarmShowing = false;
bool readLightColdShowing = false;

bool randomEffectEnabled = false;

uint8_t effectId = 0;

String wifi_ssid = "";
String wifi_password ="";

String mqtt_client_id = "esp32sl";
String mqtt_server = "";
uint16_t mqtt_server_port = 0;
String mqtt_server_user = "";
String mqtt_server_password = "";

bool useTopicPrefix = false;
String mqtt_topic_prefix = "/" + mqtt_server_user + "/";

// WiFi reconnect variables — infinite attempts with exponential backoff
unsigned long wifiReconnectTimer = 0;
unsigned long lastWiFiDisconnectTime = 0;
const unsigned int WIFI_RECONNECT_CHECK_INTERVAL = 5000;  // Check every 5s
bool wifiNeedsReconnect = false;
int wifiReconnectAttempts = 0;                              // For backoff calculation only, no limit
unsigned long wifiLastAttemptTime = 0;                      // For exponential backoff
bool wifiFirstConnectAfterBoot = true;                      // Show animation only once after boot

// WiFi reconnect animation
unsigned long wifiAnimationTimer = 0;
int wifiAnimationStep = 0;
bool wifiAnimationActive = false;
const unsigned int WIFI_ANIMATION_INTERVAL = 100;

// Watchdog (initialized in setup with esp_task_wdt_init)

// Read mode non-blocking timers
unsigned long readLightWarmTimer = 0;
unsigned long readLightColdTimer = 0;
