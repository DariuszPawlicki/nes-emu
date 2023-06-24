#include "Byte.hpp"

void Byte::setBit(int pos, bool new_bit_value)
{
    if(new_bit_value) {
        value |= (1 << pos);
    }
    else {
        value &= ~(1 << pos);
    }
}

bool Byte::getBit(int pos)
{
    uint8_t value_on_bit_pos = std::pow(2, pos);
    bool bit_status = (value & value_on_bit_pos) >> pos;

    return bit_status;
}

uint8_t Byte::getByteValue(){ return value; }

void Byte::setByteValue(uint8_t new_byte_value) { value = new_byte_value; }