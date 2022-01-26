#include "NESConsole.hpp"



void NESConsole::prg_rom_to_ram() 
{
	bool has_trainer = (this->cartridge.header.flags6 & 0x4) >> 2;

	if (has_trainer) 
		this->cartridge.rom->seekg(528);

	int prg_size = this->cartridge.header.prg_rom_size * 16384;

	uint8_t* prg_rom_buffer = new uint8_t[prg_size];

	this->cartridge.rom->read((char*)prg_rom_buffer, prg_size);
	this->cpu.clear_memory(); // Cleaning memory in case different cartridge was loaded earlier

	for (int i = 0; i < prg_size; i++) 
		this->cpu.write_to_memory(this->cpu.PRG_ROM_BEGINNING + i, prg_rom_buffer[i]);
	
	/* If cartridge contains only one 16 kB memory block, then
	   it's duplicated */

	if(this->cartridge.header.prg_rom_size == 1)
	{
		for(int i = 0; i < prg_size; i++)
			this->cpu.write_to_memory((this->cpu.PRG_ROM_BEGINNING + 16384) + i, prg_rom_buffer[i]);
	}

	delete[] prg_rom_buffer;
}

void NESConsole::chr_rom_to_ram() 
{
	int chr_size = this->cartridge.header.chr_rom_size * 8192;

	uint8_t* chr_rom_buffer = new uint8_t[chr_size];

	this->cartridge.rom->read((char*)chr_rom_buffer, chr_size);
	this->ppu.clear_memory();

	for(int i = 0; i < chr_size; i++)
		this->ppu.ppu_write(i, chr_rom_buffer[i]);
	
	delete[] chr_rom_buffer;
}

bool NESConsole::is_rom_changed()
{
	std::string ui_selected_rom_path = this->ui.get_selected_rom_path();

	if(this->selected_rom_path != ui_selected_rom_path)
	{
		this->selected_rom_path = ui_selected_rom_path;

		return true;
	}
	return false;
}

void NESConsole::insert_cartridge_and_power_up(std::string rom_path) 
{
	if(this->cartridge.load_rom(rom_path))
	{
		this->prg_rom_to_ram();
		this->chr_rom_to_ram();
		this->cpu.power_up();
	}
}

void NESConsole::cpu_cycle(){ this->cpu.cycle(); }

void NESConsole::show_main_menu() { this->ui.show_main_menu(this->cpu, this->ppu); }