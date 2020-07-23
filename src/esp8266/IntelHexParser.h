#ifndef INTELHEXPARSER_H
#define INTELHEXPARSER_H

#include <LittleFS.h>
#include <vector>
#include <WString.h>
#include <stdint.h>
#define INTEL_HEX_MAX_RECORD 255

namespace esp
{
  class IntelHexParser
  {
  public:
    IntelHexParser();

    void Open(String& fileName);
    void Close();

    size_t EnsurePage();
    String padString(String toPad, uint8_t padSize = 2);

    uint16_t address;
    std::vector<uint8_t> data;
  private:
    File file;
  };
} // namespace esp

#endif
