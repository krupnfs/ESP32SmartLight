#include "EEPROM_addresses.h"
#include "configuration.h"
#include "colors.h"
#include "led_draw.h"
#include "variables.h"

#include <version.h>

#include "effects.h"

#include "Arduino.h"
#include "EEPROM.h"
#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "PubSubClient.h"

#include "index_html.h"

AsyncWebServer server(80);

Adafruit_NeoPixel volumeStrip = Adafruit_NeoPixel(NUM_VOLUME_LEDS, VOLUME_LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel mainStrip = Adafruit_NeoPixel(NUM_MAIN_LEDS, MAIN_LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

#pragma region WebServer and MQTT Helpers
bool isMQTTConnected()
{
  return mqttClient.connected();
}

String isMQTTConnectedShowHTML()
{
  if(isMQTTConnected())
    return "<b><font color=\"#32CD32\">Подключен</font></b>";
  else
    return "<b><font color=\"#FFC300\">Отключен</font></b>";
}

String outputState(uint8_t settingId){
  if(EEPROM.read(settingId)) 
    return "checked";
  return "";
}

String createSwitch(uint8_t settingId)
{
  String formatString = "<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(settingId) + "\" "+ outputState(settingId) +">";
  Serial.println(formatString);
  return formatString;
}

void ResetShowingFlags()
{
  additionalLightShowing = false;
  readLightWarmShowing = false;
  readLightColdShowing = false;
}

String processor(const String& var)
{
  if(var == "STYLECSS") {
    Serial.println("return index_css");
    return index_css;
  }
  if(var == "VERSION") {
    Serial.print("return ");
    Serial.println(VERSION_SHORT);
    return VERSION_SHORT;
  }
  if(var == "WIFI_SSID") {
    Serial.println("return WIFI_SSID");
    return wifi_ssid;
  }
  if(var == "WIFI_PASSWORD") {
    Serial.println("return WIFI_PASSWORD");
    return wifi_password;
  }
  if(var == "SCRIPTSJS"){
    Serial.println("return scriptjs");
    return index_js;
  }
  if(var == "CURRENTIP")
  {
    return WiFi.localIP().toString();
  }
  if(var == "EFFECTID")
    return String(effectId);
  if(var == "ONOFFSWITCH")
    return createSwitch(ON_OFF_STATE_ADDRESS);
  if(var == "ADDNLSWITCH")
    return createSwitch(ADDITION_ENABLE_NIGHTLIGHT_ADDRESS);
  if(var == "LIGHTDIVSHOW")
  {
    if(light_mode == NIGHT_LIGHT)
      return "block";
    else
      return "none";
  }
  if(var == "READDIVSHOW")
  {
    if(light_mode == READ_MODE)
      return "block";
    else
      return "none";
  }
  if(var == "FLASHDIVSHOW")
  {
    if(light_mode == RAINBOW_MODE)
      return "block";
    else
      return "none";
  }
  if(var == "NIGHTRADIO" && light_mode == NIGHT_LIGHT)
    return "checked";
  if(var == "READRADIO" && light_mode == READ_MODE)
    return "checked";
  if(var == "FLASHRADIO" && light_mode == RAINBOW_MODE)
    return "checked";
#pragma region brightness
  if(var.indexOf("BR") != -1)
  {
    for (int i = 1; i <= 8; i++)
    {
      String brName = "BR" + String(i);
      if(var == brName && brightness == i)
        return "checked";
    }
  }

#pragma endregion
  if(var.indexOf("TEMP") != -1)
  {
    for (int i = 0; i <= 7; i++)
    {
      String tempName = "TEMP" + String(i);
      if(var == tempName && temperature == i)
        return "checked";
    }
  }

  if(var == "WARM_LEVEL")
    return String(warm_value);
  if(var == "COLD_LEVEL")
    return String(cold_value);
  if(var.indexOf("WL") != -1)
  {
    for (int i = 0; i <= 7; i++)
    {
      String wlName = "WL" + String(i);
      if(var == wlName && warm_value == i)
        return "checked";
    }
  }
  if(var.indexOf("CL") != -1)
  {
    for (int i = 0; i <= 7; i++)
    {
      String clName = "CL" + String(i);
      if(var == clName && cold_value == i)
        return "checked";
    }
  }

  if(var == "MQTTSTATUS")
    return isMQTTConnectedShowHTML();
#pragma region temperature
#pragma endregion
  return "";
}
#pragma endregion

#pragma region Basic Functions
void setOff()
{
  if(light_mode == RAINBOW_MODE)
    needBreakEffect = true;
  LEDS_ON = false;
  onOffChanged = true;
  ResetShowingFlags();
}

void setOn()
{
  LEDS_ON = true;
  onOffChanged = true;
}

void setMode(int mode)
{
  switch(mode)
  {
    case READ_MODE:
      needBreakEffect = true;
      light_mode = READ_MODE;
      ResetShowingFlags();
      break;
    case NIGHT_LIGHT:
      needBreakEffect = true;
      light_mode = NIGHT_LIGHT; 
      ResetShowingFlags();
      break;
    case RAINBOW_MODE:
      light_mode = RAINBOW_MODE; 
      ResetShowingFlags();
      break;
  }
  lightModeChanged = true;
}

void setBrightness(int value)
{
  brightness = value;
  brightnessChanged = true;
}

void readSettings()
{
    if(EEPROM.read(ON_OFF_STATE_ADDRESS) == 255)
    {
      EEPROM.write(ON_OFF_STATE_ADDRESS, 1);
      Serial.println("Initialized ON_OFF_STATE");
    }
    if(EEPROM.read(MODE_STATE_ADDRESS) == 255)
    {
      EEPROM.write(MODE_STATE_ADDRESS, NIGHT_LIGHT);
      Serial.println("Initialized MODE_STATE");
    }
    if(EEPROM.read(BRIGHTNESS_STATE_ADDRESS) == 255)
    {
      EEPROM.write(BRIGHTNESS_STATE_ADDRESS, 4);
      Serial.println("Initialized BRIGHTNESS");
    }
    if(EEPROM.read(TEMPERATURE_STATE_ADDRESS) == 255)
    {
      EEPROM.write(TEMPERATURE_STATE_ADDRESS, 3);
      Serial.println("Initialized TEMPERATURE");
    }
    if(EEPROM.read(EFFECT_ID_ADDRESS) < 0 || EEPROM.read(EFFECT_ID_ADDRESS) > 28)
    {
      EEPROM.write(EFFECT_ID_ADDRESS, 0);
      Serial.println("Initialized EFFECT_ID");
    }
    if(EEPROM.read(WARM_ADDRESS) == 255)
    {
      EEPROM.write(WARM_ADDRESS, 4);
      Serial.println("Initialized WARM");
    }
    if(EEPROM.read(COLD_ADDRESS) == 255)
    {
      EEPROM.write(COLD_ADDRESS, 4);
      Serial.println("Initialized COLD");
    }
    if(EEPROM.read(ADDITION_ENABLE_NIGHTLIGHT_ADDRESS) == 255)
    {
      EEPROM.write(ADDITION_ENABLE_NIGHTLIGHT_ADDRESS, 0);
      Serial.println("Initialized ADDNL");
    }
    EEPROM.commit();
    brightness = EEPROM.read(BRIGHTNESS_STATE_ADDRESS);
    Serial.println("Brightness = " + String(brightness));
    temperature = EEPROM.read(TEMPERATURE_STATE_ADDRESS);
    Serial.println("Temperature = " + String(temperature));
    light_mode = EEPROM.read(MODE_STATE_ADDRESS);
    Serial.println("Mode = " + String(light_mode));
    LEDS_ON = EEPROM.read(ON_OFF_STATE_ADDRESS);
    Serial.println("State = " + String(LEDS_ON));
    effectId = EEPROM.read(EFFECT_ID_ADDRESS);
    Serial.println("effectId = " + String(effectId));
    warm_value = EEPROM.read(WARM_ADDRESS);
    Serial.println("warm_value = " + String(warm_value));
    cold_value = EEPROM.read(COLD_ADDRESS);
    Serial.println("cold_value = " + String(cold_value));
    enableAdditionNightLight = EEPROM.read(ADDITION_ENABLE_NIGHTLIGHT_ADDRESS);
    Serial.println("enableAdditionNightLight = " + String(enableAdditionNightLight));
    //EEPROM.commit();
}

void setEffectId(int value)
{
  effectId = value;
  if(effectId > FX_COUNT)
    effectId = FX_COUNT;
  if (effectId < 0)
    effectId = 0;
  effectIdChanged = true;
    needBreakEffect = true;
}

void decrementEffect()
{
  Serial.println("effect--");
    if(effectId == 0) 
      effectId = FX_COUNT - 1;
    else
      effectId--; 
    effectIdChanged = true;
    needBreakEffect = true;
}

void incrementEffect()
{
  Serial.println("effect++");
  effectId++; if(effectId >= FX_COUNT) effectId = 0;
  effectIdChanged = true;
  needBreakEffect = true;
}

void standbyLedsUpdate()
{
  if(LEDS_ON)
  {
    digitalWrite(ON_LED_PIN, HIGH);
    digitalWrite(OFF_LED_PIN, LOW);
  }
  else
  {
    digitalWrite(ON_LED_PIN, LOW);
    digitalWrite(OFF_LED_PIN, HIGH);
  }
}

void offReadLeds()
{
  ledcWrite(LED_WARM_CHANNEL, 0);
  ledcWrite(LED_COLD_CHANNEL, 0);
}

void updateVolumeStrip()
{
  switch (light_mode)
  {
    case NIGHT_LIGHT:
      if(NIGHT_LIGHT_BRIGHTNESS_MODE)
        drawBrightness(temperature, brightness, volumeStrip);
      else
        drawTemperature(temperature, brightness, volumeStrip);
      break;
    case READ_MODE:
      break;;
    case RAINBOW_MODE:
      clearLedStrip(volumeStrip);
      break;
  }
}
#pragma endregion

#pragma region NightLight Functions
void setOnNightLight()
{
  setMode(NIGHT_LIGHT);
  LEDS_ON = true;
  onOffChanged = true;
}

void setOffNightLight()
{
  setOff();
}

String getNightLightState()
{
  return String((light_mode == NIGHT_LIGHT && LEDS_ON));
}

void setBrightnessPercent(int value)
{
  Serial.println("Set Brightness persent " + String(value));
  brightness = value * ((float)8 / (float)100);
  Serial.println("Calculated brightness " + String(brightness));
  if(brightness < 1)
    brightness = 1;
  brightnessChanged = true;
}

int getBrightnessPercent()
{
  return 100 / ((float)8 / (float)brightness);
}

void setTemperature(int value)
{
  temperature = value;
  temperatureChanged = true;
}
#pragma endregion

#pragma region ReadLight Functions
void setOnReadLight()
{
  setMode(READ_MODE);
  LEDS_ON = true;
  onOffChanged = true;
}

void setOffReadLight()
{
  setOff();
}

String getReadLightState()
{
  return String((light_mode == READ_MODE && LEDS_ON));
}

void setWarmLevel(int value)
{
  warm_value = value;
  warmChanged = true;
  readLightWarmShowing = false;
}

void setColdLevel(int value)
{
  cold_value = value;
  coldChanged = true;
  readLightColdShowing = false;
}
#pragma endregion

#pragma region FlashLight Functions
void setOnFlashLight()
{
  setMode(RAINBOW_MODE);
  LEDS_ON = true;
  onOffChanged = true;
}

void setOffFlashLight()
{
  setOff();
}

String getFlashLightState()
{
  return String((light_mode == RAINBOW_MODE && LEDS_ON));
}

void setEffectIdFlashLight(int value)
{
  setEffectId(value);
}

int getEffectIdFlashLight()
{
  return effectId;
}
#pragma endregion

#pragma region Button Functions
void clickDownButton()
{
  Serial.println("click down");
  if(light_mode == RAINBOW_MODE)
    decrementEffect();
  else if(light_mode == NIGHT_LIGHT)
  {
    if(NIGHT_LIGHT_BRIGHTNESS_MODE)
    {
      if(brightness > 1)
        brightness--;
    }
    else
    {
      if(temperature > 0)
        temperature--;
    }
  }
}

void clickUpButton()
{
  Serial.println("click up");
  if(light_mode == RAINBOW_MODE)
    incrementEffect();
  else if(light_mode == NIGHT_LIGHT)
  {
    if(NIGHT_LIGHT_BRIGHTNESS_MODE)
    {
      if(brightness < 8)
        brightness++;
    }
    else
    {
      if(temperature < 7)
        temperature++;
    }
  }
}

bool clickOnOff()
{
  if(light_mode == RAINBOW_MODE)
    needBreakEffect = true;
  LEDS_ON = !LEDS_ON;
  onOffChanged = true;
  return LEDS_ON;
}

bool switchAdditionalNL()
{
  enableAdditionNightLight = !enableAdditionNightLight;
  enableAdditionNightLightChanged = true;
  if (enableAdditionNightLight)
    offReadLeds();
  ResetShowingFlags();
  //additionalLightShowing = false;
  return enableAdditionNightLight;
}

void holdDownButton()
{
  if(light_mode == NIGHT_LIGHT)
    NIGHT_LIGHT_BRIGHTNESS_MODE = !NIGHT_LIGHT_BRIGHTNESS_MODE;
  else if(light_mode == READ_MODE)
    READ_LIGHT_WARM_MODE = !READ_LIGHT_WARM_MODE;
}

void holdUpButton()
{
  if(light_mode == READ_MODE)
  {
    enableAdditionNightLight = !enableAdditionNightLight;
    enableAdditionNightLightChanged = true;
  }
}

void holdOnOff()
{
  switch (light_mode)
  {
    case NIGHT_LIGHT:
      setMode(READ_MODE);
      break;
    case READ_MODE:
      setMode(RAINBOW_MODE);
      break;
    case RAINBOW_MODE:
      setMode(NIGHT_LIGHT);
      break;
  }
}
#pragma endregion

#pragma region Button Interrupt Functions
void IRAM_ATTR clickOnOffButtonISR()
{
  if(!onOffPressed && (digitalRead(ON_OFF_PIN) == HIGH))
  {
    onOffMillis = millis();
    onOffPressed = true;
    Serial.println("OnOff pressed");
  }
  else if (onOffPressed && (digitalRead(ON_OFF_PIN) == LOW))
  {
    onOffPressed = false;
    Serial.println("OnOff release");
    if(millis() - onOffMillis <= clickTimeOut)
    {
      Serial.println("OnOff Click");
      clickOnOff();
    }
    else
    {
      Serial.println("OnOff Hold");
      holdOnOff();
    }
  }
}

void IRAM_ATTR clickDownButtonISR()
{
  if(!downPressed && (digitalRead(VOLUME_DOWN_PIN) == HIGH))
  {
    downMillis = millis();
    downPressed = true;
    Serial.println("DOWN pressed");
  }
  else if (downPressed && (digitalRead(VOLUME_DOWN_PIN) == LOW))
  {
    downPressed = false;
    Serial.println("DOWN release");
    if(millis() - downMillis <= clickTimeOut)
    {
      Serial.println("DOWN Click");
      clickDownButton();
    }
    else
    {
      Serial.println("DOWN Hold");
      holdDownButton();
    }
  }
}

void IRAM_ATTR clickUpButtonISR()
{
  if(!upPressed && (digitalRead(VOLUME_UP_PIN) == HIGH))
  {
    upMillis = millis();
    upPressed = true;
    Serial.println("UP pressed");
  }
  else if (upPressed && (digitalRead(VOLUME_UP_PIN) == LOW))
  {
    upPressed = false;
    Serial.println("UP release");
    if(millis() - upMillis <= clickTimeOut)
    {
      Serial.println("UP Click");
      clickUpButton();
    }
    else
    {
      Serial.println("UP Hold");
      holdUpButton();
    }
  }
}
#pragma endregion

#pragma region WiFi Callbacks
void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("ESP32 WIFI Connected to Access Point");
  bool lastOnOffState = LEDS_ON;
  LEDS_ON = false;
  showConnected = true;
  needBreakEffect = true;
  for (int i = 0; i < 10; i++)
  {
    mainStrip.fill(mainStrip.ColorHSV(21000,255,15), 0, NUM_MAIN_LEDS);
    mainStrip.show();
    delay(100);
    mainStrip.clear();
    mainStrip.show();
    delay(100); 
  }
  showConnected = false;
  LEDS_ON = lastOnOffState;
}

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI Connected!");
  Serial.println("IP address of Connected WIFI: ");
  Serial.println(WiFi.localIP());
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WIFI");
  Serial.println("Reconnecting...");
  WiFi.reconnect();
}
#pragma endregion

#pragma region Mqtt messages
void onMqttMessageReceived(char *topic, byte *payload, unsigned int length)
{
  if(String(topic).indexOf("state") != -1)
    return;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
  }
  Serial.println();
  String mqtt_message = "";
  for(int i=0; i<length; i++)
    mqtt_message += (char)payload[i];
  Serial.println("Readed mqtt message: " + mqtt_message);
  if(String(topic).indexOf(topicNightLightPower) != -1)
  {
    if(mqtt_message == "1")
      setOnNightLight();
    else
      setOffNightLight();
  }
  if(String(topic).indexOf(topicSetBrightnessNightLight) != -1)
  {
    Serial.println("SetBrightness " + mqtt_message.toInt());
    setBrightnessPercent(mqtt_message.toInt());
  }
  else if(String(topic).indexOf(topicFlashLightPower) != -1) 
  {
    if(mqtt_message == "1")
      setOnFlashLight();
    else
      setOffFlashLight(); 
  }
  else if(String(topic).indexOf(topicSetEffectFlashLight) != -1)
  {
    Serial.println("SetEffect " + mqtt_message.toInt());
    setEffectIdFlashLight(mqtt_message.toInt());
  }
  else if(String(topic).indexOf(topicReadLightPower) != -1) 
  {
    if(mqtt_message == "1")
      setOnReadLight(); 
    else
      setOffReadLight();
  }
}

void publishStates()
{
  mqttClient.publish(topicGetNightLightStatus, String(getNightLightState()).c_str(), retain_flag);
  mqttClient.publish(topicGetReadLightStatus, String(getReadLightState()).c_str(), retain_flag);
  mqttClient.publish(topicGetFlashLightStatus, String(getFlashLightState()).c_str(), retain_flag);
  mqttClient.publish(topicGetNightLightBrightness, String(getBrightnessPercent()).c_str(), retain_flag);
  mqttClient.publish(topicGetEffectFlashLight, String(getEffectIdFlashLight()).c_str(), retain_flag);
}
#pragma endregion

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Init EEProm");
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("Init mainStrip");
  mainStrip.begin();
  Serial.println("Init volumeStrip");
  volumeStrip.begin();
  Serial.println("ON_LED "+ String(ON_LED_PIN) + " pin init");
  pinMode(ON_LED_PIN, OUTPUT);
  Serial.println("OFF_LED "+ String(OFF_LED_PIN) + " pin init");
  pinMode(OFF_LED_PIN, OUTPUT);
  Serial.println("Configure PWM");
  ledcSetup(LED_WARM_CHANNEL, PWM_FREQ, RESOLUTION_WARM);
  ledcSetup(LED_COLD_CHANNEL, PWM_FREQ, RESOLUTION_COLD);
  Serial.println("Attach WARM_LED_PIN "+ String(WARM_LED_PIN) + " pin init");
  ledcAttachPin(WARM_LED_PIN, LED_WARM_CHANNEL);
  Serial.println("Attach COLD_LED_PIN "+ String(COLD_LED_PIN) + " pin init");
  ledcAttachPin(COLD_LED_PIN, LED_COLD_CHANNEL);
  Serial.println("Read settings");
  readSettings();
  clearLedStrips(mainStrip, volumeStrip);
  standbyLedsUpdate();
  
#pragma region CheckVersion
  String readedVersion = EEPROM.readString(VERSION_ADDRESS);
  if(readedVersion != VERSION_SHORT)
  {
    for (int i = 0; i < 10; i++)
    {
      mainStrip.fill(mainStrip.ColorHSV(0,255,15), 0, NUM_MAIN_LEDS);
      mainStrip.show();
      delay(100);
      mainStrip.clear();
      mainStrip.show();
      delay(100); 
    }
    EEPROM.writeString(VERSION_ADDRESS, VERSION_SHORT);
    EEPROM.commit();
  }
#pragma endregion  
  
#pragma region Interrupts
  attachInterrupt(digitalPinToInterrupt(ON_OFF_PIN), clickOnOffButtonISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(VOLUME_DOWN_PIN), clickDownButtonISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(VOLUME_UP_PIN), clickUpButtonISR, CHANGE);
#pragma endregion

#pragma region WiFi
  String ssid = "ESP32_SL_"+WiFi.macAddress();

  WiFi.mode(WIFI_MODE_APSTA);
  Serial.println("\n[*] Creating ESP32 AP");
  WiFi.softAP(ssid.c_str(), "Mamapapa15");  /*Configuring ESP32 access point SSID and password*/
  Serial.print("[+] AP Created with IP Gateway ");
  Serial.println(WiFi.softAPIP());     /*Printing the AP IP address*/

  wifi_ssid = EEPROM.readString(SSID_ADDRESS);
  wifi_password = EEPROM.readString(PASSWORD_ADDRESS);

  WiFi.onEvent(Wifi_connected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(Get_IPAddress, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(Wifi_disconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
#pragma endregion

#pragma region Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/update_setting", HTTP_GET, [](AsyncWebServerRequest *request){
    int setting_id;
    int setting_value;
    if (request->hasParam(SETTING_ID) && request->hasParam(SETTING_VALUE)) {
      setting_id = request->getParam(SETTING_ID)->value().toInt();
      setting_value = request->getParam(SETTING_VALUE)->value().toInt();
      String result = "";
      switch (setting_id)
      {
        case ON_OFF_STATE_ADDRESS: result = String(clickOnOff()); break;
        case ADDITION_ENABLE_NIGHTLIGHT_ADDRESS: result = String(switchAdditionalNL()); break;
      }
      request->send(200, "text/plain", result);
    }
    else
      request->send(400, "text/plain", "Bad request: Bad parameters received");
  });
  server.on("/update_wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid") && request->hasParam("password")) {
      wifi_ssid = request->getParam("ssid")->value();
      wifi_password = request->getParam("password")->value();
      EEPROM.writeString(SSID_ADDRESS, wifi_ssid);
      EEPROM.writeString(PASSWORD_ADDRESS, wifi_password);
      EEPROM.commit();
      WiFi.disconnect();
      WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
      request->send(200, "text/plain", "OK");
    }
    request->send(400, "text/plain", "Bad request: Bad parameters receive");
  });
  server.on("/getEffectId", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(effectId));
  });
  server.on("/increment_effect", HTTP_GET, [](AsyncWebServerRequest *request) {
    incrementEffect();
    request->send(200, "text/plain", "OK");
  });
  server.on("/decrement_effect", HTTP_GET, [](AsyncWebServerRequest *request) {
    decrementEffect();
    request->send(200, "text/plain", "OK");
  });
  server.on("/set_mode", HTTP_GET, [](AsyncWebServerRequest *request) {
    int mode;
    if(request->hasParam("mode"))
    {
      mode = request->getParam("mode")->value().toInt();
      setMode(mode);
      request->send(200, "text/plain", "OK");
    }
    else
      request->send(400, "text/plain", "Bad request: Bad parameters received");
  });
  server.on("/set_brightness", HTTP_GET, [](AsyncWebServerRequest *request){
    int value;
    if(request->hasParam("value"))
    {
      value = request->getParam("value")->value().toInt();
      setBrightness(value);
      request->send(200, "text/plain", "OK");
    }
    else
      request->send(400, "text/plain", "Bad request: Bad parameters received");
  });
  server.on("/set_temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    int value;
    if(request->hasParam("value"))
    {
      value = request->getParam("value")->value().toInt();
      setTemperature(value);
      request->send(200, "text/plain", "OK");
    }
    else
      request->send(400, "text/plain", "Bad request: Bad parameters received");
  });
  server.on("/setWarmLevel", HTTP_GET, [](AsyncWebServerRequest *request){
    int value;
    if(request->hasParam("value"))
    {
      value = request->getParam("value")->value().toInt();
      setWarmLevel(value);
      request->send(200, "text/plain", "OK");
    }
    else
      request->send(400, "text/plain", "Bad request: Bad parameters received");
  });
  server.on("/setColdLevel", HTTP_GET, [](AsyncWebServerRequest *request){
    int value;
    if(request->hasParam("value"))
    {
      value = request->getParam("value")->value().toInt();
      setColdLevel(value);
      request->send(200, "text/plain", "OK");
    }
    else
      request->send(400, "text/plain", "Bad request: Bad parameters received");
  });
  server.on("/setReadBrightness", HTTP_GET, [](AsyncWebServerRequest *request)
  {

  });
  AsyncElegantOTA.begin(&server);
  server.begin();
#pragma endregion

}

void loop() 
{
  #pragma region update settings in EEPROM
  if(onOffChanged)
  {
    EEPROM.write(ON_OFF_STATE_ADDRESS, LEDS_ON);
    EEPROM.commit();
    onOffChanged = false;
    publishStates();
  }
  if(lightModeChanged)
  {
    EEPROM.write(MODE_STATE_ADDRESS, light_mode);
    EEPROM.commit();
    lightModeChanged = false;
    publishStates();
  }
  if(brightnessChanged)
  {
    EEPROM.write(BRIGHTNESS_STATE_ADDRESS, brightness);
    EEPROM.commit();
    brightnessChanged = false;
    publishStates();
  }
  if(temperatureChanged)
  {
    EEPROM.write(TEMPERATURE_STATE_ADDRESS, temperature);
    EEPROM.commit();
    temperatureChanged = false;
    publishStates();
  }
  if(effectIdChanged)
  {
    EEPROM.write(EFFECT_ID_ADDRESS, effectId);
    EEPROM.commit();
    effectIdChanged = false;
    publishStates();
  }
  if(warmChanged)
  {
    EEPROM.write(WARM_ADDRESS, warm_value);
    EEPROM.commit();
    warmChanged = false;
    publishStates();
  }
  if(coldChanged)
  {
    EEPROM.write(COLD_ADDRESS, cold_value);
    EEPROM.commit();
    coldChanged = false;
    publishStates();
  }
  if(enableAdditionNightLightChanged)
  {
    EEPROM.write(ADDITION_ENABLE_NIGHTLIGHT_ADDRESS, enableAdditionNightLight);
    EEPROM.commit();
    enableAdditionNightLightChanged = false;
    publishStates();
  }
#pragma endregion

  #pragma region MQTT Client connect and reconnect

  if(mqtt_server != "")
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      if(!mqttClient.connected())
      { 
        #pragma region Init MQTT
        if(!mqttInitialized)
        {
          Serial.println("MQTT setServer: " + mqtt_server + " port: " + String(mqtt_server_port));
          mqttClient.setServer(mqtt_server.c_str(), mqtt_server_port);
          Serial.print("MQTT setCallback");
          mqttClient.setCallback(onMqttMessageReceived);
          mqttInitialized = true;
        }
        #pragma endregion

        Serial.println("MQTT connecting ClientID: " + mqtt_client_id + " User: " + mqtt_server_user + " Password: " + mqtt_server_password);
        bool connectionResult = mqttClient.connect(mqtt_client_id.c_str(), mqtt_server_user.c_str(), mqtt_server_password.c_str());
        Serial.println("MQTT connection result: " + String(connectionResult));
        if(connectionResult)
        {
          bool subscribeResult = false;
          if(!useTopicPrefix)
          {
            subscribeResult = mqttClient.subscribe(topic);
            Serial.println("Topic: " + String(topic) + " Subcribe result: " + String(subscribeResult));
          }
          else
          {
            subscribeResult = mqttClient.subscribe((mqtt_topic_prefix + String(topic)).c_str());
            Serial.println("Topic: " + mqtt_topic_prefix + String(topic) + " Subcribe result: " + String(subscribeResult));
          }
        }
      }
      else
      {
        mqttClient.loop();
        if (millis() - publishTimer >= (sending_period * 1000)) {
          publishTimer = millis();
          publishStates();
        }
      }
    }
  }
  #pragma endregion
  
  #pragma region Main cicle work lamp
  standbyLedsUpdate();
  if(LEDS_ON)
  {
    updateVolumeStrip();
    switch (light_mode)
    {
      case RAINBOW_MODE:
        Serial.print("show_fx. effectId = ");
        Serial.println(effectId);
        show_fx(mainStrip, effectId);
        offReadLeds();
        break;
      case NIGHT_LIGHT:
        drawNightLight(temperature, brightness, mainStrip);
        offReadLeds();
        break;
      case READ_MODE:
        if(enableAdditionNightLight)
        {
          if(!additionalLightShowing)
          {
            drawNightLight(temperature, brightness, mainStrip);
            additionalLightShowing = true;
          }
        }
        else
        {
          clearLedStrips(mainStrip, volumeStrip);
          additionalLightShowing = false;
        }
        if(!readLightWarmShowing)
        {
          delay(100);
          ledcWrite(LED_WARM_CHANNEL, 256 / 8 * warm_value);
          readLightWarmShowing = true;
        }
        if(!readLightColdShowing)
        {
          delay(100);
          ledcWrite(LED_COLD_CHANNEL, 256 / 8 * cold_value);
          readLightColdShowing = true;
        }
        break;
    }
  }
  else
  {
    offReadLeds();
    readLightColdShowing = false;
    readLightWarmShowing = false;
    additionalLightShowing = false;
    if(!showConnected)
      clearLedStrips(mainStrip, volumeStrip);
  }
  #pragma endregion
}