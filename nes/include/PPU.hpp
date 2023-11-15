#pragma once

#include "Byte.hpp"
#include "Cartridge.hpp"

#include <array>
#include <memory>


class PPU {
public:
    // Accessing PPU bus
    void ppuWrite(uint16_t address, uint8_t data);
    uint8_t ppuRead(uint16_t address);

    // Accessing memory mapped PPU registers, connected to CPU bus
    void cpuWrite(uint16_t address, uint8_t data);
    Byte cpuRead(uint16_t address);

    void connectToCartridge(std::shared_ptr<Cartridge> cartridge);

    Byte ppu_ctrl;
    Byte ppu_mask;
    Byte ppu_status;
    Byte ppu_scroll;
    Byte ppu_addr;
    Byte ppu_data;

    Byte oam_addr;
    Byte oam_data;
    Byte oam_dma;

    std::shared_ptr<Cartridge> cartridge;

private:
    std::array<uint8_t, 8 * 1024> pattern_table{};
    std::array<uint8_t, 4 * 1024> name_table{};
    std::array<uint8_t, 32> palette{};
};