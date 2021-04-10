#include "UserInterface.hpp"



UserInterface::UserInterface()
{
    auto height = sf::VideoMode::getDesktopMode().height;
    auto width = sf::VideoMode::getDesktopMode().width;

    this->window = new sf::RenderWindow(sf::VideoMode(width, height), "NES-EMU");
    this->window->setVerticalSyncEnabled(true);  

    ImGui::SFML::Init(*(this->window));
}

UserInterface::~UserInterface(){ delete this->window; }

sf::RenderWindow& UserInterface::get_window(){ return *this->window; }

std::string UserInterface::get_rom_path(){ return this->selected_rom_path; }

void UserInterface::show_main_menu(CPU6502 const& cpu, Cartridge const& cartridge)
{
    static bool debug_mode = false;
    static ImGui::FileBrowser file_browser;

    file_browser.SetTitle("Choose ROM");
    file_browser.SetTypeFilters({".nes"});

    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("Menu"))
        {
            if(ImGui::MenuItem("Insert Cartridge"))
                file_browser.Open();

            if(ImGui::MenuItem("Quit"))
                this->window->close();

            ImGui::EndMenu();
        } 

        if(ImGui::BeginMenu("Config"))
        {
            ImGui::MenuItem("Debug Mode", "", &debug_mode);
            ImGui::EndMenu();
        }
        
        file_browser.Display();

        if(file_browser.HasSelected())
        {
            this->selected_rom_path = file_browser.GetSelected();

            file_browser.ClearSelected();
        }

        if(debug_mode)
            this->show_debugger(cpu, cartridge);

        ImGui::EndMainMenuBar();      
    }     
}

void UserInterface::show_debugger(CPU6502 const& cpu, Cartridge const& cartridge)
{
    static MemoryEditor mem_edit;
    static uint8_t flags = cpu.status;

    ImGui::Begin("Disassembler");

    ImGui::Text("PC: 0x%x", cpu.pc);
    ImGui::Text("Stack Pointer: 0x%x", cpu.sp);
    ImGui::Text("ACC: 0x%x\n", cpu.acc);
    ImGui::Text("X: 0x%x", cpu.x);
    ImGui::Text("Y: 0x%x", cpu.y);
    ImGui::Text("Status Flags: N - %d  O - %d  B - %d  D - %d  I - %d  Z - %d  C - %d", 
                (flags & 0x80) >> 7, (flags & 0x40) >> 6, (flags & 0x10) >> 4, 
                (flags & 0x8) >> 3, (flags&0x4) >> 2, (flags & 0x2) >> 1, flags & 0x1);

    // ImGui::SetCursorPos({600, 500});

    // if(ImGui::Button("Step"))
    //     cpu.bus.memory[0] += 1;

    ImGui::End();

    mem_edit.DrawWindow("Memory", cpu.bus.memory, 64 * 1024); 
}