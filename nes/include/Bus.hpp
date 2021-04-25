#pragma once

#include <cstdint>
#include <cstring>

class Bus
{
	public:
		uint8_t* memory;
	
	public:
		Bus(size_t size);
		~Bus();
		uint8_t* read(uint16_t address);
		void write(uint16_t address, uint8_t data);
		void clear_memory(size_t size);
};