#pragma once

#include <cmath>
#include <cstdint>


class Byte
{
public:
    void setBit(int pos, bool new_bit_value);
    bool getBit(int pos);
    uint8_t getByteValue();
    void setByteValue(uint8_t new_byte_value);

private:
    uint8_t value;
};