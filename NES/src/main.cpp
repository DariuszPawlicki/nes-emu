#include <iostream>
#include "NESConsole.hpp"


int main(int argc, char** argv)
{
	NESConsole* nes = new NESConsole();
	
	if(nes->insert_cartridge_and_run(argv[1]))
	{
		while(true)
		{
			nes->cpu_cycle();
		}
	}
	else
	{
		std::cout<<"File not found."<<std::endl;
	}

	return 0;
}