#include "CpuBus.hpp"


CpuBus::CpuBus(){ this->cpu.connect_bus(this); }

void CpuBus::power_up(){ this->cpu.power_up(); }

uint8_t CpuBus::read(uint16_t address) 
{ 
    uint8_t data;

    if(address >= 0x0000 && address <= 0x1FFF) // CPU ram
    {
        data = this->cpu_ram[address & 0x07FF]; // Memory mirroring
    }
    else if(address >= 0x2000 && address <= 0x3FFF) // PPU registers
    {
        // data = this->ppu.cpu_read(address & 0x0007).get_value();
        data = 0;
    }
    else if(address >= 0x4000 && address <= 0x4017) // APU range
    {
        data = 0; // Placeholder for debugging, waiting for future implementation
    }
    else if(address >= 0x4018 && address <= 0x401F) // APU and I/O functionality that is normally disabled. 
    {
        data = 0; // Placeholder for debugging, waiting for future implementation
    }
    else // Cartridge space
    {
        data = this->cartridge->cpu_read(address);
    }

    return data; 
}

void CpuBus::write(uint16_t address, uint8_t data) // TODO
{ 
    this->cpu_ram[address] = data; 
}

void CpuBus::insert_cartridge(const std::shared_ptr<Cartridge>& cartridge) { this->cartridge = cartridge; }

void CpuBus::clear_memory(){  }