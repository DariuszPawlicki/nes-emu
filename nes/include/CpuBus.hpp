#pragma once

#include "CPU6502.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"
#include "Byte.hpp"

#include <cstdint>
#include <cstring>

class CpuBus
{
	public:
		uint8_t cpu_ram[2048];
	
	private:
		CPU6502 cpu;
		PPU ppu;
		Cartridge cartridge;
	
	public:
		CpuBus();

		uint8_t read(uint16_t address);
		void write(uint16_t address, uint8_t data);

		void clear_memory();
};