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
    constexpr inline static uint16_t PPUCTRL_REGISTER_ADDRESS{0x2000};
    constexpr inline static uint16_t PPUMASK_REGISTER_ADDRESS{0x2001};
    constexpr inline static uint16_t PPUSTATUS_REGISTER_ADDRESS{0x2002};
    constexpr inline static uint16_t OAMADDR_REGISTER_ADDRESS{0x2003};
    constexpr inline static uint16_t OAMDATA_REGISTER_ADDRESS{0x2004};
    constexpr inline static uint16_t PPUSCROLL_REGISTER_ADDRESS{0x2005};
    constexpr inline static uint16_t PPUADDR_REGISTER_ADDRESS{0x2006};
    constexpr inline static uint16_t PPUDATA_REGISTER_ADDRESS{0x2007};
    constexpr inline static uint16_t OAMDMA_REGISTER_ADDRESS{0x4014};

    std::unordered_map<uint16_t, std::pair<std::string, chips_commons::Register<8>>> ppu_registers {
        {PPUCTRL_REGISTER_ADDRESS, {"PPUCTRL", {}}},
        {PPUMASK_REGISTER_ADDRESS, {"PPUMASK", {}}},
        {PPUSTATUS_REGISTER_ADDRESS, {"PPUSTATUS", {}}},
        {OAMADDR_REGISTER_ADDRESS, {"OAMADDR", {}}},
        {OAMDATA_REGISTER_ADDRESS, {"OAMDATA", {}}},
        {PPUSCROLL_REGISTER_ADDRESS, {"PPUSCROLL", {}}},
        {PPUADDR_REGISTER_ADDRESS, {"PPUADDR", {}}},
        {PPUDATA_REGISTER_ADDRESS, {"PPUDATA", {}}},
        {OAMDMA_REGISTER_ADDRESS, {"OAMDMA", {}}}
    };

    std::array<uint8_t, 2 * 1024> name_table{};
    std::array<uint8_t, 32> palette{};
};