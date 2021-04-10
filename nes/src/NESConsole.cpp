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

	delete[] prg_rom_buffer;
}

void NESConsole::chr_rom_to_ram() 
{

}

bool NESConsole::is_rom_changed()
{
	std::string ui_selected_rom_path = this->ui.get_rom_path();

	if(this->selected_rom_path != ui_selected_rom_path)
	{
		this->selected_rom_path = ui_selected_rom_path;

		return true;
	}
	return false;
}

sf::RenderWindow& NESConsole::get_ui_window(){ return this->ui.get_window(); }

std::string NESConsole::get_selected_rom_path(){ return this->selected_rom_path; }

bool NESConsole::insert_cartridge_and_power_up(std::string rom_path) 
{
	if(this->cartridge.load_rom(rom_path))
	{
		this->prg_rom_to_ram();
		this->cpu.power_up();

		return true;
	}
	return false;
}

void NESConsole::cpu_cycle(){ this->cpu.cpu_cycle(); }

void NESConsole::show_main_menu() { this->ui.show_main_menu(this->cpu, this->cartridge); }
