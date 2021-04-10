#pragma once

#include "CPU6502.hpp"
#include "Cartridge.hpp"
#include "UserInterface.hpp"


class NESConsole
{
	private:
		CPU6502 cpu;
		Cartridge cartridge;
		UserInterface ui;
		std::string selected_rom_path;

		void prg_rom_to_ram();
		void chr_rom_to_ram();

	public:	
		sf::RenderWindow& get_ui_window();
		std::string get_selected_rom_path();		

		bool insert_cartridge_and_power_up(std::string rom_path);
		void cpu_cycle();
		void show_main_menu();
		bool is_rom_changed();
};