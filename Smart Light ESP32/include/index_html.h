#include "Arduino.h"

#pragma region index_page
const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML><html>
    <head>
        <title>ESP Smart Light</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <meta charset="utf-8">
        <link rel="icon" href="data:,">
        %STYLECSS%
    </head>
    <body>
        <h2>ESP Smart Light</h2>
        <h3>Настройки</h3>
        <a href="/update"><b><font color="#FF0000">Обновление</font></b></a>
		<h3>Версия %VERSION%<h3>
		<h3>Подключeние к сети WIFI</h3>
		<table align="center" class="settingstable">
			<tr>
				<td class="texttd"><b>SSID:</b></td>
				<td class="texttd"><input type="text" id="ssid" value="%WIFI_SSID%" size="15" maxlength="32"></td>
			</tr>
			<tr>
				<td class="texttd"><b>Пароль:</b></td>
				<td class="texttd"><input type="password" id="pass" value="%WIFI_PASSWORD%" size="15" maxlength="32"></td>
			</tr>
			<tr>
				<td class="texttd"><b>Текущий адрес:</b></td>
				<td class="texttd"><b>%CURRENTIP%</b></td>
			</tr>
		</table>
		<br><input class="button-24" type="button" value="Применить" onclick="updateWiFiSettings()">

		<h3>Подключeние к MQTT</h3>
		<table align="center" class="settingstable panel">
			<tr>
				<td class="texttd"><b>Название устройства (лат.):</b></td>
				<td class="texttd"><input type="text" id="device_topic" value="%DEVICE_TOPIC%" size="20" maxlength="32"></td>
			</tr>
			<tr>
				<td class="texttd"><font size="2"><i>Добавляется к названию прослушиваемых топиков</i></font></td>
			</tr>
			<tr>
				<td class="texttd"><b>Сервер:</b></td>
				<td class="texttd"><input type="text" id="mqtt_server" value="%MQTT_SERVER%" size="20" maxlength="32"></td>
			</tr>
			<tr>
				<td class="texttd"><b>Порт сервера:</b></td>
				<td class="texttd"><input type="text" id="mqtt_server_port" value="%MQTT_SERVER_PORT%" size="20" maxlength="5"></td>
			</tr>
			<tr>
				<td class="texttd"><b>Пользователь:</b></td>
				<td class="texttd"><input type="text" id="mqtt_user" value="%MQTT_USER%" size="20" maxlength="32"></td>
			</tr>
			<tr>
				<td class="texttd"><b>Пароль:</b></td>
				<td class="texttd"><input type="password" id="mqtt_pass" value="%MQTT_PASSWORD%" size="20" maxlength="32"></td>
			</tr>
			<tr>
				<td class="texttd"><b>Статус подключения:</b></td>
				<td class="texttd">%MQTTSTATUS%</td>
			</tr>
		</table>
		<br><input class="button-24" type="button" value="Применить" onclick="updateMQTTSettings()">

		<br><br><table align="center" class="settingstable">
			<tr>
				<td class="texttd"><b>Состояние вкл/выкл</b></td>
				<td class="controltd">
					<label class="switch">
						%ONOFFSWITCH%
						<span class="slider"></span>
					</label>
				</td>
			</tr>
		</table>

		<form>
		 <div class="radio-group">
		  <input type="radio" id="option-one" name="selector" onclick="clickLight()" %NIGHTRADIO%><label class="label_radio" for="option-one">Ночник</label><input type="radio" id="option-two" name="selector" onclick="clickRead()" %READRADIO%><label class="label_radio" for="option-two">Чтение</label><input type="radio" id="option-three" name="selector" onclick="clickFlash()" %FLASHRADIO%><label class="label_radio" for="option-three">Гирлянда</label>
		 </div>
		</form>
		<div id="lightDiv" class="hideDiv" style="display:%LIGHTDIVSHOW%">
			<h3>Яркость</h3>
			<form>
				<div class="brightness-group">
					<input type="radio" id="br1" name="brightness_selector" onclick="clickBrightness(1)" %BR1%><label class="brightness_radio" for="br1">1</label>
					<input type="radio" id="br2" name="brightness_selector" onclick="clickBrightness(2)" %BR2%><label class="brightness_radio" for="br2">2</label>
					<input type="radio" id="br3" name="brightness_selector" onclick="clickBrightness(3)" %BR3%><label class="brightness_radio" for="br3">3</label>
					<input type="radio" id="br4" name="brightness_selector" onclick="clickBrightness(4)" %BR4%><label class="brightness_radio" for="br4">4</label>
					<input type="radio" id="br5" name="brightness_selector" onclick="clickBrightness(5)" %BR5%><label class="brightness_radio" for="br5">5</label>
					<input type="radio" id="br6" name="brightness_selector" onclick="clickBrightness(6)" %BR6%><label class="brightness_radio" for="br6">6</label>
					<input type="radio" id="br7" name="brightness_selector" onclick="clickBrightness(7)" %BR7%><label class="brightness_radio" for="br7">7</label>
					<input type="radio" id="br8" name="brightness_selector" onclick="clickBrightness(8)" %BR8%><label class="brightness_radio" for="br8">8</label>
				</div>
			</form>
			<h3>Температура, К</h3>
			<form>
				<div class="brightness-group">
					<input type="radio" id="temp0" name="temp_selector" onclick="clickTemp(0)" %TEMP0%><label class="brightness_radio" for="temp0">1</label>
					<input type="radio" id="temp1" name="temp_selector" onclick="clickTemp(1)" %TEMP1%><label class="brightness_radio" for="temp1">2</label>
					<input type="radio" id="temp2" name="temp_selector" onclick="clickTemp(2)" %TEMP2%><label class="brightness_radio" for="temp2">3</label>
					<input type="radio" id="temp3" name="temp_selector" onclick="clickTemp(3)" %TEMP3%><label class="brightness_radio" for="temp3">4</label>
					<input type="radio" id="temp4" name="temp_selector" onclick="clickTemp(4)" %TEMP4%><label class="brightness_radio" for="temp4">5</label>
					<input type="radio" id="temp5" name="temp_selector" onclick="clickTemp(5)" %TEMP5%><label class="brightness_radio" for="temp5">6</label>
					<input type="radio" id="temp6" name="temp_selector" onclick="clickTemp(6)" %TEMP6%><label class="brightness_radio" for="temp6">7</label>
					<input type="radio" id="temp7" name="temp_selector" onclick="clickTemp(7)" %TEMP7%><label class="brightness_radio" for="temp7">8</label>
				</div>
			</form>
		</div>
		<div id="readDiv" class="hideDiv" style="display:%READDIVSHOW%">
			<h3>Интенсивность теплого света</h3>
			<form>
				<div class="brightness-group">
					<input type="radio" id="wl0" name="warm_level_selector" onclick="setWarmLevel(0)" %WL0%><label class="brightness_radio" for="wl0">0</label>
					<input type="radio" id="wl1" name="warm_level_selector" onclick="setWarmLevel(1)" %WL1%><label class="brightness_radio" for="wl1">1</label>
					<input type="radio" id="wl2" name="warm_level_selector" onclick="setWarmLevel(2)" %WL2%><label class="brightness_radio" for="wl2">2</label>
					<input type="radio" id="wl3" name="warm_level_selector" onclick="setWarmLevel(3)" %WL3%><label class="brightness_radio" for="wl3">3</label>
					<input type="radio" id="wl4" name="warm_level_selector" onclick="setWarmLevel(4)" %WL4%><label class="brightness_radio" for="wl4">4</label>
					<input type="radio" id="wl5" name="warm_level_selector" onclick="setWarmLevel(5)" %WL5%><label class="brightness_radio" for="wl5">5</label>
					<input type="radio" id="wl6" name="warm_level_selector" onclick="setWarmLevel(6)" %WL6%><label class="brightness_radio" for="wl6">6</label>
					<input type="radio" id="wl7" name="warm_level_selector" onclick="setWarmLevel(7)" %WL7%><label class="brightness_radio" for="wl7">7</label>
				</div>
			</form>
			<h3>Интенсивность холодного света</h3>
			<form>
				<div class="brightness-group">
					<input type="radio" id="cl0" name="cold_level_selector" onclick="setColdLevel(0)" %CL0%><label class="brightness_radio" for="cl0">0</label>
					<input type="radio" id="cl1" name="cold_level_selector" onclick="setColdLevel(1)" %CL1%><label class="brightness_radio" for="cl1">1</label>
					<input type="radio" id="cl2" name="cold_level_selector" onclick="setColdLevel(2)" %CL2%><label class="brightness_radio" for="cl2">2</label>
					<input type="radio" id="cl3" name="cold_level_selector" onclick="setColdLevel(3)" %CL3%><label class="brightness_radio" for="cl3">3</label>
					<input type="radio" id="cl4" name="cold_level_selector" onclick="setColdLevel(4)" %CL4%><label class="brightness_radio" for="cl4">4</label>
					<input type="radio" id="cl5" name="cold_level_selector" onclick="setColdLevel(5)" %CL5%><label class="brightness_radio" for="cl5">5</label>
					<input type="radio" id="cl6" name="cold_level_selector" onclick="setColdLevel(6)" %CL6%><label class="brightness_radio" for="cl6">6</label>
					<input type="radio" id="cl7" name="cold_level_selector" onclick="setColdLevel(7)" %CL7%><label class="brightness_radio" for="cl7">7</label>
				</div>
			</form>
			<table align="center" class="settingstable">
				<tr>
					<td class="texttd"><b>Дополнительная подсветка вкл/выкл</b></td>
					<td class="controltd">
						<label class="switch">
							%ADDNLSWITCH%
							<span class="slider"></span>
						</label>
					</td>
				</tr>
			</table>
		</div>
		<div id="flashDiv" class="hideDiv" style="display:%FLASHDIVSHOW%">
			<h3>Переключение эффектов</h3>
			<table align="center" class="settingstable">
				<tr>
					<td class="texttd">Эффект:</td>
					<td class="texttd" width="10"><input class="button-24" type="button" value="&lt;" onclick="decrementEffect()"></td>
					<td class="controltd"><div id="effectIdText">%EFFECTID%</div></td>
					<td class="texttd" width="10"><input class="button-24" type="button" value="&gt;" onclick="incrementEffect()"></td>
				</tr>
			</tbody></table>
		<div>
		%SCRIPTSJS%
    </body>
)rawliteral";
#pragma endregion

// <h3>Консоль MQTT:</h3>
// 		<textarea rows="10" cols="60" id="mqtt_console"></textarea>

#pragma region CSS
const char index_css[] PROGMEM = R"rawliteral(
<style>

@import url("https://fonts.googleapis.com/css?family=Lato");

html {
	font-family: Arial; 
	display: inline-block; 
	text-align: center;
}

body {
	font-family: Lato, Arial;
	color: #fff;
	padding: 20px;
	background-color: #e74c3c;
	background-image: url('data:image/gif;base64,R0lGODlhBQAFALMAAP///xcXFwsMDQ0OEAkJDBAREwAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAGQAAAAAACH5BAEAAAAALAAAAAAFAAUAAAQLkIRZZiDyVj3JDhEAOw==');
}

h1 {
	font-weight: normal;
	font-size: 40px;
	font-weight: normal;
	text-transform: uppercase;
}
h1 span {
	font-size: 13px;
	display: block;
	padding-left: 4px;
}

h2 {
	font-size: 1.5rem;
}

p {
	margin-top: 200px;
}
p a {
	text-transform: uppercase;
	text-decoration: none;
	display: inline-block;
	color: #fff;
	padding: 5px 10px;
	margin: 0 5px;
	background-color: #b83729;
	-moz-transition: all 0.2s ease-in;
	-o-transition: all 0.2s ease-in;
	-webkit-transition: all 0.2s ease-in;
	transition: all 0.2s ease-in;
}
p a:hover {
	background-color: #ab3326;
}
.switch {
	position: relative; 
	display: inline-block; 
	width: 60px; 
	height: 38px
	} 
.switch input {
	display: none
	}
.switch input:disabled+.slider {
	background-color: rgb(107, 0, 0); 
	}
.slider {
	position: absolute; 
	top: 0; 
	left: 0; 
	right: 0; 
	bottom: 0; 
	background-color: #e74c3c; 
	border-radius: 6px
	}
.slider:before {
	position: absolute; 
	content: ""; 
	height: 26px; 
	width: 26px; 
	left: 6px; 
	bottom: 6px; 
	background-color: #fff; 
	-webkit-transition: .4s; 
	transition: .4s; 
	border-radius: 3px
	}
input:checked+.slider {
	background-color: #b30000
	}
input:checked+.slider:before {
	-webkit-transform: translateX(22px); 
	-ms-transform: translateX(22px); 
	transform: translateX(22px)
	}
.settingstable {
	width: 450px;
	}
.controltd {
	padding-left: 10px
	}
.comboboxtd {
	padding-left: 10px
	}
.combobox {
	height: 38px;
	width: 245px;
	background-color: #b30000;
	color: #fff;
	border-radius: 7px;
	border-color: #b30000;
	}
.texttd {
	text-align: left;
	}
.button-24 {
	background: #b30000;
	border: 1px solid #b30000;
	border-radius: 6px;
	box-shadow: rgba(0, 0, 0, 0.1) 1px 2px 4px;
	box-sizing: border-box;
	color: #FFFFFF;
	cursor: pointer;
	display: inline-block;
	font-family: nunito,roboto,proxima-nova,"proxima nova",sans-serif;
	font-size: 16px;
	font-weight: 800;
	line-height: 16px;
	min-height: 40px;
	outline: 0;
	padding: 12px 14px;
	text-align: center;
	text-rendering: geometricprecision;
	text-transform: none;
	user-select: none;
	-webkit-user-select: none;
	touch-action: manipulation;
	vertical-align: middle;
}

.button-24:hover,
.button-24:active {
	background-color: initial;
	background-position: 0 0;
	color: #e74c3c;
	border: 1px solid #e74c3c;
}

.button-24:active {
  	opacity: .5;
}

input[type=radio] {
	position: absolute;
	visibility: hidden;
	display: none;
}

.label_radio {
	color: #b30000;
	display: inline-block;
	cursor: pointer;
	font-weight: bold;
	padding: 5px 20px;
}

input[type=radio]:checked + .label_radio{
	color: #FFFFFF;
	background: #b3000033;
}

.label_radio + input[type=radio] + .label_radio {
  	border-left: solid 3px #b3000033;
}
.radio-group {
	border: solid 3px #b3000033;
	display: inline-block;
	margin: 20px;
	border-radius: 10px;
	overflow: hidden;
}

.brightness_radio {
	color: #b30000;
	display: inline-block;
	cursor: pointer;
	font-weight: bold;
	padding: 5px 20px;
}

input[type=radio]:checked + .brightness_radio{
	color: #FFFFFF;
	background: #b3000033;
}

.brightness_radio + input[type=radio] + .brightness_radio {
	border-left: solid 3px #b3000033;
	border-right: solid 3px #b3000033;
}
.brightness-group {
	border: solid 3px #b3000033;
	display: inline-block;
	margin: 20px;
	margin-top: 0px;
	border-radius: 10px;
	overflow: hidden;
}

.hideDiv {
	display: none;
}

.slide2container {
  width: 100%;
}

.slider2 {
  -webkit-appearance: none;
  width: 100%;
  height: 25px;
  background: #d3d3d3;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}

.slider2:hover {
  opacity: 1;
}

.slider2::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 25px;
  height: 25px;
  background: #04AA6D;
  cursor: pointer;
}

.slider2::-moz-range-thumb {
  width: 25px;
  height: 25px;
  background: #04AA6D;
  cursor: pointer;
}

  </style>
)rawliteral";
#pragma endregion

#pragma region JS
const char index_js[] PROGMEM = R"rawliteral(
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
<script type="text/javascript">

if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events got Connected!");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events got Disconnected!");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
}

	function setWarmLevel(value)
	{
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/setWarmLevel?value="+value, true);
		xhr.send();
	}
	function setColdLevel(value)
	{
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/setColdLevel?value="+value, true);
		xhr.send();
	}

	function clickBrightness(value){
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/set_brightness?value="+value, true);
		xhr.send();
	}

	function clickTemp(value){
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/set_temperature?value="+value, true);
		xhr.send();
	}

	function clickLight(){
		document.getElementById('lightDiv').style.display = 'block';
		document.getElementById('readDiv').style.display ='none';
		document.getElementById('flashDiv').style.display ='none';
		setMode(0);
	}
	function clickRead(){
		document.getElementById('readDiv').style.display = 'block';
		document.getElementById('lightDiv').style.display ='none';
		document.getElementById('flashDiv').style.display ='none';
		setMode(2);
	}
	function clickFlash(){
		document.getElementById('flashDiv').style.display = 'block';
		document.getElementById('readDiv').style.display ='none';
		document.getElementById('lightDiv').style.display ='none';
		setMode(1);
	}

	function setMode(mode){
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/set_mode?mode="+mode, true);
		xhr.send();
	}

	function toggleCheckbox(element) {
		var xhr = new XMLHttpRequest();
		if(element.checked) {
			xhr.open("GET", "/update_setting?settingId="+element.id+"&value=1", true);
		}
		else { 
			xhr.open("GET", "/update_setting?settingId="+element.id+"&value=0", true); 
		}
		xhr.send();
	}

	function getEffectId()
	{
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if (xhr.status == 200)
				document.getElementById("effectIdText").innerHTML = xhr.responseText;
		}
		xhr.open("GET", "/getEffectId", true);
		xhr.send();
	}

	function updateMQTTSettings(){
		var mqtt_client_id = document.getElementById("device_topic").value;
		var mqtt_server_address = document.getElementById("mqtt_server").value;
		var mqtt_server_port = document.getElementById("mqtt_server_port").value;
		var mqtt_server_user = document.getElementById("mqtt_user").value;
		var mqtt_server_password = document.getElementById("mqtt_pass").value;
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/update_mqtt?mqtt_client_id="+mqtt_client_id+"&mqtt_server_address="+mqtt_server_address+"&mqtt_server_port="+mqtt_server_port+"&mqtt_server_user="+mqtt_server_user+"&mqtt_server_password="+mqtt_server_password, true);
		xhr.send();
	}

	function updateWiFiSettings() {
		var ssid_string = document.getElementById("ssid").value;
		var password_string = document.getElementById("pass").value;
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 400)
				alert(xhr.contentText);
		}
		xhr.open("GET", "/update_wifi?ssid="+ssid_string+"&password="+password_string, true);
		xhr.send();
	}
	function decrementEffect() {
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 200)
				getEffectId();
		}
		xhr.open("GET", "/decrement_effect", true);
		xhr.send();
	}
	function incrementEffect() {
		var xhr = new XMLHttpRequest();
		xhr.onload = function() {
			if(xhr.status == 200)
				getEffectId();
		}
		xhr.open("GET", "/increment_effect", true);
		xhr.send();
	}
</script>
)rawliteral";
#pragma endregion