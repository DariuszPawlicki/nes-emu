#include "CpuBus.hpp"


CpuBus::CpuBus(){ this->cpu.connect_bus(this); }


uint8_t CpuBus::read(uint16_t address) 
{ 
    uint8_t data;

    if(address >= 0x0000 && address <= 0x1FFF) // CPU ram
    {
        data = this->cpu_ram[address & 0x07FF]; // Memory mirroring
    }
    else if(address >= 0x2000 && address <= 0x3FFF) // PPU registers
    {
        data = this->ppu.cpu_read(address & 0x0007).get_value();
    }
    else if(address >= 0x4000 && address <= 0x4017) // APU range
    {
        
    }
    else if(address >= 0x4018 && address <= 0x401F) // APU and I/O functionality that is normally disabled. 
    {
        
    }
    else // Cartridge space
    {
        data = this->cpu_ram[address];
    }

    return data; 
}

void CpuBus::write(uint16_t address, uint8_t data) { this->cpu_ram[address] = data; }

void CpuBus::insert_cartridge(const std::shared_ptr<Cartridge>& cartridge) { this->cartridge = cartridge; }

void CpuBus::clear_memory(){  }