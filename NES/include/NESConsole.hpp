#pragma once

#include "CPU6502.hpp"
#include "Cartridge.hpp"
#include "Display.hpp"


class NESConsole
{
	private:
		CPU6502 cpu;
		Cartridge cartridge;
		Display display;

		void prg_rom_to_ram();
		void chr_rom_to_ram();


	public:	
		bool insert_cartridge_and_run(std::string file_path);
		void cpu_cycle();
};