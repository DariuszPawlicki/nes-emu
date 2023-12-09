#pragma once


#include <cstdint>


class Mapper {
public:
	Mapper(uint8_t prg_rom_size, uint8_t chr_rom_size);



protected:
	uint8_t prg_rom_size;
	uint8_t chr_rom_size;
};