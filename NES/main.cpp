#include <iostream>
#include "CPU6502.hpp"

int main()
{
	CPU6502* c = new CPU6502();

	c->load_rom(std::string("..\\ROMS\\cpu_dummy_writes_oam.nes"));
}