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

String device_topic = "esp32sl_my";

String topic() { return device_topic + "/#"; }

String topicSetNightLightPower() { return device_topic + "/nightlight/power"; }
String topicGetNightLightPower() { return device_topic + "/nightlight/power/state"; }
String topicSetNightLightBrightness() { return device_topic + "/nightlight/brightness"; }
String topicGetNightLightBrightness() { return device_topic + "/nightlight/brightness/state"; }
String topicSetNightLightTemperature() { return device_topic + "/nightlight/temperature"; }
String topicGetNightLightTemperature() { return device_topic + "/nightlight/temperature/state"; }

String topicSetReadLightPower() { return device_topic + "/readlight/power"; }
String topicGetReadLightPower() { return device_topic + "/readlight/power/state"; }
String topicSetWarmChannel() { return device_topic + "/readlight/warmchannel"; }
String topicGetWarmChannel() { return device_topic + "/readlight/warmchannel/state"; }
String topicSetColdChannel() { return device_topic + "/readlight/coldchannel"; }
String topicGetColdChannel() { return device_topic + "/readlight/coldchannel/state"; }
String topicSetAdditionalLight() { return device_topic + "/readlight/additionallight"; }
String topicGetAdditionalLight() { return device_topic + "/readlight/additionallight/state"; }

String topicSetFlashLightPower() { return device_topic + "/flashlight/power"; }
String topicGetFlashLightPower() { return device_topic + "/flashlight/power/state"; }
String topicSetEffectFlashLight() { return device_topic + "/flashlight/effect"; }
String topicGetEffectFlashLight() { return device_topic + "/flashlight/effect/state"; }

// String topicComboSetPower = device_topic + "/combo/setpower";
// String topicComboGetPower = device_topic + "/combo/getpower/state";
// String topicComboSetMode = device_topic + "/combo/setmode";
// String topicComboGetMode = device_topic + "/combo/getmode/state";
// String topicComboSetBrightness = device_topic + "/combo/setbrightness/";
// String topicComboGetBrightness = device_topic + "/combo/getbrightness/state";


