#pragma once

#include "CPU6502.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"
#include "UserInterface.hpp"


class NESConsole
{
	private:
		void prg_rom_to_ram();
		void chr_rom_to_ram();

	public:	
		CPU6502 cpu;
		PPU ppu;
		Cartridge cartridge;
		UserInterface ui;
		std::string selected_rom_path;	

		void insert_cartridge_and_power_up(std::string rom_path);
		void cpu_cycle();
		void show_main_menu();
		bool is_rom_changed();
};