#include "IntelHexParser.h"
#include "http.h"
#include "stk.h"

esp::IntelHexParser::IntelHexParser()
{
}

void esp::IntelHexParser::Open(String& fileName)
{
  LittleFS.begin();
  this->file = LittleFS.open(fileName, "r");
  this->address = 0x0000U;
  if (!this->file)
  {
    esp::Http::instance().log("File doesn't exist");
  }
}

void esp::IntelHexParser::Close()
{
  LittleFS.end();
}

uint8_t hex2byte(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return 0xFF;
}

String esp::IntelHexParser::padString(String toPad, uint8_t padSize)
{
  String padded = toPad;
  while (padded.length() < padSize)
  {
    padded = "0" + padded;
  }
  return padded;
}

size_t esp::IntelHexParser::EnsurePage()
{
  if (!this->file)
  {
    esp::Http::instance().log("File doesn't exist");
    return 0;
  }
  while (this->data.size() < PAGE_SIZE)
  {
    if (this->file.available() < 11)
    {
      esp::Http::instance().log("Not enough bytes available to read " + String(this->file.available()));
      return this->data.size();
    }
    char buffer[INTEL_HEX_MAX_RECORD * 2];
    memset(buffer, 0x00, INTEL_HEX_MAX_RECORD * 2);
    size_t charsRead = this->file.readBytesUntil('\n', buffer, 300);
    if (charsRead < 11)
    {
      return 0;
    }
    if (buffer[0] != ':')
    {
      esp::Http::instance().log("Read Bytes: " + String(charsRead) + ", First char was not ':', it was " + String(buffer[0], 16));
      return 0;
    }
    uint8_t dataLength = (hex2byte(buffer[1]) << 4) + hex2byte(buffer[2]);
    if (dataLength == 0x00)
    {
      esp::Http::instance().log("Zero data length");
      this->file.close();
      return this->data.size();
    }
    uint16_t address = (hex2byte(buffer[3]) << 12) + (hex2byte(buffer[4]) << 8) + (hex2byte(buffer[5]) << 4) + hex2byte(buffer[6]);
    if (this->data.size() == 0) {
      this->address = address;
    }
    uint8_t recordType = (hex2byte(buffer[7]) << 4) + hex2byte(buffer[8]);
    if (recordType != 0x00)
    {
      esp::Http::instance().log("Record type is not 0x00");
      return this->data.size();
    }
    uint8_t calcChecksum = dataLength;
    calcChecksum += address >> 8;
    calcChecksum += address & 0xFF;
    calcChecksum += recordType;
    String printData;
    for (uint8_t i = 0; i < dataLength; i++)
    {
      byte readByte = (hex2byte(buffer[9 + i * 2]) << 4) + hex2byte(buffer[9 + i * 2 + 1]);
      this->data.push_back( readByte );
      calcChecksum += readByte;
      printData += padString(String(readByte, 16));
    }
    calcChecksum = (~calcChecksum)+1;
    uint8_t fileChecksum = (hex2byte(buffer[9 + dataLength * 2]) << 4) + hex2byte(buffer[9 + dataLength * 2 + 1]);
    esp::Http::instance().log("Address: " + padString(String(address, 16), 4) + ", Data: " + printData + ", calcChecksum: " + padString(String(calcChecksum, 16)) + " fileChecksum: " + padString(String(fileChecksum, 16)));
    if (calcChecksum != fileChecksum)
    {
      esp::Http::instance().log("Checksum didn't match");
      return -1;
    }
  }
  // Otherwise we assume it was a LF since the rest validated
  return this->data.size();
}
