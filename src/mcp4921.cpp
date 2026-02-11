#include "mcp4921.h"

MCP4921::MCP4921(uint8_t pin_cs, uint8_t pin_ldac) {
  _pin_cs = pin_cs;
  _pin_ldac = pin_ldac;
  pinMode(_pin_cs, OUTPUT);
  pinMode(_pin_ldac, OUTPUT);
  digitalWrite(_pin_cs, HIGH);
  digitalWrite(_pin_ldac, HIGH);
}

void MCP4921::config(bool ab, bool buf, bool ga, bool shdn) {
  _config = 0x0000;
  if (ab) _config |= MCP4921_CONFIG_AB;
  if (buf) _config |= MCP4921_CONFIG_BUF;
  if (ga) _config |= MCP4921_CONFIG_GA;
  if (shdn) _config |= MCP4921_CONFIG_SHDN;
}

void MCP4921::begin(SPIClass& spi, SPISettings& settings) {
  _spi = &spi;
  _settings = &settings;
}

void MCP4921::write(uint16_t value) {
  uint16_t data = _config | (value & 0x0FFF);

  _spi->beginTransaction(*_settings);
  digitalWrite(_pin_cs, LOW);
  _spi->transfer16(data);
  digitalWrite(_pin_cs, HIGH);
  _spi->endTransaction();

  // LDACをLOWにして出力更新
  digitalWrite(_pin_ldac, LOW);
  digitalWrite(_pin_ldac, HIGH);
}