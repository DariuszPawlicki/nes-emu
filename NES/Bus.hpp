#pragma once

#include <cstdint>

class Bus
{
	private:
		uint8_t memory[1024 * 64];
	
	public:
		uint8_t read(uint16_t address);
		void write(uint16_t address, uint8_t data);
};