#include "Cartridge.hpp"
#include "NESConsole.hpp"


NESConsole::NESConsole() : main_bus(std::make_shared<MainBus>()) {
    main_bus->connectWithCPU();
}

void NESConsole::insertCartridgeAndPowerUp(const std::string& rom_path) {
    main_bus->insertCartridge(std::make_shared<Cartridge>(rom_path));
    main_bus->powerUp();
}

bool NESConsole::isRomChanged() {
    std::string ui_selected_rom_path = nes_console_main_window.getSelectedRomPath();

    if (selected_rom_path != ui_selected_rom_path) {
        selected_rom_path = ui_selected_rom_path;
        
        return true;
    }

    return false;
}

void NESConsole::showMainMenu() {
    nes_console_main_window.showMainMenu(*main_bus);
}