#include "PPU.hpp"


void PPU::clear_memory(){ this->bus.clear_memory(16 * 1024); }

void PPU::write_to_memory(uint16_t address, uint8_t data)
{ 
    address &= 0x3FFF; // PPU can address 64 KB of memory
                      //  but has only 16 KB, so address
                     //   should be wrapped to allowed range
                    //    if given address would be 0x4000
                   //     then it would be wrapped to 0x0000
    this->bus.write(address, data); 
}

uint8_t* PPU::read_from_memory(uint16_t address)
{ 
    address &= 0x3FFF;
    return this->bus.read(address); 
}