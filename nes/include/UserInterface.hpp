#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include "imfilebrowser.h"
#include "imgui_memory_editor.h"
#include "CPU6502.hpp"
#include "Cartridge.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


class UserInterface
{ 
    private:
        sf::RenderWindow* window;      
        std::string selected_rom_path;
        
    
    private:
        void show_debugger(CPU6502& cpu, Cartridge& cartridge);
        std::vector<std::string> disassemble(CPU6502& cpu);

    public:
        UserInterface();
        ~UserInterface();

        sf::RenderWindow& get_window();
        std::string get_rom_path();

        void show_main_menu(CPU6502& cpu, Cartridge& cartridge); // Passing elements of console object
                                                                //  for convenient data access
                                                               //   needed for a debugger etc.                                                                 
};