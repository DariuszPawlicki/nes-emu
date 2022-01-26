#pragma once

#include "CpuBus.hpp"
#include "Byte.hpp"

class PPU
{ 
    public:
        CpuBus bus = CpuBus(16 * 1024);

        // Registers
        
        Byte ppu_ctrl;
        Byte ppu_mask;
        Byte ppu_status;
        Byte ppu_scroll;
        Byte ppu_addr;
        Byte ppu_data;

        Byte oam_addr;
        Byte oam_data;      
        Byte oam_dma;
    
    public:
        void clear_memory();
        void write_to_memory(uint16_t address, uint8_t data);
        uint8_t* read_from_memory(uint16_t address);
};