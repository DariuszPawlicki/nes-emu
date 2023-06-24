#pragma once

#include "CpuBus.hpp"
#include "Cartridge.hpp"
#include "UserInterface.hpp"


class NESConsole {
public:
    void insertCartridgeAndPowerUp(std::string rom_path);
    void showMainMenu();
    bool isRomChanged();

    CpuBus cpu_bus;
    UserInterface ui;
    std::string selected_rom_path;
};