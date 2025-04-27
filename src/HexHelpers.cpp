
#include "HexHelpers.hpp"

static void toUpper(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::string hex(uint16_t value, bool prefix /* = true*/) {
    char str[16];
    char *p = &str[16];
    do {
        p--;
        uint32_t digit = value % 16;
        value /= 16;
        *p = digit >= 10 ? 'a' + (digit - 10) : '0' + digit;
    } while (value > 0);
    if (prefix) {
        p--;
        *p = 'X';
        p--;
        *p = '0';
    }
    auto ret = std::string(p, &str[16] - p);
    toUpper(ret);
    return ret;
}

uint8_t parseRegisterAddr(const std::string &str, bool &valid) {
    if (std::tolower(str[0]) != 'v') {
        valid = false;
        return 0;
    }
    if (isxdigit(str.c_str()[1]) == 0) {
        valid = false;
        return 0;
    }
    uint8_t number = (uint8_t)std::strtol(str.c_str() + 1, NULL, 16);
    valid = number <= 0XF;
    return number;
}

uint16_t parseNumber(const std::string &str, uint16_t maxVal, bool &valid) {
    uint16_t val = 0;
    try {
        valid = true;
        val = std::stoll(str.c_str(), nullptr, 0);
    } catch (const std::invalid_argument &e) {
        valid = false;
        return 0;
    }
    if (val > maxVal) {
        valid = false;
        return 0;
    }
    return val;
}

uint8_t parseNibble(const std::string &str, bool &valid) {
    return parseNumber(str, 0XF, valid);
}

uint8_t parseByte(const std::string &str, bool &valid) {

    return parseNumber(str, 0XFF, valid);
}

uint16_t parseAddr(const std::string &str, bool &valid) {
    return parseNumber(str, 0XFFF, valid);
}

bool isNumber(const std::string &s) {
    if (s[0] == '0' && std::tolower(s[1]) == 'x') {
        return isNumber(s.substr(2));
    }
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) {
                             return !std::isxdigit(c);
                         }) == s.end();
}
