#include "NESConsole.hpp"


NESConsole::NESConsole() {

	this->cpu = new CPU6502();
}


void NESConsole::insert_cartridge(std::string file_path) 
{
	this->cartridge = new Cartridge();
	this->cartridge->load_rom(file_path);
	this->prg_rom_to_ram();
}



void NESConsole::prg_rom_to_ram() {

	bool is_trainer = (this->cartridge->header->flags6 & 0x8) >> 3;

	if (is_trainer) 
	{
		this->cartridge->rom->seekg(528);
	}

	int prg_size = this->cartridge->header->prg_rom_size * 16384;

	uint8_t* prg_rom_buffer = new uint8_t[prg_size];

	this->cartridge->rom->read((char*)prg_rom_buffer, prg_size);

	for (int i = 0; i < prg_size; i++) 
	{
		this->cpu->bus.write(this->cpu->ROM_MEMORY_BEGINNING + i, prg_rom_buffer[i]);
	}

	delete[] prg_rom_buffer;
}


void NESConsole::chr_rom_to_ram() {


}


NESConsole::~NESConsole() 
{
	delete this->cpu;
	delete this->cartridge;
	delete this->display;
}