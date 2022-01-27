#include "NESConsole.hpp"


void NESConsole::insert_cartridge_and_power_up(std::string rom_path) 
{
	std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>();

	this->cpu_bus.insert_cartridge(cartridge);
	this->cpu_bus.power_up();
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

void NESConsole::show_main_menu() { this->ui.show_main_menu(this->cpu_bus); }