#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include "imfilebrowser.h"
#include "imgui_memory_editor.h"
#include "CpuBus.hpp"
#include "Cartridge.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <array>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


class UserInterface
{ 
    private:
        // GUI objects       
        ImGui::FileBrowser file_browser;

        MemoryEditor cpu_mem_edit;
        MemoryEditor ppu_mem_edit;

        // Input values and helper variables
        std::string selected_rom_path;

        bool debug_mode = false;
        bool restart = false;   

        char breakpoint_str[5];
        char pc_input[5];

        uint16_t breakpoint;

        unsigned int step_num = 1;

        std::array<uint8_t, 64 * 1024> cpu_mem_layout;
        
    
    private:
        void show_cpu_debugger(CpuBus& cpu_bus);
        void show_ppu_debugger(PPU& ppu);
        std::vector<std::string> disassemble(CPU6502& cpu);

    public:
        sf::RenderWindow* window;     

        UserInterface();
        ~UserInterface();

        std::string get_selected_rom_path();
        bool is_restart_checked();
        void show_main_menu(CpuBus& cpu_bus);        // Passing cpu bus
                                                    //  for convenient data access
                                                   //   needed for a debugger etc.
        void reset_helpers();                                                                 
};