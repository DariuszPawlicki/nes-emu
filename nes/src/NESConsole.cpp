#include "NESConsole.hpp"


void NESConsole::insertCartridgeAndPowerUp(std::string rom_path) {
    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>(rom_path);

    cpu_bus.insertCartridge(cartridge);
    cpu_bus.powerUp();
}

bool NESConsole::isRomChanged() {
    std::string ui_selected_rom_path = nes_console_main_window.getSelectedRomPath();

    if (selected_rom_path != ui_selected_rom_path) {
        selected_rom_path = ui_selected_rom_path;
        
        return true;
    }

    return false;
}

void NESConsole::showMainMenu() { nes_console_main_window.showMainMenu(cpu_bus); }