#pragma once

#include <cstdint>

class Bus
{
	private:
		uint8_t memory[64 * 1024];
	
	public:
		uint8_t read(uint16_t address);
		void write(uint16_t address, uint8_t data);
};