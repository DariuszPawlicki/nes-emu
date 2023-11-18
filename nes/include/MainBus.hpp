#pragma once

#include "PPU.hpp"
#include "CPU6502.hpp"
#include "Cartridge.hpp"

#include <array>
#include <memory>
#include <cstdint>


class MainBus : public std::enable_shared_from_this<MainBus>
{
    public:
        [[nodiscard]] uint8_t read(uint16_t address) const;
        void write(uint16_t address, uint8_t data);

		void connectWithCPU();
        void powerUp();
        void insertCartridge(std::shared_ptr<Cartridge> cartridge);

        CPU6502 cpu;
        PPU ppu;
        std::shared_ptr<Cartridge> cartridge;

	private:
		std::array<uint8_t, 2048> cpu_ram{};
};