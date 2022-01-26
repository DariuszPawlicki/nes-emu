#pragma once

#include <cstdint>
#include <cstring>

class CpuBus
{
	public:
		uint8_t* memory;
	
	public:
		CpuBus(size_t size);
		~CpuBus();
		uint8_t* read(uint16_t address);
		void write(uint16_t address, uint8_t data);
		void clear_memory(size_t size);
};