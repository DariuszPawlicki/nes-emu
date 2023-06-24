#include "NESConsole.hpp"


void NESConsole::insertCartridgeAndPowerUp(std::string rom_path) {
    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>(rom_path);

    cpu_bus.insertCartridge(cartridge);
    cpu_bus.powerUp();
}

bool NESConsole::isRomChanged() {
    std::string ui_selected_rom_path = ui.get_selected_rom_path();

    if (selected_rom_path != ui_selected_rom_path) {
        selected_rom_path = ui_selected_rom_path;
        
        return true;
    }

    return false;
}

void NESConsole::showMainMenu() { ui.show_main_menu(cpu_bus); }