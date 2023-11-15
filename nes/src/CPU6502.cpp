#include "MainBus.hpp"
#include "CPU6502.hpp"

// linijka 3637 - błąd testu

void CPU6502::connectToBus(std::shared_ptr<MainBus> main_bus) { this->main_bus = std::move(main_bus); }


void CPU6502::powerUp() {
    uint8_t vector_lsb = main_bus->read(0xFFFC); // Setting program counter to address
    //  stored in reset vector
    uint8_t vector_msb = main_bus->read(0xFFFD);

    pc = static_cast<uint16_t>(vector_msb << 8) + static_cast<uint16_t>(vector_lsb);
    status.setByteValue(0x34);
    acc = 0;
    x = 0;
    y = 0;
    sp = 0xFD;
    main_bus->write(0x4015, 0);
    main_bus->write(0x4017, 0);

    for (int i = 0; i <= 16; ++i) {
        main_bus->write(0x4000 + i, 0);
    }

    for (int i = 0; i <= 3; ++i) {
        main_bus->write(0x4010 + i, 0);
    }
}


void CPU6502::cycle() {
    // Fetch
    instr_opcode = main_bus->read(pc);

    // Decode
    CPU6502::Instruction cur_instruction = op_map[instr_opcode];
    std::string addressing_name = cur_instruction.op_name.substr(4, cur_instruction.op_name.length());
    uint8_t operand_bytes = this->operand_bytes[addressing_name];

    ++pc;

    if (operand_bytes == 1) {
        instr_operand = main_bus->read(pc);
        ++pc;
    }
    else if (operand_bytes == 2) {
        instr_operand = main_bus->read(pc);
        ++pc;
        instr_operand += main_bus->read(pc) << 8;
        ++pc;
    }

    // Execute
    (this->*(cur_instruction.adr_mod))();
    (this->*(cur_instruction.operation))();
}

void CPU6502::clearMemory() {}

void CPU6502::write(uint16_t address, uint8_t data) { main_bus->write(address, data); }

uint8_t CPU6502::read(uint16_t address) { return main_bus->read(address); }

bool CPU6502::extractFlag(Flags flag) { return status.getBit(flag); }

void CPU6502::setFlag(Flags flag, bool flag_value) { status.setBit(flag, flag_value); }

void CPU6502::stackPush(uint8_t data) {
    main_bus->write(STACK_BEGINNING + sp, data);
    --sp;
}

uint8_t CPU6502::stackPop() {
    uint8_t data = main_bus->read(STACK_BEGINNING + (++sp));
    return data;
}

void CPU6502::reset() {
    if (!extractFlag(InterruptDisable)) {
        uint8_t vector_lsb = main_bus->read(0xFFFC);
        uint8_t vector_msb = main_bus->read(0xFFFD);

        pc = (uint16_t) (vector_msb << 8) + (uint16_t) vector_lsb;
        setFlag(InterruptDisable, true);
        sp -= 3;
    }
}

void CPU6502::irq() {
    if (!extractFlag(InterruptDisable)) {
        uint8_t vector_lsb = main_bus->read(0xFFFE);
        uint8_t vector_msb = main_bus->read(0xFFFF);

        pc = (uint16_t) (vector_msb << 8) + (uint16_t) vector_lsb;
    }
}

void CPU6502::nmi() {
    uint8_t vector_lsb = main_bus->read(0xFFFA);
    uint8_t vector_msb = main_bus->read(0xFFFB);

    pc = (uint16_t) (vector_msb << 8) + (uint16_t) vector_lsb;
}

void CPU6502::ADC() {
    uint8_t memory_data = data_extracted;
    bool carry = extractFlag(Carry);
    uint16_t result = acc + memory_data + carry;

    bool acc_7b = (acc & 0x80) >> 7;
    bool memory_7b = (memory_data & 0x80) >> 7;
    bool r_7b = (result & 0x80) >> 7;

    setFlag(Overflow, ~(acc_7b ^ memory_7b) & (acc_7b ^ r_7b));
    setFlag(Carry, result > 255);

    acc = result;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::AND() {
    acc &= data_extracted;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::ASL() {
    setFlag(Carry, data_extracted >= 128);

    data_extracted <<= 1;

    if (op_map[instr_opcode].adr_mod == &CPU6502::mod_acc) {
        acc = data_extracted;
    }
    else {
        main_bus->write(target_address, data_extracted);
    }

    setFlag(Zero, data_extracted == 0);
    setFlag(Negative, data_extracted >= 128);
}

void CPU6502::BCC() {
    if (!extractFlag(Carry)) {
        pc = instr_operand;
    }
}

void CPU6502::BCS() {
    if (extractFlag(Carry)) {
        pc = instr_operand;
    }
}

void CPU6502::BEQ() {
    if (extractFlag(Zero)) {
        pc = instr_operand;
    }
}

void CPU6502::BIT() {
    uint8_t data = data_extracted;

    setFlag(Overflow, ((data & 0x40) >> 6) == 1);
    setFlag(Negative, ((data & 0x80) >> 7) == 1);

    data &= acc;

    setFlag(Zero, data == 0);
}

void CPU6502::BMI() {
    if (extractFlag(Negative)) {
        pc = instr_operand;
    }
}

void CPU6502::BNE() {
    if (!extractFlag(Zero)) {
        pc = instr_operand;
    }
}

void CPU6502::BPL() {
    if (!extractFlag(Negative)) {
        pc = instr_operand;
    }
}

void CPU6502::BRK() {
    stackPush((pc >> 8) & 0x00FF);
    stackPush(pc & 0x00FF);

    uint8_t irq_lsb = main_bus->read(0xFFFE);
    uint8_t irq_msb = main_bus->read(0xFFFF);

    pc = ((uint16_t) irq_msb << 8) + (uint16_t) irq_lsb;

    setFlag(InterruptDisable, true);
    setFlag(Break, true);
    setFlag(Unused, true);

    stackPush(status.getByteValue());

    setFlag(Break, false);
}

void CPU6502::BVC() {
    if (!extractFlag(Overflow)) {
        pc = instr_operand;
    }
}

void CPU6502::BVS() {
    if (extractFlag(Overflow)) {
        pc = instr_operand;
    }
}

void CPU6502::CLC() {
    setFlag(Carry, false);
}

void CPU6502::CLD() {
    setFlag(Decimal, false);
}

void CPU6502::CLI() {
    setFlag(InterruptDisable, false);
}

void CPU6502::CLV() {
    setFlag(Overflow, false);
}

void CPU6502::CMP() {
    uint8_t data = data_extracted;

    setFlag(Carry, acc >= data);
    setFlag(Zero, acc == data);
    setFlag(Negative, (uint8_t) (acc - data) >= 128);
}

void CPU6502::CPX() {
    uint8_t data = data_extracted;

    setFlag(Carry, x >= data);
    setFlag(Zero, x == data);
    setFlag(Negative, (uint8_t) (x - data) >= 128);
}

void CPU6502::CPY() {
    uint8_t data = data_extracted;

    setFlag(Carry, y >= data);
    setFlag(Zero, y == data);
    setFlag(Negative, (uint8_t) (y - data) >= 128);
}

void CPU6502::DEC() {
    data_extracted--;

    main_bus->write(target_address, data_extracted);

    setFlag(Zero, data_extracted == 0);
    setFlag(Negative, data_extracted >= 128);
}

void CPU6502::DEX() {
    --x;

    setFlag(Zero, x == 0);
    setFlag(Negative, x >= 128);
}

void CPU6502::DEY() {
    --y;

    setFlag(Zero, y == 0);
    setFlag(Negative, y >= 128);
}

void CPU6502::EOR() {
    acc ^= data_extracted;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::INC() {
    ++data_extracted;

    main_bus->write(target_address, data_extracted);

    setFlag(Zero, data_extracted == 0);
    setFlag(Negative, data_extracted >= 128);
}

void CPU6502::INX() {
    ++x;

    setFlag(Zero, x == 0);
    setFlag(Negative, x >= 128);
}

void CPU6502::INY() {
    ++y;

    setFlag(Zero, y == 0);
    setFlag(Negative, y >= 128);
}

void CPU6502::JMP() {
    pc = instr_operand;
}

void CPU6502::JSR() {
    --pc;

    stackPush(pc >> 8);
    stackPush(pc & 0x00FF);

    pc = instr_operand;
}

void CPU6502::LDA() {
    acc = data_extracted;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::LDX() {
    x = data_extracted;

    setFlag(Zero, x == 0);
    setFlag(Negative, x >= 128);
}

void CPU6502::LDY() {
    y = data_extracted;

    setFlag(Zero, y == 0);
    setFlag(Negative, y >= 128);
}

void CPU6502::LSR() {
    setFlag(Carry, data_extracted % 2);

    data_extracted >>= 1;

    if (op_map[instr_opcode].adr_mod == &CPU6502::mod_acc) {
        acc = data_extracted;
    }
    else {
        main_bus->write(target_address, data_extracted);
    }

    setFlag(Zero, data_extracted == 0);
    setFlag(Negative, data_extracted >= 128);
}

void CPU6502::NOP() {
    return;
}

void CPU6502::ORA() {
    acc |= data_extracted;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::PHA() {
    stackPush(acc);
}

void CPU6502::PHP() {
    setFlag(Unused, true);
    setFlag(Break, true);

    stackPush(status.getByteValue());

    setFlag(Unused, false);
    setFlag(Break, false);
}

void CPU6502::PLA() {
    acc = stackPop();

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::PLP() {
    status.setByteValue(stackPop());
    setFlag(Unused, true);
}

void CPU6502::ROL() {
    bool carry = extractFlag(Carry);

    status.setByteValue(status.getByteValue() & ~0x1);
    status.setByteValue(status.getByteValue() | (data_extracted & 0x80) >> 7);

    data_extracted <<= 1;
    data_extracted += carry;

    if (op_map[instr_opcode].adr_mod == &CPU6502::mod_acc) {
        acc = data_extracted;
    }
    else {
        main_bus->write(target_address, data_extracted);
    }

    setFlag(Zero, data_extracted == 0);
    setFlag(Negative, data_extracted >= 128);
}

void CPU6502::ROR() {
    bool carry = extractFlag(Carry);

    status.setByteValue(status.getByteValue() & ~0x1);
    status.setByteValue(status.getByteValue() | (data_extracted & 0x1));

    data_extracted >>= 1;
    data_extracted |= carry << 7;

    if (op_map[instr_opcode].adr_mod == &CPU6502::mod_acc) {
        acc = data_extracted;
    }
    else {
        main_bus->write(target_address, data_extracted);
    }

    setFlag(Zero, data_extracted == 0);
    setFlag(Negative, data_extracted >= 128);
}

void CPU6502::RTI() {
    status.setByteValue(stackPop());

    setFlag(Unused, false);
    setFlag(Break, false);

    pc = (uint16_t) stackPop() + ((uint16_t) stackPop() << 8);
}

void CPU6502::RTS() {
    pc = (uint16_t) stackPop();
    pc += ((uint16_t) stackPop() << 8) + 1;
}

void CPU6502::SBC() {
    uint8_t memory_data = data_extracted;
    bool carry = extractFlag(Carry);
    uint16_t result = acc + ~memory_data + carry;

    bool acc_7b = (acc & 0x80) >> 7;
    bool memory_7b = (memory_data & 0x80) >> 7;
    bool r_7b = (result & 0x80) >> 7;

    setFlag(Overflow, (acc_7b ^ memory_7b) & (acc_7b ^ r_7b));
    setFlag(Carry, !(result > 255));

    acc = result;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::SEC() {
    setFlag(Carry, true);
}

void CPU6502::SED() {
    setFlag(Decimal, true);
}

void CPU6502::SEI() {
    setFlag(InterruptDisable, true);
}

void CPU6502::STA() {
    main_bus->write(target_address, acc);
}

void CPU6502::STX() {
    main_bus->write(target_address, x);
}

void CPU6502::STY() {
    main_bus->write(target_address, y);
}

void CPU6502::TAX() {
    x = acc;

    setFlag(Zero, x == 0);
    setFlag(Negative, x >= 128);
}

void CPU6502::TAY() {
    y = acc;

    setFlag(Zero, y == 0);
    setFlag(Negative, y >= 128);
}

void CPU6502::TSX() {
    x = sp;

    setFlag(Zero, x == 0);
    setFlag(Negative, x >= 128);
}

void CPU6502::TXA() {
    acc = x;

    setFlag(Zero, acc == 0);
    setFlag(Negative, acc >= 128);
}

void CPU6502::TXS() {
    sp = x;
}

void CPU6502::TYA() {
    acc = y;

    setFlag(Zero, y == 0);
    setFlag(Negative, y >= 128);
}

void CPU6502::UNK() {
    return;
}

void CPU6502::LAX() {

}

void CPU6502::mod_abs() {
    target_address = instr_operand;
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_acc() {
    data_extracted = acc;
}

void CPU6502::mod_zp() {
    target_address = (instr_operand & 0x00FF);
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_zpx() {
    target_address = (instr_operand + x) & 0x00FF;
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_zpy() {
    target_address = (instr_operand + y) & 0x00FF;
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_absx() {
    target_address = instr_operand + x;
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_absy() {
    target_address = instr_operand + y;
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_imd() {
    data_extracted = instr_operand;
}

void CPU6502::mod_rel() {
    int8_t signed_operand = instr_operand;
    instr_operand = signed_operand + pc;
}

void CPU6502::mod_imp() {
    return;
}

void CPU6502::mod_idr() {
    uint8_t least_significant_byte = main_bus->read(instr_operand);
    uint8_t most_significant_byte;

    /* Implementation of hardware bug. If least significant byte is 0xFF,
       then page is not crossed when reading most significant byte, instead
       address is wrapped, so if operand is $02FF, then lsb is read from
       $02FF and msb is read from $0200 instead of $0300. */

    if ((instr_operand & 0x00FF) == 0x00FF) {
        most_significant_byte = main_bus->read(instr_operand & 0xFF00);
    }
    else {
        most_significant_byte = main_bus->read(instr_operand + 1);
    }

    target_address = ((uint16_t) most_significant_byte << 8) + (uint16_t) least_significant_byte;
    instr_operand = target_address;
}

void CPU6502::mod_idrx() {
    uint8_t lsb_loc = instr_operand + x;
    uint8_t msb_loc = instr_operand + x + 1;

    uint8_t least_significant_byte = main_bus->read(lsb_loc);
    uint8_t most_significant_byte = main_bus->read(msb_loc);

    target_address = ((uint16_t) most_significant_byte << 8) + (uint16_t) least_significant_byte;
    data_extracted = main_bus->read(target_address);
}

void CPU6502::mod_idry() {
    uint8_t lsb_loc = (instr_operand & 0x00FF);
    uint8_t msb_loc = (instr_operand & 0x00FF) + 1;

    uint8_t least_significant_byte = main_bus->read(lsb_loc);
    uint8_t most_significant_byte = main_bus->read(msb_loc);

    target_address = ((uint16_t) most_significant_byte << 8) + (uint16_t) least_significant_byte + y;
    data_extracted = main_bus->read(target_address);
}