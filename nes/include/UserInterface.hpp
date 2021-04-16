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
        // GUI objects       
        ImGui::FileBrowser file_browser;
        MemoryEditor mem_edit;

        // Input values and helper variables
        std::string selected_rom_path;
        bool debug_mode = false;
        bool restart = false;    
        char breakpoint_str[5];
        char pc_input[5];
        uint16_t breakpoint;
        int step_num = 1;
        
    
    private:
        void show_debugger(CPU6502& cpu);
        std::vector<std::string> disassemble(CPU6502& cpu);

    public:
        sf::RenderWindow* window;     

        UserInterface();
        ~UserInterface();

        std::string get_selected_rom_path();
        bool is_restart_checked();
        void show_main_menu(CPU6502& cpu); // Passing elements of console object
                                                                //  for convenient data access
                                                               //   needed for a debugger etc.
        void reset_helpers();                                                                 
};