#pragma once

#include <Arduino.h>
#include <SPI.h>

#define MCP4921_CONFIG_AB 0x8000    // 0:Write to DACA, 1:Write to DACB
#define MCP4921_CONFIG_BUF 0x4000   // 0:Unbuffered, 1:Buffered
#define MCP4921_CONFIG_GA 0x2000    // 0:1x, 1:2x
#define MCP4921_CONFIG_SHDN 0x1000  // 0:Shutdown, 1:Active

class MCP4921 {
 public:
  MCP4921(uint8_t pin_cs, uint8_t pin_ldac);
  void begin(SPIClass& spi, SPISettings& settings);
  void config(bool ab, bool buf, bool ga, bool shdn);
  void write(uint16_t value);

 private:
  uint8_t _pin_cs;
  uint8_t _pin_ldac;
  SPIClass* _spi;
  SPISettings* _settings;
  uint16_t _config = 0x3000; // Default: DACA, Unbuffered, 1x, Active
};