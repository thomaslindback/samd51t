#pragma once
#include <bitset>
#include <type_traits>

inline void _print_reg_8(uint8_t val) {
    std::bitset<8> reg(val);
    Serial.print(reg.to_string().c_str());
}

template<size_t N>
void printReg(uint32_t val, const char* nl = "") {
    if(N == 16) {
        Serial.print("<");
        _print_reg_8(val >> 8);
        Serial.print(".");
        _print_reg_8(val);
        Serial.print("> ");
    } else if(N == 32) {
        Serial.print("<");
        _print_reg_8(val >> 24);
        Serial.print(".");
        _print_reg_8(val >> 16);
        Serial.print(".");
        _print_reg_8(val >> 8);
        Serial.print(".");
        _print_reg_8(val);
        Serial.print("> ");
    } else {
        Serial.print("<");
        _print_reg_8(val);
        Serial.print("> ");
    }
    Serial.print(nl);
}

template <typename Enumclass>
auto as_integer(Enumclass const value)
    -> typename std::underlying_type<Enumclass>::type
{
    return static_cast<typename std::underlying_type<Enumclass>::type>(value);
}
