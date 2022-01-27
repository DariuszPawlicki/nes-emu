#pragma once

#include "CpuBus.hpp"
#include "Cartridge.hpp"
#include "UserInterface.hpp"


class NESConsole
{
	public:	
		CpuBus cpu_bus;
		UserInterface ui;
		std::string selected_rom_path;	

	public:
		NESConsole();
		~NESConsole();
		
		void insert_cartridge_and_power_up(std::string rom_path);
		void show_main_menu();
		bool is_rom_changed();
};