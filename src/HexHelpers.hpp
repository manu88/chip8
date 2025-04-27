
#pragma once

#include <string>

std::string hex(uint16_t value, bool prefix = true);
uint8_t parseRegisterAddr(const std::string &str, bool &valid);
uint16_t parseNumber(const std::string &str, uint16_t maxVal,
                     bool &valid);
uint8_t parseNibble(const std::string &str, bool &valid) ;
uint8_t parseByte(const std::string &str, bool &valid) ;
uint16_t parseAddr(const std::string &str, bool &valid);
bool isNumber(const std::string &s);
