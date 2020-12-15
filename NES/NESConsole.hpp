#pragma once


#include "CPU6502.hpp"
#include "Cartridge.hpp"
#include "Display.hpp"


class NESConsole
{

	private:
		CPU6502* cpu;
		Cartridge* cartridge;
		Display* display;

		void prg_rom_to_ram();
		void chr_rom_to_ram();


	public:	
		void insert_cartridge(std::string file_path);
		NESConsole();
		~NESConsole();

};