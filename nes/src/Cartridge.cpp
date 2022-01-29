#include "Cartridge.hpp"
#include <iostream>
#include <iomanip>


Cartridge::Cartridge(const std::string& rom_path)
{
	std::ifstream rom;

	rom.open(rom_path, std::ios::binary);

	if(rom.is_open())
	{
		this->rom_opened_correctly = true;

		uint8_t* header_data = new uint8_t[16];

		rom.seekg(0);
		rom.read((char*)header_data, 16);

		for(int i = 0; i < 4; i++)
			this->header.nes_const[i] = header_data[i];

		this->header.prg_rom_size = header_data[4];
		this->header.chr_rom_size = header_data[5];
		this->header.mapper_info1 = header_data[6];
		this->header.mapper_info2 = header_data[7];
		this->header.prg_ram_size = header_data[8];
		this->header.tv_info1 = header_data[9];
		this->header.tv_info2 = header_data[10];

		uint8_t lower_nibble = (this->header.mapper_info1 & 0b11110000) >> 4;
		uint8_t higher_nibble = (this->header.mapper_info1 & 0b00001111) << 4;

		this->mapper_id = higher_nibble + lower_nibble;

		for (int i = 11; i < 16; i++)
			this->header.padding[i - 11] = header_data[i];

		delete[] header_data;

		if(this->header.mapper_info1 & 0x04)
			rom.seekg(512, std::ios_base::cur); // Skipping trainer part if exists
		
		this->prg_rom.resize(16384 * this->header.prg_rom_size);
		this->chr_rom.resize(8192 * this->header.chr_rom_size);
		
		rom.read((char*)this->prg_rom.data(), this->prg_rom.size());
		rom.read((char*)this->chr_rom.data(), this->chr_rom.size());

		rom.close();

		if(this->header.prg_rom_size == 1) // If PRG ROM block count is 1 then, this block is duplicated
			this->prg_rom.insert(this->prg_rom.end(), this->prg_rom.begin(), this->prg_rom.end());
	}
	else
		this->rom_opened_correctly = false;
}

uint8_t Cartridge::cpu_read(uint16_t address)
{
	/* 
	   If PRG ROM blocks count is 1, then
	   PRG ROM block is duplicated, so
	   vector takes 32kB of space - 0x0000 -> 0x7FFF
	   Addresses given as arguments are in range - 0x4020 -> 0xFFFF
	   so there is a need to map them into vector addressable space
	*/
	if(this->header.prg_rom_size == 1)
		address &= 0x7FFF;

	return this->prg_rom[address];
}