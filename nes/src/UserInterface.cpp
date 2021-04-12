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

void UserInterface::show_main_menu(CPU6502& cpu, Cartridge& cartridge)
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

void UserInterface::show_debugger(CPU6502& cpu, Cartridge& cartridge)
{
    static MemoryEditor mem_edit;
    

    ImGui::SetNextWindowSize({552, 595}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({23, 19}, ImGuiCond_Once);

    ImGui::Begin("Disassembler");

    ImGui::Text("PC: 0x%X", cpu.pc);
    ImGui::Text("Stack Pointer: 0x%X", cpu.sp);
    ImGui::Text("ACC: 0x%X\n", cpu.acc);
    ImGui::Text("X: 0x%X", cpu.x);
    ImGui::Text("Y: 0x%X", cpu.y);
    ImGui::Text("Status Hex Value: 0x%X", cpu.status);

    uint8_t flags = cpu.status;

    ImGui::Text("Status Flags Value: N - %d  V - %d  U - %d  B - %d  D - %d  I - %d  Z - %d  C - %d", 
                (flags & 0x80) >> 7, (flags & 0x40) >> 6, (flags & 0x20) >> 5, (flags & 0x10) >> 4, 
                (flags & 0x8) >> 3, (flags & 0x4) >> 2, (flags & 0x2) >> 1, flags & 0x1);

    ImGui::Separator();
    
    std::vector<std::string> disassembled_instructions;
    disassembled_instructions = this->disassemble(cpu);

    if(ImGui::BeginTable("Instructions", 1))
    {
        for(auto it = disassembled_instructions.begin(); it != disassembled_instructions.end(); it++)
        {
            std::string decoded = *it;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(decoded.c_str());
        }
        ImGui::EndTable();
    }  

    static char breakpoint_str[5];
    static uint16_t breakpoint;

    ImGui::SetCursorPos({353, 250});
    ImGui::PushItemWidth(40);
    ImGui::InputText(" ", breakpoint_str, IM_ARRAYSIZE(breakpoint_str)); 

    ImGui::SetCursorPos({403, 250});    
    if(ImGui::Button("Set Breakpoint", {120, 19}))
    {
        std::stringstream break_hex;

        break_hex << std::hex << breakpoint_str;
        break_hex >> breakpoint;
        memset(&breakpoint_str, (int)'\0', 4);
    }

    ImGui::SetCursorPos({353, 220});
    ImGui::Text("Breakpoint: 0x%X", breakpoint);

    ImGui::SetCursorPos({353, 320});
    if(ImGui::Button("Run", {100, 30}))
    {
        while(cpu.pc != breakpoint)
            cpu.cycle();
    }

    ImGui::SetCursorPos({353, 351});
    if(ImGui::Button("Step", {100, 30}))
        cpu.cycle();
    
    static char pc_input[5];

    ImGui::PushItemWidth(40);
    ImGui::SetCursorPos({270, 25});
    ImGui::InputText("", pc_input, IM_ARRAYSIZE(pc_input));

    ImGui::SetCursorPos({320, 25});    
    if(ImGui::Button("Set PC", {60, 19}))
    {
        std::stringstream pc_hex;

        pc_hex << std::hex << pc_input;
        pc_hex >> cpu.pc;
        memset(&pc_input, (int)'\0', 4);
    }
    
    ImGui::SetCursorPos({393, 25});
    ImGui::Text("X: %.2f Y: %.2f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

    ImGui::End();

    ImGui::SetNextWindowSize({552, 595}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({652, 19}, ImGuiCond_Once);

    mem_edit.DrawWindow("Memory", cpu.bus.memory, 64 * 1024); 
}

std::vector<std::string> UserInterface::disassemble(CPU6502& cpu)
{
    std::vector<std::string> disassembled_instructions;

        uint16_t tmp_pc = cpu.pc;

        for(int i = 0; i < 27; i++)
        {
            std::stringstream dis_instruction;

            uint8_t op_code = *cpu.read_from_memory(tmp_pc);       
            CPU6502::Instruction instruction = cpu.op_map[op_code];

            std::string addressing_name = instruction.op_name.substr(4, instruction.op_name.length());
            uint8_t operand_bytes = cpu.operand_bytes[addressing_name];
            std::vector<uint8_t> operands;
            
            dis_instruction << '$' << std::hex << std::setw(4); 
            dis_instruction << std::setfill('0') << tmp_pc << "   ";                    // Print pc address e.g. $0F01
            dis_instruction << std::hex << std::uppercase << std::setw(2);             //  Print operation
                                                                                      //   hex value e.g. 0B
            dis_instruction << std::setfill('0') << (uint16_t)op_code << ' '; 

            for(int j = 1; j <= operand_bytes; j++)
            {
                operands.push_back(*cpu.read_from_memory(tmp_pc + j));

                dis_instruction << std::hex << std::uppercase << std::setfill('0'); // Print operands padded 
                                                                                   //  to length 2 e.g. operand 9 = 09
                dis_instruction << std::setw(2) << (uint16_t)operands.back() << ' ';
            }

            for(int i = 0; i < ((2 - operand_bytes) * 3) + 4; i++)
                dis_instruction << ' ';

            dis_instruction << instruction.op_name << ' ';

            if(operand_bytes > 0)
            {
                uint16_t full_operand;

                if(operand_bytes == 1)
                    full_operand = operands[0];
                else if(operand_bytes == 2)
                    full_operand = ((uint16_t)operands[1] << 8) + (uint16_t)operands[0];

                if(addressing_name == "IMD") // If instruction addressing is immediate then
                                            //  print operand in form #$ instead $
                    dis_instruction << '#';
                else if(addressing_name == "REL") // Commented in CPU6502 mod_rel function
                    full_operand = tmp_pc + full_operand + 2;

                dis_instruction << '$' << std::setfill('0') << std::setw(4);
                dis_instruction << std::hex << full_operand;
            }
            
            dis_instruction << '\n';

            tmp_pc += operand_bytes + 1;

            std::string stream_str = dis_instruction.str();
            std::transform(stream_str.begin(), stream_str.end(), stream_str.begin(), ::toupper);

            disassembled_instructions.push_back(stream_str);
        }

    return disassembled_instructions;
}