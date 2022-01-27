#pragma once

#include "Byte.hpp"

class PPU
{ 
    private:
        uint8_t pattern_table[8 * 1024];
        uint8_t name_table[4 * 1024];
        uint8_t palette[32];

    public:
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
        // Accessing devices connected to PPU bus
        void ppu_write(uint16_t address, uint8_t data);
        uint8_t ppu_read(uint16_t address);

        // Accessing memory mapped PPU registers, connected to CPU bus
        void cpu_write(uint16_t address, uint8_t data);
        Byte cpu_read(uint16_t address);
};