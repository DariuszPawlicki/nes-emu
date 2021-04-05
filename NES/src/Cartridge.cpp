#include "Cartridge.hpp"



void Cartridge::extract_header_info() 
{
	uint8_t* header_data = new uint8_t[16];

	this->rom.read((char*)header_data, 16);

	for (int i = 0; i < 4; i++) {
		this->header.nes_const[i] = header_data[i];
	}

	this->header.prg_rom_size = header_data[4];
	this->header.chr_rom_size = header_data[5];
	this->header.flags6 = header_data[6];
	this->header.flags7 = header_data[7];
	this->header.flags8 = header_data[8];
	this->header.flags9 = header_data[9];
	this->header.flags10 = header_data[10];

	uint8_t lower_nibble = (this->header.flags6 & 0b11110000) >> 4;
	uint8_t higher_nibble = (this->header.flags6 & 0b00001111) << 4;

	this->header.mapper_id = higher_nibble + lower_nibble;

	for (int i = 11; i < 16; i++) {
		this->header.padding[i - 11] = header_data[i];
	}

	delete[] header_data;
}


bool Cartridge::load_rom(std::string file_path) 
{
	this->rom.open(file_path, std::ios::binary);

	if (this->rom.is_open())
	{
		this->extract_header_info();
		return true;
	}
	return false;
}