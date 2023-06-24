#include "NESConsole.hpp"


int main() {
    NESConsole nes;

    sf::RenderWindow& window = *(nes.ui.window);
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        ImGui::SFML::Update(window, clock.restart());

        nes.showMainMenu();

        if (nes.isRomChanged() || nes.ui.isRestartChecked()) {
            nes.insertCartridgeAndPowerUp(nes.selected_rom_path);
            nes.ui.resetHelpers();
        }

        window.clear(sf::Color(102, 102, 255, 255));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}