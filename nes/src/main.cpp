#include "NESConsole.hpp"
#include <iostream>


int main()
{
	NESConsole nes;

    sf::RenderWindow& window = nes.get_ui_window();
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

        if(nes.is_rom_changed())
            nes.insert_cartridge_and_power_up(nes.get_selected_rom_path());
    
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

	return 0;
}