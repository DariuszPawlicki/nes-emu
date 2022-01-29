#pragma once

#include "Byte.hpp"

#include <array>


class PPU
{ 
    private:
        std::array<uint8_t, 8 * 1024> pattern_table;
        std::array<uint8_t, 4 * 1024> name_table;
        std::array<uint8_t, 32> palette;

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
        // Accessing PPU bus
        void ppu_write(uint16_t address, uint8_t data);
        uint8_t ppu_read(uint16_t address);

        // Accessing memory mapped PPU registers, connected to CPU bus
        void cpu_write(uint16_t address, uint8_t data);
        Byte cpu_read(uint16_t address);
};