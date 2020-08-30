#pragma once
#include <bitset>

template<size_t N>
void printReg(uint32_t val) {
    std::bitset<N> reg(val);
    Serial.print("<");
    Serial.print(reg.to_string().c_str());
    Serial.print("> ");
}