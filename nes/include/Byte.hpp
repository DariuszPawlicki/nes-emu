#pragma once


#include <cstdint>
#include <cmath>


class Byte
{
    /* Class for representing status registers
       for convenient access to certain bit values
       of specific one byte register. */
       
    private:
        uint8_t value;
    
    public:       
        void set_bit(int pos, bool value);
        bool get_bit(int pos);

        uint8_t get_value();
        void set_value(uint8_t value);
};