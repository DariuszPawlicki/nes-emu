#pragma once

#include "CPU6502.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"
#include "Byte.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <array>


class CPUBus
{
    public:
        CPUBus();

        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);

        void insertCartridge(std::shared_ptr<Cartridge> cartridge);
        void powerUp();

        CPU6502 cpu;
        PPU ppu;
        std::shared_ptr<Cartridge> cartridge;

	private:
		std::array<uint8_t, 2048> cpu_ram;
};