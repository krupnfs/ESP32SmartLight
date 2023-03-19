#define DOWN_CLICK 0
#define DOWN_HOLD 1
#define UP_CLICK 2
#define UP_HOLD 3
#define DOWN_HOLDING_PRESS 4
#define DOWN_HOLDING_RELEASE 5
#define UP_HOLDING_PRESS 6
#define UP_HOLDING_RELEASE 7
#define UNKNOWN_KEY -1

enum ClickType
{
    CLICK_ON_OFF = 0,
    CLICK_ON_CLICK_OFF = 1
};

enum HoldType
{
    HOLD_ON_OFF = 0,
    HOLD_ON_HOLD_OFF = 1,
    HOLD_ON_RELEASE_OFF = 2
};

#pragma region Modes
#define NIGHT_LIGHT 0
#define RAINBOW_MODE 1
#define READ_MODE 2
#pragma endregion

#pragma region VolumeModes
#define BRIGHTNESS_MODE 0
#define TEMPERATURE_MODE 1
#pragma endregion

#define FX_COUNT 29

const int sending_period = 3;
const bool retain_flag = false;

const char* SETTING_ID = "settingId";
const char* SETTING_VALUE = "value";

const char* PROGMEM topic = "esp32sl/#";

const char* PROGMEM topicNightLightPower = "esp32sl/nightlight/power";
const char* PROGMEM topicGetNightLightStatus = "esp32sl/nightlight/power/state";
const char* PROGMEM topicSetBrightnessNightLight = "esp32sl/nightlight/brightness";
const char* PROGMEM topicGetNightLightBrightness = "esp32sl/nightlight/brightness/state";
const char* PROGMEM topicSetTemperatureNightLight = "esp32sl/nightlight/temperature";

const char* PROGMEM topicReadLightPower = "esp32sl/readlight/power";
const char* PROGMEM topicGetReadLightStatus = "esp32sl/readlight/power/state";
const char* PROGMEM topicFlashLightPower = "esp32sl/flashlight/power";
const char* PROGMEM topicGetFlashLightStatus = "esp32sl/flashlight/power/state";


