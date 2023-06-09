#define EEPROM_SIZE 1024

#define ON_OFF_STATE_ADDRESS 0
#define MODE_STATE_ADDRESS 1
#define BRIGHTNESS_STATE_ADDRESS 2
#define TEMPERATURE_STATE_ADDRESS 3
#define EFFECT_ID_ADDRESS 4
#define VERSION_ADDRESS 5
#define WIFI_SSID_ADDRESS (VERSION_ADDRESS+10)
#define WIFI_PASSWORD_ADDRESS (WIFI_SSID_ADDRESS+33)
#define WARM_ADDRESS (WIFI_PASSWORD_ADDRESS+33)
#define COLD_ADDRESS (WARM_ADDRESS+1)
#define MQTT_SERVER_ADDRESS (COLD_ADDRESS+1)
#define MQTT_SERVER_PORT_ADDRESS (MQTT_SERVER_ADDRESS+33)
#define MQTT_SERVER_USER_ADDRESS (MQTT_SERVER_PORT_ADDRESS+4)
#define MQTT_SERVER_PASSWORD_ADDRESS (MQTT_SERVER_USER_ADDRESS+33)
#define ADDITION_ENABLE_NIGHTLIGHT_ADDRESS (MQTT_SERVER_PASSWORD_ADDRESS+33)
#define DEVICE_TOPIC_ADDRESS (ADDITION_ENABLE_NIGHTLIGHT_ADDRESS+1)