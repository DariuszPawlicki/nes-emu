#pragma once

#include "Cartridge.hpp"
#include "utils/ChipsCommons.hpp"

#include <array>
#include <memory>


class PPU {
public:
    // Accessing PPU bus
    void ppuWrite(uint16_t address, uint8_t data);
    uint8_t ppuRead(uint16_t address);

    // Accessing memory mapped PPU registers, connected to CPU bus
    void cpuWrite(uint16_t address, uint8_t data);
    uint8_t cpuRead(uint16_t address);

    void connectToCartridge(std::shared_ptr<Cartridge> cartridge);

    std::shared_ptr<Cartridge> cartridge;

private:
    chips_commons::Register<8> ppu_ctrl;
    chips_commons::Register<8> ppu_mask;
    chips_commons::Register<8> ppu_status;
    chips_commons::Register<8> ppu_scroll;
    chips_commons::Register<8> ppu_addr;
    chips_commons::Register<8> ppu_data;

    chips_commons::Register<8> oam_addr;
    chips_commons::Register<8> oam_data;
    chips_commons::Register<8> oam_dma;

    std::array<uint8_t, 2 * 1024> name_table{};
    std::array<uint8_t, 32> palette{};
};