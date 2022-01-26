#include "CpuBus.hpp"




CpuBus::CpuBus(size_t size){ this->memory = new uint8_t[size]{ 0 }; }

CpuBus::~CpuBus() { delete[] this->memory; }

uint8_t* CpuBus::read(uint16_t address) 
{ 
    uint8_t* data_address;

    if(address >= 0x0 && address <= 0x1FFF)
    {
        data_address = &this->memory[address & 0x07FF]; // Memory mirroring
    }
    else
    {
        data_address = &this->memory[address];
    }

    return data_address; 
}

void CpuBus::write(uint16_t address, uint8_t data) { this->memory[address] = data; }

void CpuBus::clear_memory(size_t size){ memset(this->memory, 0, size); }