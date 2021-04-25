#include "Byte.hpp"


void Byte::set_bit(int pos, bool value)
{
    if(value == true)
        this->value |= (1 << pos);
    else
        this->value &= ~(1 << pos);
}

bool Byte::get_bit(int pos)
{
    uint8_t value = std::pow(2, pos);
    bool bit_status = (this->value & value) >> pos;

    return bit_status;
}

uint8_t Byte::get_value(){ return this->value; }

void Byte::set_value(uint8_t value) { this->value = value; }