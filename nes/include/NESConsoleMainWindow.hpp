#pragma once

#include <imgui.h>

#include "CPUBus.hpp"

#include <vector>
#include <array>

#include <imgui-SFML.h>
#include <imfilebrowser.h>
#include <imgui_memory_editor.h>
#include <SFML/Graphics/RenderWindow.hpp>


class NESConsoleMainWindow
{
    public:
        std::shared_ptr<sf::RenderWindow> window;

        NESConsoleMainWindow();

        std::string getSelectedRomPath();
        bool isRestartChecked();
        void showMainMenu(CPUBus& cpu_bus);
        void resetHelpers();

    private:
        // GUI objects       
        ImGui::FileBrowser file_browser;

        MemoryEditor cpu_mem_edit;
        MemoryEditor ppu_mem_edit;

        // Input values and helper variables
        std::string selected_rom_path;

        bool debug_mode{false};
        bool restart{false};

        char breakpoint_str[5];
        char pc_input[5];

        uint16_t breakpoint;

        unsigned int step_num{1};

        std::array<uint8_t, 64 * 1024> cpu_mem_layout;

        void showCpuDebugger(CPUBus& cpu_bus);
        void showPpuDebugger(PPU& ppu);
        std::vector<std::string> disassemble(CPU6502& cpu);
};