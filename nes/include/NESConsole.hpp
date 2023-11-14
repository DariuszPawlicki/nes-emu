#pragma once

#include "CPUBus.hpp"
#include "Cartridge.hpp"
#include "NESConsoleMainWindow.hpp"


class NESConsole {
public:
    void insertCartridgeAndPowerUp(const std::string& rom_path);
    void showMainMenu();
    bool isRomChanged();

    CPUBus cpu_bus;
    NESConsoleMainWindow nes_console_main_window;
    std::string selected_rom_path;
};