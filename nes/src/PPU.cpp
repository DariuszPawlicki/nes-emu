#include "PPU.hpp"


void PPU::ppuWrite(uint16_t address, uint8_t data) {
    address &= 0x3FFF;

    if (address >= 0x0000 && address <= 0x1FFF) {

    }
    else if (address >= 0x2000 && address <= 0x3EFF) {

    }
}

uint8_t PPU::ppuRead(uint16_t address) {
    address &= 0x3FFF;

    uint8_t data{0};


    return data;
}

void PPU::cpuWrite(uint16_t address, uint8_t data) {
    address &= 0x0007;


}

Byte PPU::cpuRead(uint16_t address) {
    Byte data;

    return data;
}