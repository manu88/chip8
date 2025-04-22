
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
