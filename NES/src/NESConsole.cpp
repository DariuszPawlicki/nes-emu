#include "NESConsole.hpp"



void NESConsole::prg_rom_to_ram() 
{
	bool has_trainer = (this->cartridge.header.flags6 & 0x4) >> 2;

	if (has_trainer) 
	{
		this->cartridge.rom.seekg(528);
	}

	int prg_size = this->cartridge.header.prg_rom_size * 16384;

	uint8_t* prg_rom_buffer = new uint8_t[prg_size];

	this->cartridge.rom.read((char*)prg_rom_buffer, prg_size);

	for (int i = 0; i < prg_size; i++) 
	{
		this->cpu.bus.write(this->cpu.PRG_ROM_BEGINNING + i, prg_rom_buffer[i]);
	}

	delete[] prg_rom_buffer;
}

void NESConsole::chr_rom_to_ram() 
{

}

bool NESConsole::insert_cartridge_and_run(std::string file_path) 
{
	if(this->cartridge.load_rom(file_path))
	{
		this->prg_rom_to_ram();
		this->cpu.power_up();

		return true;
	}
	return false;
}

void NESConsole::cpu_cycle()
{
	this->cpu.cpu_cycle();
}