#pragma once

#include "Cartridge.hpp"
#include "utils/ChipsCommons.hpp"

#include <array>
#include <memory>
#include <unordered_map>


class PPU {
public:
    // Accessing PPU bus
    void ppuWrite(uint16_t address, uint8_t data);
    uint8_t ppuRead(uint16_t address) const;

    // Accessing memory mapped PPU registers, connected to CPU bus
    void cpuWrite(uint16_t address, uint8_t data);
    uint8_t cpuRead(uint16_t address) const;

    void connectToCartridge(std::shared_ptr<Cartridge> cartridge);

    std::shared_ptr<Cartridge> cartridge;

private:
    std::unordered_map<uint16_t, std::pair<std::string, chips_commons::Register<8>>> ppu_registers {
        {0x2000, {"PPUCTRL", {}}},
        {0x2001, {"PPUMASK", {}}},
        {0x2002, {"PPUSTATUS", {}}},
        {0x2003, {"OAMADDR", {}}},
        {0x2004, {"OAMDATA", {}}},
        {0x2005, {"PPUSCROLL", {}}},
        {0x2006, {"PPUADDR", {}}},
        {0x2007, {"PPUDATA", {}}},
        {0x4014, {"OAMDMA", {}}}
    };

    std::array<uint8_t, 2 * 1024> name_table{};
    std::array<uint8_t, 32> palette{};
};