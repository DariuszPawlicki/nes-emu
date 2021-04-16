#include "NESConsole.hpp"
#include <iostream>

// D16E

int main()
{
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
        
        nes.show_main_menu();

        if(nes.is_rom_changed() || nes.ui.is_restart_checked())
        {
            nes.insert_cartridge_and_power_up(nes.selected_rom_path);
            nes.ui.reset_helpers();
        }   
              
        window.clear(sf::Color(102, 102, 255, 255));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

	return 0;
}