#include "Cartridge.hpp"



Cartridge::Cartridge(const std::string& rom_path)
{
	std::ifstream rom;

	rom.open(rom_path, std::ios::binary);

	if(rom.is_open())
	{
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
	}
	else
		this->rom_opened_correctly = false;
}