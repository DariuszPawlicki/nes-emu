#pragma once

#include "MainBus.hpp"
#include "UI/NESConsoleMainWindow.hpp"


class NESConsole {
    public:
        NESConsole();

        void insertCartridgeAndPowerUp(const std::string& rom_path);
        void showMainMenu();
        bool isRomChanged();

        std::shared_ptr<MainBus> main_bus;
        NESConsoleMainWindow nes_console_main_window;
        std::string selected_rom_path;
};