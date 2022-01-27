#pragma once

#include "CPU6502.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"
#include "Byte.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <array>

class CpuBus
{
	public:
		std::array<uint8_t, 2048> cpu_ram;
	
	private:
		CPU6502 cpu;
		PPU ppu;
		std::shared_ptr<Cartridge> cartridge;
	
	public:
		CpuBus();

		uint8_t read(uint16_t address);
		void write(uint16_t address, uint8_t data);

		void insert_cartridge(const std::shared_ptr<Cartridge>& cartridge);
		void power_up();

		void clear_memory();
};