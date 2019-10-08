#include "http.h"
#include "Configuration.h"
#include "ArduinoConnection.h"
#include "../IPC/Message.h"
#include <WString.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoOTA.h>
#include <uCRC16Lib.h>

using namespace std::placeholders;

void array_to_string(const uint8_t array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*3+0] = ' ';
        buffer[i*3+1] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*3+2] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*3] = '\0';
}

esp::Http& esp::Http::instance() {
  static esp::Http __instance;
  return __instance;
}

esp::Http::Http() {
    this->arduinoVersion = new char[1] { 0 };
    this->server = new ESP8266WebServer();
    this->server->on("/", [this]() { this->index(); });
    this->server->on("/api/updateUnitId", HTTP_POST, [this]() { this->handleUnitIdUpdate(); });
    this->server->on("/api/updateSsid", HTTP_POST, [this]() { this->handleSsidUpdate(); });
    this->server->on("/api/updatePwd", HTTP_POST, [this]() { this->handlePwdUpdate(); });
    this->server->on("/api/updateSd", HTTP_POST, [this]() { this->handleSiteDataUpdate(); });
    this->server->on("/api/applyWiFi", HTTP_POST, [this]() { this->handleApplyWiFi(); });
    this->server->on("/api/restartESP", HTTP_POST, [this]() { this->handleRestart(false); });
    this->server->on("/api/resetESP", HTTP_POST, [this]() {
      wifi_station_disconnect();
      ESP.eraseConfig();
      this->handleRestart(false);
    });

    this->server->on("/api/restartArduino", HTTP_POST, [this]() { this->handleRestart(true); });
    this->server->onNotFound([this]() { this->handleNotFound(); });
    this->updateServer = new ESP8266HTTPUpdateServer(80);
    this->updateServer->setup(this->server);
    this->webSocket = new WebSocketsServer(81);
    this->webSocket->begin();
    this->webSocket->onEvent(std::bind(&Http::webSocketEvent, this, _1, _2, _3, _4));
    this->server->begin();
    ArduinoOTA.begin();
}

esp::Http::~Http() {}

void esp::Http::setup() {
}

void esp::Http::loop() {
  ArduinoOTA.handle();
  this->webSocket->loop();
  this->server->handleClient();
}

void esp::Http::log(const String& string) {
  String ss = string;
  this->webSocket->broadcastTXT(ss);
}

void esp::Http::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT){
    String incoming;
    for(size_t i = 0; i < length; i++) incoming += ((char) payload[i]);
    this->log(incoming);
  }
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define VERSION_STRING TOSTRING(VERSION_RAW)

void esp::Http::index() {
  auto ip = WiFi.localIP();
  if (ip[0] == 0) {
    ip = WiFi.softAPIP();
  }
  String ip_str = String(ip[0]) + "." + ip[1] + "." + ip[2] + "." + ip[3];
  String html = "<html>\n";
  html += "<script>\n";
  html += "function postIt(elem, url) {\n";
  html += "  var xhr = new XMLHttpRequest();\n";
  html += "  xhr.open('post', url);\n";
  html += "  xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\n";
  html += "  var input = document.getElementById(elem);\n";
  html += "  xhr.send(elem + '=' + encodeURIComponent(input.value));\n";
  html += "}\n";
  html += "function start() {";
  html += "  var connection = new WebSocket('ws://";
  html += ip_str;
  html += ":81');";
  html += "  connection.onmessage = function (e) {\n";
  html += "    console.log('Server: ', e.data);\n";
  html += "  };";
  html += "  connection.onclose = function() {";
  html += "    start();";
  html += "  };";
  html += "}";
  html += "start();";
  html += "</script>\n";
  html += "<body><h1>IPOCS Configuration Tool</h1><br />\n";
  html += "Arduino Software Version: ";
  html += this->arduinoVersion;
  html += "<br />\n";
  html += "ESP8266 Software Version: ";
  html += VERSION_STRING;
  html += "<br />\n";
  html += "<table>\n";
  html += "<tr><th>UnitID</th><td><input id='unitId' value='";
  html += Configuration::getUnitID();
  html += "'><button onClick='postIt(\"unitId\", \"/api/updateUnitId\");'>Update</button></td></tr>\n";
  html += "<tr><th>SSID</th><td><input id='ssid' value='";
  char ssid[33];
  Configuration::getSSID(ssid);
  html += String(ssid);
  html +="'><button onClick='postIt(\"ssid\", \"/api/updateSsid\");'>Update</button></td></tr>\n";
  html += "<tr><th>PWD</th><td><input id='pwd' value='";
  char password[61];
  Configuration::getPassword(password);
  html += String(password);
  html += "'><button onClick='postIt(\"pwd\", \"/api/updatePwd\");'>Update</button></td></tr>\n";
  uint8_t siteData[200];
  uint8_t siteDataLength = Configuration::getSD(siteData, 200);
  char str[700];
  array_to_string(siteData, siteDataLength, str);
  html += "<tr><th>Site Data</th><td><input id='sd' value='";
  String sdStr = str;
  sdStr.trim();
  html += sdStr;
  html += "'><button onClick='postIt(\"sd\", \"/api/updateSd\");'>Update</button></td></tr>\n";
  html += "<tr><th>&nbsp;</th><td><input id='hidden' type='hidden' value=''><button onClick='postIt(\"hidden\", \"/api/applyWiFi\");'>Apply WiFi changes</button></td></tr>\n";
  html += "<tr><th>&nbsp;</th><td><input id='hidden' type='hidden' value=''><button onClick='postIt(\"hidden\", \"/api/restartESP\");'>Restart ESP</button></td></tr>\n";
  html += "<tr><th>&nbsp;</th><td><button onClick='postIt(\"hidden\", \"/api/restartArduino\");'>Restart Arduino</button></td></tr>\n";
  html += "<tr><th>&nbsp;</th><td><input id='hidden' type='hidden' value=''><button onClick='postIt(\"hidden\", \"/api/resetESP\");'>Reset WiFi</button> (WARNING: This erases WiFi configuration)</td></tr>\n";
  html += "</table>\n";
  html += "<a href='/update'>Update</a>";
  html += "</body></html>\n";
  this->server->send(200, "text/html", html);
}

void esp::Http::handleApplyWiFi() {
  char ssid[33];
  Configuration::getSSID(ssid);
  char password[61];
  Configuration::getPassword(password);
  esp::Http::instance().log("Connecting to SSID: " + String(ssid));
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void esp::Http::handleUnitIdUpdate() {
  this->log("New Unit ID: " + this->server->arg("unitId"));
  Configuration::setUnitID(this->server->arg("unitId").toInt());
  this->server->send(200);
}

void esp::Http::handleSsidUpdate() {
  String ssid = this->server->arg("ssid");
  this->log("New SSID: " + ssid);
  Configuration::setSSID(ssid.c_str());
  this->server->send(200);
}

void esp::Http::handlePwdUpdate() {
  String pwd = this->server->arg("pwd");
  this->log("New PWD: " + pwd);
  Configuration::setPassword(pwd.c_str());
  this->server->send(200);
}

void esp::Http::handleRestart(bool restartArduino) {
  if (!restartArduino) {
    ESP.reset();
  } else {
    this->log("Resetting arduino");
    IPC::Message *message = IPC::Message::create();
    message->RT_TYPE = IPC::RESTART;
    message->setPayload();
    ArduinoConnection::instance().send(message);
    delete message;
  }
  this->server->send(200);
}

void esp::Http::handleSiteDataUpdate() {
  this->log("New SiteData: " + this->server->arg("sd"));

  String command = this->server->arg("sd");
  byte sd[200];
  int sdLength = 0;
  while (command.length() != 0)
  {
    int spPos = command.indexOf(' ');
    if (spPos == -1)
      spPos = 2;
    String byteStr = command.substring(0, spPos);
    command.remove(0, spPos + 1);
    sd[sdLength++] = strtol(byteStr.c_str(), NULL, 16);
  }
  Configuration::setSD(sd, sdLength);
  
  uint16_t crc = uCRC16Lib::calculate(sd, sdLength);
  this->log("CRC Length: " + String(sdLength));
  this->log("CRC: " + String(crc));
  this->server->send(200);
}

void esp::Http::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += this->server->uri();
  message += "\nMethod: ";
  message += (this->server->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += this->server->args();
  message += "\n";
  for (uint8_t i=0; i<this->server->args(); i++){
    message += " " + this->server->argName(i) + ": " + this->server->arg(i) + "\n";
  }
  this->server->send(404, "text/plain", message);
}

void esp::Http::setArduinoVersion(uint8_t* arduinoVersion) {
  delete this->arduinoVersion;
  this->arduinoVersion = new char[strlen((const char*)arduinoVersion) + 1];
  memcpy(this->arduinoVersion, arduinoVersion, strlen((const char*)arduinoVersion) + 1);
}