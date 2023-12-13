#include "Cartridge.hpp"
#include "ui/NESConsoleMainWindow.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


NESConsoleMainWindow::NESConsoleMainWindow() {
    auto height{sf::VideoMode::getDesktopMode().height};
    auto width{sf::VideoMode::getDesktopMode().width};

    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), "NES-EMU");
    window->setVerticalSyncEnabled(true);

    ImGui::SFML::Init(*window);

    resetHelpers();
}

std::string NESConsoleMainWindow::getSelectedRomPath() { return selected_rom_path; }

bool NESConsoleMainWindow::isRestartChecked() { return restart; }

void NESConsoleMainWindow::showMainMenu(MainBus& cpu_bus) {
    file_browser.SetTitle("Choose ROM");
    file_browser.SetTypeFilters({".nes"});

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Insert Cartridge"))
                file_browser.Open();

            if (ImGui::MenuItem("Reload ROM"))
                restart = true;

            if (ImGui::MenuItem("Quit"))
                window->close();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")) {
            ImGui::MenuItem("Debug Mode", "", &debug_mode);
            ImGui::EndMenu();
        }

        file_browser.Display();

        if (file_browser.HasSelected()) {
            selected_rom_path = file_browser.GetSelected().string();
            file_browser.ClearSelected();
        }

        if (debug_mode) {
            showCpuDebugger(cpu_bus);
            //showPpuDebugger(ppu);
        }

        ImGui::EndMainMenuBar();
    }
}

void NESConsoleMainWindow::showCpuDebugger(MainBus& cpu_bus) {
    CPU6502& cpu = cpu_bus.cpu;

    ImGui::SetNextWindowSize({552, 595}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({23, 19}, ImGuiCond_Once);

    ImGui::Begin("Disassembler");

    chips_commons::Register<8> status = cpu.status;

    ImGui::Text("PC: 0x%X", cpu.pc);
    ImGui::Text("Stack Pointer: 0x%X", cpu.sp);
    ImGui::Text("ACC: 0x%X\n", cpu.acc);
    ImGui::Text("X: 0x%X", cpu.x);
    ImGui::Text("Y: 0x%X", cpu.y);
    ImGui::Text("Status Hex Value: 0x%X", status.to_ulong());

    ImGui::Text("Status Flags Value: N - %d  V - %d  U - %d  B - %d  D - %d  I - %d  Z - %d  C - %d",
                status.test(7), status.test(6), status.test(5), status.test(4),
                status.test(3), status.test(2), status.test(1), status.test(0));

    ImGui::Separator();

    std::vector<std::string> disassembled_instructions{disassemble(cpu)};

    if (ImGui::BeginTable("Instructions", 1)) {
        for (auto it = disassembled_instructions.begin(); it != disassembled_instructions.end(); ++it) {
            std::string decoded = *it;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(decoded.c_str());
        }
        ImGui::EndTable();
    }

    ImGui::SetCursorPos({353, 250});
    ImGui::PushItemWidth(40);
    ImGui::InputText(" ", breakpoint_str, IM_ARRAYSIZE(breakpoint_str));

    ImGui::SetCursorPos({403, 250});
    if (ImGui::Button("Set Breakpoint", {120, 19})) {
        std::stringstream break_hex;

        break_hex << std::hex << breakpoint_str;
        break_hex >> breakpoint;
        std::memset(&breakpoint_str, (int) '\0', 4);
    }

    ImGui::SetCursorPos({353, 200});
    ImGui::Text("Step: %d", step_num);

    ImGui::SetCursorPos({353, 220});
    ImGui::Text("Breakpoint: 0x%X", breakpoint);

    ImGui::SetCursorPos({353, 351});
    if (ImGui::Button("Run To Breakpoint", {125, 30})) {
        while (cpu.pc != breakpoint) {
            cpu.cycle();
            ++step_num;
        }
    }

    ImGui::SetCursorPos({353, 320});
    if (ImGui::Button("Step", {125, 30})) {
        cpu.cycle();
        ++step_num;
    }

    ImGui::PushItemWidth(40);
    ImGui::SetCursorPos({270, 25});
    ImGui::InputText("", pc_input, IM_ARRAYSIZE(pc_input));

    ImGui::SetCursorPos({320, 25});
    if (ImGui::Button("Set PC", {60, 19})) {
        std::stringstream pc_hex;

        pc_hex << std::hex << pc_input;
        pc_hex >> cpu.pc;
        std::memset(&pc_input, (int) '\0', 4);
    }

    ImGui::SetCursorPos({393, 25});
    ImGui::Text("X: %.2f Y: %.2f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

    ImGui::End();

    ImGui::SetNextWindowSize({552, 595}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({652, 19}, ImGuiCond_Once);

    for (int i = 0; i <= 0xFFFF; i++) {
        cpu_mem_layout[i] = cpu_bus.read(i);
    }

    cpu_mem_edit.DrawWindow("CPU Memory", &cpu_mem_layout, 64 * 1024);
}

void NESConsoleMainWindow::showPpuDebugger(PPU& ppu) {
    ImGui::SetNextWindowSize({552, 595}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({300, 110}, ImGuiCond_Once);

    //ppu_mem_edit.DrawWindow("PPU Memory", ppu.bus.cpu_ram, 16 * 1024);
}

std::vector<std::string> NESConsoleMainWindow::disassemble(CPU6502& cpu) {
    std::vector<std::string> disassembled_instructions;

    uint16_t tmp_pc = cpu.pc;

    for (int i = 0; i < 27; i++) {
        std::stringstream dis_instruction;

        uint8_t op_code = cpu.read(tmp_pc);
        CPU6502::Instruction instruction = cpu.op_map.at(op_code);

        std::string addressing_name = instruction.op_name.substr(4, instruction.op_name.length());
        uint8_t operand_bytes = cpu.operand_bytes.at(addressing_name);
        std::vector<uint8_t> operands;

        dis_instruction << '$' << std::hex << std::setw(4);
        dis_instruction << std::setfill('0') << tmp_pc << "   ";              // Print pc address e.g. $0F01
        dis_instruction << std::hex << std::uppercase << std::setw(2);       //  Print operation
        //   hex value e.g. 0B
        dis_instruction << std::setfill('0') << (uint16_t) op_code << ' ';

        for (int j = 1; j <= operand_bytes; j++) {
            operands.push_back(cpu.read(tmp_pc + j));

            dis_instruction << std::hex << std::uppercase << std::setfill('0');   // Print operands padded with 0
            //  to length 2 e.g. operand 9 = 09
            dis_instruction << std::setw(2) << (uint16_t) operands.back() << ' ';
        }

        for (int i = 0; i < ((2 - operand_bytes) * 3) + 4; ++i) {
            dis_instruction << ' ';
        }

        dis_instruction << instruction.op_name << ' ';

        if (operand_bytes > 0) {
            uint16_t full_operand;

            if (operand_bytes == 1) {
                full_operand = operands[0];
            }
            else if (operand_bytes == 2) {
                full_operand = ((uint16_t) operands[1] << 8) + (uint16_t) operands[0];
            }

            if (addressing_name == "IMD") // If instruction addressing is immediate then
                //  print operand in form #$ instead $
                dis_instruction << '#';
            else if (addressing_name == "REL") // Commented in CPU6502 mod_rel function
                full_operand = tmp_pc + (int8_t) full_operand + 2;

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

void NESConsoleMainWindow::resetHelpers() {
    std::memset(&breakpoint_str, '\0', 4);
    std::memset(&pc_input, '\0', 4);
    breakpoint = 0;
    step_num = 1;
    restart = false;
}