#include "http.h"
#include "Configuration.h"
#include "ArduinoConnection.h"
#include "../IPC/Message.h"
#include <WString.h>
#include <uCRC16Lib.h>
#include "ArduinoFlash.h"
#include <LittleFS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "LedControl.hpp"

using namespace std::placeholders;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define VERSION_STRING TOSTRING(VERSION_RAW)

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


String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(ESP8266WebServer* server, String path) { // send the right file to the client (if it exists)
  LittleFS.begin();
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  if (path.indexOf('.') == -1) {
    path = "/index.html";
  }
  String contentType = getContentType(path);            // Get the MIME type
  bool exists = LittleFS.exists(path);
  if (!exists) {
    path += ".gz";
    exists = LittleFS.exists(path);
  }
  if (exists) {                            // If the file exists
    File file = LittleFS.open(path, "r");                 // Open it
    server->streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
  }
  return exists;
  LittleFS.end();
}

esp::Http::Http() {
    this->arduinoVersion = new char[1] { 0 };
    this->server = new ESP8266WebServer();
    
    this->server->on("/reason", [this]() { this->handleReason(); });
    this->server->on("/api/fileUpload", HTTP_POST, [this]() { this->server->send(200); },  [this]() { this->handleFileUpload(); });
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

void esp::Http::broadcastOpInPorgress(bool isBusy) {
  String isBusyStr = this->getJsonFormatted(String("opInProgress"), String(isBusy ? "true" : "false"));
  this->webSocket->broadcastTXT(isBusyStr);
}

void esp::Http::broadcastMessage(const char* action, const char* value) {
  String msg = this->getJsonFormatted(String(action), String(value));
  this->webSocket->broadcastTXT(msg);
}

void esp::Http::log(const String& string) {
  String ss = this->getJsonFormatted(String("log"), string);
  this->webSocket->broadcastTXT(ss);
}

String esp::Http::getJsonFormatted(String action, String value) {
  DynamicJsonDocument doc(1024);
  doc["action"] = action;
  doc["value"] = value;
  String output;
  serializeJson(doc, output);
  return output;
}

String esp::Http::getJsonFormatted(const char* const action, String value) {
  return this->getJsonFormatted(String(action), value);
}

String esp::Http::getUnitId() {
  return this->getJsonFormatted("valueUnitId", String(Configuration::getUnitID()));
}

String esp::Http::getSsid() {
  char ssid[33];
  Configuration::getSSID(ssid);
  return this->getJsonFormatted("valueSsid", String(ssid));
}

String esp::Http::getPwd() {
  char password[61];
  Configuration::getPassword(password);
  return this->getJsonFormatted(String("valuePwd"), String(password));
}

String esp::Http::getSiteData() {
  uint8_t siteData[200];
  uint8_t siteDataLength = Configuration::getSD(siteData, 200);
  char str[700];
  array_to_string(siteData, siteDataLength, str);
  String sdStr(str);
  sdStr.trim();
  return this->getJsonFormatted(String("valueSiteData"), sdStr);
}

String esp::Http::getFiles() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();

  LittleFS.begin();
  LittleFS.mkdir("/hex");
  Dir dir = LittleFS.openDir("/hex");
  while (dir.next()) {
    File f = dir.openFile("r");
    if (f) {
    JsonObject nested = array.createNestedObject();
    nested["name"] = String(f.fullName());
    nested["size"] = f.size();
    f.close();
    }
  }
  String output;
  serializeJson(array, output);
  LittleFS.end();
  return this->getJsonFormatted(String("valueFiles"), output);
}

void esp::Http::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    String txtUnitId = this->getUnitId();
    this->webSocket->sendTXT(num, txtUnitId);
    String txtSsid = this->getSsid();
    this->webSocket->sendTXT(num, txtSsid);
    String txtPwd = this->getPwd();
    this->webSocket->sendTXT(num, txtPwd);
    String txtSiteData = this->getSiteData();
    this->webSocket->sendTXT(num, txtSiteData);
    String txtFiles = this->getFiles();
    this->webSocket->sendTXT(num, txtFiles);
    String aVersion = this->getJsonFormatted(String("versionArduino"), String(this->arduinoVersion));
    this->webSocket->sendTXT(num, aVersion);
    String eVersion = this->getJsonFormatted(String("versionEsp"), String(VERSION_STRING));
    this->webSocket->sendTXT(num, eVersion);
    String isBusy = this->getJsonFormatted(String("opInProgress"), String(esp::ArduinoFlash::instance().isBusy() ? "true" : "false"));
    this->webSocket->sendTXT(num, isBusy);
  }
  if (type == WStype_TEXT){
    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload, length);
    if (doc["action"] == "ping") {
      String pong = this->getJsonFormatted(String("ping"), String(""));
    }
    if (doc["action"] == "setUnitId") {
      Configuration::setUnitID(String((const char*)doc["value"]).toInt());
      String txtUnitId = this->getUnitId();
      this->webSocket->broadcastTXT(txtUnitId);
    }
    if (doc["action"] == "setSsid") {
      Configuration::setSSID((const char*)doc["value"]);
      String txtUnitId = this->getSsid();
      this->webSocket->broadcastTXT(txtUnitId);
    }
    if (doc["action"] == "setPwd") {
      Configuration::setPassword((const char*)doc["value"]);
      String txtUnitId = this->getPwd();
      this->webSocket->broadcastTXT(txtUnitId);
    }
    if (doc["action"] == "setSiteData") {
      String command = String((const char*)doc["value"]);
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
      String txtUnitId = this->getSiteData();
      this->webSocket->broadcastTXT(txtUnitId);
    }
    if (doc["action"] == "deleteFile") {
      String fileName((const char*)doc["value"]);
      LittleFS.begin();
      LittleFS.remove(fileName);
      LittleFS.end();
      String txtFiles = this->getFiles();
      this->webSocket->broadcastTXT(txtFiles);
    }
    if (doc["action"] == "verifyFile") {
      String fileName((const char*)doc["value"]);
      String verBeg = this->getJsonFormatted(String("verifyBegin"), String(""));
      this->webSocket->sendTXT(num, verBeg);
      esp::ArduinoFlash::verifyFile(fileName, [this, num](uint32_t pos, uint32_t max) -> void {
        String aVersion = this->getJsonFormatted(String("verifyProgress"), String((uint32_t)(((float)pos / max) * 100)));
        this->webSocket->sendTXT(num, aVersion);
      });
      String verEnd = this->getJsonFormatted(String("verifyEnd"), String(""));
      this->webSocket->sendTXT(num, verEnd);
    }
    if (doc["action"] == "verifyFlash") {
      String fileName((const char*)doc["value"]);
      esp::ArduinoFlash::instance().initiate(fileName, true);
    }
    if (doc["action"] == "programFlash") {
      String fileName((const char*)doc["value"]);
      esp::ArduinoFlash::instance().initiate(fileName, false);
    }
    if (doc["action"] == "restartArduino") {
      this->handleRestart(true);
    }
    if (doc["action"] == "restartEsp") {
      this->handleRestart(false);
    }
    if (doc["action"] == "resetEsp") {
      wifi_station_disconnect();
      ESP.eraseConfig();
      this->handleRestart(false);
    }
    if (doc["action"] == "applyWiFi") {
      wifi_station_disconnect();
    }
  }
}

void esp::Http::handleReason() {
  rst_info *myResetInfo;
  myResetInfo = ESP.getResetInfoPtr();
  String html = "<html><body>" + String(myResetInfo->reason) + ", " + String(myResetInfo->exccause) + "</body></body>";
  this->server->send(200, "text/html", html);
}

static File fsUploadFile;

void esp::Http::handleFileUpload() {
  HTTPUpload& upload = this->server->upload();
  if(upload.status == UPLOAD_FILE_START){
    LittleFS.begin();
    String filename = String(upload.filename);
    if(!filename.startsWith("/")) filename = "/" + filename;
    LittleFS.mkdir("/hex");
    filename = "/hex" + filename;
    this->log("Starting file upload " + filename);
    fsUploadFile = LittleFS.open(filename, "w+");
    if (!!!fsUploadFile)
    {
      this->log("Failed to create file");
    }
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(!!fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    } else {
      this->log("Failed writing chunk...");
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(!!fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
      fsUploadFile.close();
      this->server->send(200);
      LittleFS.end();
      String txtFiles = this->getFiles();
      this->webSocket->broadcastTXT(txtFiles);
    } else {
      this->server->send(500, "text/plain", "500: couldn't create file");
    }
  }
}

void esp::Http::handleRestart(bool restartArduino) {
  if (!restartArduino) {
    ESP.restart();
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
void esp::Http::handleNotFound() {
  if (!handleFileRead(this->server, this->server->uri()))
  {
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
    this->server->sendHeader("Location", "/");
    this->server->send(302, "text/plain", message);
  }
}

void esp::Http::setArduinoVersion(uint8_t* arduinoVersion) {
  if (strncmp(this->arduinoVersion, (const char*)arduinoVersion, strlen((const char*)arduinoVersion)) != 0) {
    delete this->arduinoVersion;
    this->arduinoVersion = new char[strlen((const char*)arduinoVersion) + 1];
    memcpy(this->arduinoVersion, arduinoVersion, strlen((const char*)arduinoVersion) + 1);
    String newVersion = this->getJsonFormatted(String("versionArduino"), String(this->arduinoVersion));
    this->webSocket->broadcastTXT(newVersion);
  }
}
