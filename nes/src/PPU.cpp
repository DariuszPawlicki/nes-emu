#include "PPU.hpp"


void PPU::ppu_write(uint16_t address, uint8_t data)
{ 
    address &= 0x3FFF;

    if(address >= 0x0000 && address <= 0x1FFF)
    {

    }
    else if(address >= 0x2000 && address <= 0x3EFF)
    {

    }
}

uint8_t* PPU::ppu_read(uint16_t address)
{ 
    address &= 0x3FFF;
    
}

void PPU::cpu_write(uint16_t address, uint8_t data)
{
    address &= 0x0007;


}