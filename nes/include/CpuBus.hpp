#pragma once

#include "CPU6502.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"

#include <cstdint>
#include <cstring>

class CpuBus
{
	public:
		uint8_t* memory;
	
	private:
		CPU6502 cpu;
		PPU ppu;
		Cartridge cartridge;
	
	public:
		CpuBus(size_t size);
		~CpuBus();
		uint8_t* read(uint16_t address);
		void write(uint16_t address, uint8_t data);
		void clear_memory(size_t size);
};