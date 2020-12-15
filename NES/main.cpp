#include <iostream>
#include "NESConsole.hpp"

int main()
{
	NESConsole* nes = new NESConsole();

	nes->insert_cartridge(std::string("C:\\Users\\Darek\\source\\repos\\NES\\ROMS\\cpu_dummy_reads.nes"));
}