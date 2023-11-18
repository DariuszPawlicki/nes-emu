#pragma once

#include "utils/ChipsCommons.hpp"

#include <string>
#include <cstdint>
#include <fstream>
#include <functional>
#include <unordered_map>


class MainBus;

class CPU6502 {
    constexpr inline static uint16_t PRG_ROM_BEGINNING{0x8000};
    constexpr inline static uint16_t STACK_BEGINNING{0x0100};

public:
	enum class AdditionalCycles {
		Default,
		PageCrossed, // +1 if to a new page
		NewPage // +1 if branch succeedes, +2 if to a new page
	};

    enum Flags {
        Carry,
        Zero,
        InterruptDisable,
        Decimal,
        Break,
        Unused,
        Overflow,
        Negative
    };

    struct Instruction {
        std::string op_name;
        std::function<void()> operation;
        std::function<void()> adr_mod;
        uint8_t cycles;
        AdditionalCycles additional_cycle;

        Instruction() = default;

        Instruction(const std::string& op_name, std::function<void()> operation,
                    std::function<void()> adr_mod, uint8_t cycles, AdditionalCycles additional_cycle) : op_name(op_name),
            operation(operation),
            adr_mod(adr_mod),
            cycles(cycles), additional_cycle(additional_cycle) {
        }

        Instruction(const std::string& op_name, std::function<void()> operation,
                    std::function<void()> adr_mod, uint8_t cycles) : op_name(op_name), operation(operation),
                                                        adr_mod(adr_mod),
                                                        cycles(cycles), additional_cycle(AdditionalCycles::Default) {
        }

        void operator()() const {
            adr_mod();
            operation();
        }

        std::string getAddressingModeName() const {
            auto separator_pos{op_name.find('_')};
            return op_name.substr(separator_pos + 1);
        }
    };

    const std::unordered_map<std::string, uint8_t> operand_bytes{
        {"IMD", 1}, {"ZP", 1}, {"ZPX", 1},
        {"ABS", 2}, {"ABSX", 2}, {"ABSY", 2},
        {"IDRX", 1}, {"IDRY", 1}, {"REL", 1},
        {"IMP", 0}, {"ACC", 0}, {"IDR", 2},
        {"ZPY", 1}, {"???", 0},
    };

    std::shared_ptr<MainBus> main_bus;

    uint16_t instr_operand{0};
    uint16_t target_address{0};
    uint8_t instr_opcode{0};
    uint8_t data_extracted{0}; // Data extracted by addressing mode

    // Registers

    uint16_t pc{0};
    uint8_t sp{0};
    uint8_t acc{0};
    uint8_t x{0};
    uint8_t y{0};

    chips_commons::Register<8> status; // Processor status flags - starting from most significant bit -
                                      //  N V U B D I Z C - Negative, Overflow, Unused, Break cmd, Decimal mode,
                                     //   Interrupt disable, Zero flag, Carry flag

    const std::unordered_map<uint8_t, Instruction> op_map{

        {0x69, Instruction("ADC_IMD", [this]{ this->ADC(); }, [this]{ this->mod_imd(); }, 2)},
        {0x18, Instruction("CLC_IMP", [this]{ this->CLC(); }, [this]{ this->mod_imp(); }, 2)},
        {0x49, Instruction("EOR_IMD", [this]{ this->EOR(); }, [this]{ this->mod_imd(); }, 2)},
        {0xA2, Instruction("LDX_IMD", [this]{ this->LDX(); }, [this]{ this->mod_imd(); }, 2)},
        {0x2A, Instruction("ROL_ACC", [this]{ this->ROL(); }, [this]{ this->mod_acc(); }, 2)},
        {0x86, Instruction("STX_ZP", [this]{ this->STX(); }, [this]{ this->mod_zp(); }, 3)},
        {0x2B, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0x72, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0xBF, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x65, Instruction("ADC_ZP", [this]{ this->ADC(); }, [this]{ this->mod_zp(); }, 3)},
        {0xD8, Instruction("CLD_IMP", [this]{ this->CLD(); }, [this]{ this->mod_imp(); }, 2)},
        {0x45, Instruction("EOR_ZP", [this]{ this->EOR(); }, [this]{ this->mod_zp(); }, 3)},
        {0xA6, Instruction("LDX_ZP", [this]{ this->LDX(); }, [this]{ this->mod_zp(); }, 3)},
        {0x26, Instruction("ROL_ZP", [this]{ this->ROL(); }, [this]{ this->mod_zp(); }, 5)},
        {0x96, Instruction("STX_ZPY", [this]{ this->STX(); }, [this]{ this->mod_zpy(); }, 4)},
        {0x2F, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 6)},
        {0x73, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 8)},
        {0xC2, Instruction("NOP_IMD", [this]{ this->NOP(); }, [this]{ this->mod_imd(); }, 2)},
        {0x75, Instruction("ADC_ZPX", [this]{ this->ADC(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x58, Instruction("CLI_IMP", [this]{ this->CLI(); }, [this]{ this->mod_imp(); }, 2)},
        {0x55, Instruction("EOR_ZPX", [this]{ this->EOR(); }, [this]{ this->mod_zpx(); }, 4)},
        {0xB6, Instruction("LDX_ZPY", [this]{ this->LDX(); }, [this]{ this->mod_zpy(); }, 4)},
        {0x36, Instruction("ROL_ZPX", [this]{ this->ROL(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x8E, Instruction("STX_ABS", [this]{ this->STX(); }, [this]{ this->mod_abs(); }, 4)},
        {0x32, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x74, Instruction("NOP_ZPX", [this]{ this->NOP(); }, [this]{ this->mod_zpx(); }, 4)},
        {0xC3, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 8)},
        {0x6D, Instruction("ADC_ABS", [this]{ this->ADC(); }, [this]{ this->mod_abs(); }, 4)},
        {0xB8, Instruction("CLV_IMP", [this]{ this->CLV(); }, [this]{ this->mod_imp(); }, 2)},
        {0x4D, Instruction("EOR_ABS", [this]{ this->EOR(); }, [this]{ this->mod_abs(); }, 4)},
        {0xAE, Instruction("LDX_ABS", [this]{ this->LDX(); }, [this]{ this->mod_abs(); }, 4)},
        {0x2E, Instruction("ROL_ABS", [this]{ this->ROL(); }, [this]{ this->mod_abs(); }, 6)},
        {0x7D, Instruction("ADC_ABSX", [this]{ this->ADC(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x18, Instruction("CLC_IMP", [this]{ this->CLC(); }, [this]{ this->mod_imp(); }, 2)},
        {0x5D, Instruction("EOR_ABSX", [this]{ this->EOR(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0xBE, Instruction("LDX_ABSY", [this]{ this->LDX(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x3E, Instruction("ROL_ABSX", [this]{ this->ROL(); }, [this]{ this->mod_absx(); }, 7)},
        {0x84, Instruction("STY_ZP", [this]{ this->STY(); }, [this]{ this->mod_zp(); }, 3)},
        {0x33, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 8)},
        {0x77, Instruction("UNK_ZPX", [this]{ this->UNK(); }, [this]{ this->mod_zpx(); }, 6)},
        {0xC7, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 5)},
        {0x79, Instruction("ADC_ABSY", [this]{ this->ADC(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x59, Instruction("EOR_ABSY", [this]{ this->EOR(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x94, Instruction("STY_ZPX", [this]{ this->STY(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x34, Instruction("NOP_ZPX", [this]{ this->NOP(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x7A, Instruction("NOP_???", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0xCB, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0x61, Instruction("ADC_IDRX", [this]{ this->ADC(); }, [this]{ this->mod_idrx(); }, 6)},
        {0x41, Instruction("EOR_IDRX", [this]{ this->EOR(); }, [this]{ this->mod_idrx(); }, 6)},
        {0xA0, Instruction("LDY_IMD", [this]{ this->LDY(); }, [this]{ this->mod_imd(); }, 2)},
        {0x6A, Instruction("ROR_ACC", [this]{ this->ROR(); }, [this]{ this->mod_acc(); }, 2)},
        {0x8C, Instruction("STY_ABS", [this]{ this->STY(); }, [this]{ this->mod_abs(); }, 4)},
        {0x37, Instruction("UNK_ZPX", [this]{ this->UNK(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x7B, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 7)},
        {0xCF, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 6)},
        {0x71, Instruction("ADC_IDRY", [this]{ this->ADC(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0x51, Instruction("EOR_IDRY", [this]{ this->EOR(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0xA4, Instruction("LDY_ZP", [this]{ this->LDY(); }, [this]{ this->mod_zp(); }, 3)},
        {0x66, Instruction("ROR_ZP", [this]{ this->ROR(); }, [this]{ this->mod_zp(); }, 5)},
        {0xB4, Instruction("LDY_ZPX", [this]{ this->LDY(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x76, Instruction("ROR_ZPX", [this]{ this->ROR(); }, [this]{ this->mod_zpx(); }, 6)},
        {0xAA, Instruction("TAX_IMP", [this]{ this->TAX(); }, [this]{ this->mod_imp(); }, 2)},
        {0x3A, Instruction("NOP_???", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0x7C, Instruction("NOP_ABSX", [this]{ this->NOP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0xD2, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x29, Instruction("AND_IMD", [this]{ this->AND(); }, [this]{ this->mod_imd(); }, 2)},
        {0xC9, Instruction("CMP_IMD", [this]{ this->CMP(); }, [this]{ this->mod_imd(); }, 2)},
        {0xE6, Instruction("INC_ZP", [this]{ this->INC(); }, [this]{ this->mod_zp(); }, 5)},
        {0xAC, Instruction("LDY_ABS", [this]{ this->LDY(); }, [this]{ this->mod_abs(); }, 4)},
        {0x6E, Instruction("ROR_ABS", [this]{ this->ROR(); }, [this]{ this->mod_abs(); }, 6)},
        {0xA8, Instruction("TAY_IMP", [this]{ this->TAY(); }, [this]{ this->mod_imp(); }, 2)},
        {0x3B, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 7)},
        {0x7F, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 7)},
        {0xD3, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 8)},
        {0x25, Instruction("AND_ZP", [this]{ this->AND(); }, [this]{ this->mod_zp(); }, 3)},
        {0xC5, Instruction("CMP_ZP", [this]{ this->CMP(); }, [this]{ this->mod_zp(); }, 3)},
        {0xF6, Instruction("INC_ZPX", [this]{ this->INC(); }, [this]{ this->mod_zpx(); }, 6)},
        {0xBC, Instruction("LDY_ABSX", [this]{ this->LDY(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x7E, Instruction("ROR_ABSX", [this]{ this->ROR(); }, [this]{ this->mod_absx(); }, 7)},
        {0xBA, Instruction("TSX_IMP", [this]{ this->TSX(); }, [this]{ this->mod_imp(); }, 2)},
        {0x3C, Instruction("NOP_ABSX", [this]{ this->NOP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x80, Instruction("NOP_IMD", [this]{ this->NOP(); }, [this]{ this->mod_imd(); }, 2)},
        {0xD4, Instruction("NOP_ZPX", [this]{ this->NOP(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x35, Instruction("AND_ZPX", [this]{ this->AND(); }, [this]{ this->mod_zpx(); }, 4)},
        {0xD5, Instruction("CMP_ZPX", [this]{ this->CMP(); }, [this]{ this->mod_zpx(); }, 4)},
        {0xEE, Instruction("INC_ABS", [this]{ this->INC(); }, [this]{ this->mod_abs(); }, 6)},
        {0x8A, Instruction("TXA_IMP", [this]{ this->TXA(); }, [this]{ this->mod_imp(); }, 2)},
        {0x3F, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 7)},
        {0x82, Instruction("NOP_IMD", [this]{ this->NOP(); }, [this]{ this->mod_imd(); }, 2)},
        {0xD7, Instruction("UNK_ZPX", [this]{ this->UNK(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x2D, Instruction("AND_ABS", [this]{ this->AND(); }, [this]{ this->mod_abs(); }, 4)},
        {0xCD, Instruction("CMP_ABS", [this]{ this->CMP(); }, [this]{ this->mod_abs(); }, 4)},
        {0xFE, Instruction("INC_ABSX", [this]{ this->INC(); }, [this]{ this->mod_absx(); }, 7)},
        {0x4A, Instruction("LSR_ACC", [this]{ this->LSR(); }, [this]{ this->mod_acc(); }, 2)},
        {0x40, Instruction("RTI_IMP", [this]{ this->RTI(); }, [this]{ this->mod_imp(); }, 6)},
        {0x9A, Instruction("TXS_IMP", [this]{ this->TXS(); }, [this]{ this->mod_imp(); }, 2)},
        {0x42, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x83, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 6)},
        {0xDA, Instruction("NOP_???", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0x3D, Instruction("AND_ABSX", [this]{ this->AND(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0xDD, Instruction("CMP_ABSX", [this]{ this->CMP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x46, Instruction("LSR_ZP", [this]{ this->LSR(); }, [this]{ this->mod_zp(); }, 5)},
        {0x60, Instruction("RTS_IMP", [this]{ this->RTS(); }, [this]{ this->mod_imp(); }, 6)},
        {0x98, Instruction("TYA_IMP", [this]{ this->TYA(); }, [this]{ this->mod_imp(); }, 2)},
        {0x43, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 8)},
        {0x87, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 3)},
        {0xDB, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 7)},
        {0x39, Instruction("AND_ABSY", [this]{ this->AND(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0xD9, Instruction("CMP_ABSY", [this]{ this->CMP(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0xE8, Instruction("INX_IMP", [this]{ this->INX(); }, [this]{ this->mod_imp(); }, 2)},
        {0x56, Instruction("LSR_ZPX", [this]{ this->LSR(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x21, Instruction("AND_IDRX", [this]{ this->AND(); }, [this]{ this->mod_idrx(); }, 6)},
        {0xC1, Instruction("CMP_IDRX", [this]{ this->CMP(); }, [this]{ this->mod_idrx(); }, 6)},
        {0xC8, Instruction("INY_IMP", [this]{ this->INY(); }, [this]{ this->mod_imp(); }, 2)},
        {0x4E, Instruction("LSR_ABS", [this]{ this->LSR(); }, [this]{ this->mod_abs(); }, 6)},
        {0xE9, Instruction("SBC_IMD", [this]{ this->SBC(); }, [this]{ this->mod_imd(); }, 2)},
        {0x02, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x44, Instruction("NOP_ZP", [this]{ this->NOP(); }, [this]{ this->mod_zp(); }, 3)},
        {0x89, Instruction("NOP_IMD", [this]{ this->NOP(); }, [this]{ this->mod_imd(); }, 2)},
        {0xDC, Instruction("NOP_ABSX", [this]{ this->NOP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x31, Instruction("AND_IDRY", [this]{ this->AND(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0xD1, Instruction("CMP_IDRY", [this]{ this->CMP(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0x5E, Instruction("LSR_ABSX", [this]{ this->LSR(); }, [this]{ this->mod_absx(); }, 7)},
        {0xE5, Instruction("SBC_ZP", [this]{ this->SBC(); }, [this]{ this->mod_zp(); }, 3)},
        {0x03, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 8)},
        {0x47, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 5)},
        {0x8B, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0xDF, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 7)},
        {0xF5, Instruction("SBC_ZPX", [this]{ this->SBC(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x04, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 3)},
        {0x4B, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0x8F, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 4)},
        {0xE2, Instruction("NOP_IMD", [this]{ this->NOP(); }, [this]{ this->mod_imd(); }, 2)},
        {0x0A, Instruction("ASL_ACC", [this]{ this->ASL(); }, [this]{ this->mod_acc(); }, 2)},
        {0xE0, Instruction("CPX_IMD", [this]{ this->CPX(); }, [this]{ this->mod_imd(); }, 2)},
        {0x4C, Instruction("JMP_ABS", [this]{ this->JMP(); }, [this]{ this->mod_abs(); }, 3)},
        {0xEA, Instruction("NOP_IMP", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0xED, Instruction("SBC_ABS", [this]{ this->SBC(); }, [this]{ this->mod_abs(); }, 4)},
        {0x07, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 5)},
        {0x4F, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 6)},
        {0x92, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0xE3, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 8)},
        {0x06, Instruction("ASL_ZP", [this]{ this->ASL(); }, [this]{ this->mod_zp(); }, 5)},
        {0xE4, Instruction("CPX_ZP", [this]{ this->CPX(); }, [this]{ this->mod_zp(); }, 3)},
        {0x6C, Instruction("JMP_IDR", [this]{ this->JMP(); }, [this]{ this->mod_idr(); }, 5)},
        {0xFD, Instruction("SBC_ABSX", [this]{ this->SBC(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x0B, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0x52, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x93, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 6)},
        {0xE7, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 5)},
        {0x16, Instruction("ASL_ZPX", [this]{ this->ASL(); }, [this]{ this->mod_zpx(); }, 6)},
        {0xEC, Instruction("CPX_ABS", [this]{ this->CPX(); }, [this]{ this->mod_abs(); }, 4)},
        {0x20, Instruction("JSR_ABS", [this]{ this->JSR(); }, [this]{ this->mod_abs(); }, 6)},
        {0x09, Instruction("ORA_IMD", [this]{ this->ORA(); }, [this]{ this->mod_imd(); }, 2)},
        {0xF9, Instruction("SBC_ABSY", [this]{ this->SBC(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x0C, Instruction("NOP_ABS", [this]{ this->NOP(); }, [this]{ this->mod_abs(); }, 4)},
        {0x53, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 8)},
        {0x97, Instruction("UNK_ZPY", [this]{ this->UNK(); }, [this]{ this->mod_zpy(); }, 4)},
        {0xEB, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0x0E, Instruction("ASL_ABS", [this]{ this->ASL(); }, [this]{ this->mod_abs(); }, 6)},
        {0x05, Instruction("ORA_ZP", [this]{ this->ORA(); }, [this]{ this->mod_zp(); }, 3)},
        {0xE1, Instruction("SBC_IDRX", [this]{ this->SBC(); }, [this]{ this->mod_idrx(); }, 6)},
        {0x0F, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 6)},
        {0x54, Instruction("NOP_ZPX", [this]{ this->NOP(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x9B, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 5)},
        {0xEF, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 6)},
        {0x1E, Instruction("ASL_ABSX", [this]{ this->ASL(); }, [this]{ this->mod_absx(); }, 7)},
        {0x15, Instruction("ORA_ZPX", [this]{ this->ORA(); }, [this]{ this->mod_zpx(); }, 4)},
        {0xF1, Instruction("SBC_IDRY", [this]{ this->SBC(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0x12, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x57, Instruction("UNK_ZPX", [this]{ this->UNK(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x9C, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 5)},
        {0xF2, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x0D, Instruction("ORA_ABS", [this]{ this->ORA(); }, [this]{ this->mod_abs(); }, 4)},
        {0x90, Instruction("BCC_REL", [this]{ this->BCC(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xC0, Instruction("CPY_IMD", [this]{ this->CPY(); }, [this]{ this->mod_imd(); }, 2)},
        {0xA9, Instruction("LDA_IMD", [this]{ this->LDA(); }, [this]{ this->mod_imd(); }, 2)},
        {0x1D, Instruction("ORA_ABSX", [this]{ this->ORA(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x38, Instruction("SEC_IMP", [this]{ this->SEC(); }, [this]{ this->mod_imp(); }, 2)},
        {0x13, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 8)},
        {0x5A, Instruction("NOP_???", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0x9E, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 5)},
        {0xF3, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 8)},
        {0xB0, Instruction("BCS_REL", [this]{ this->BCS(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xC4, Instruction("CPY_ZP", [this]{ this->CPY(); }, [this]{ this->mod_zp(); }, 3)},
        {0xA5, Instruction("LDA_ZP", [this]{ this->LDA(); }, [this]{ this->mod_zp(); }, 3)},
        {0x19, Instruction("ORA_ABSY", [this]{ this->ORA(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0xF8, Instruction("SED_IMP", [this]{ this->SED(); }, [this]{ this->mod_imp(); }, 2)},
        {0x14, Instruction("NOP_ZPX", [this]{ this->NOP(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x5B, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 7)},
        {0x9F, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 5)},
        {0xF4, Instruction("NOP_ZPX", [this]{ this->NOP(); }, [this]{ this->mod_zpx(); }, 4)},
        {0xF0, Instruction("BEQ_REL", [this]{ this->BEQ(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xCC, Instruction("CPY_ABS", [this]{ this->CPY(); }, [this]{ this->mod_abs(); }, 4)},
        {0xB5, Instruction("LDA_ZPX", [this]{ this->LDA(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x01, Instruction("ORA_IDRX", [this]{ this->ORA(); }, [this]{ this->mod_idrx(); }, 6)},
        {0x78, Instruction("SEI_IMP", [this]{ this->SEI(); }, [this]{ this->mod_imp(); }, 2)},
        {0x17, Instruction("UNK_ZPX", [this]{ this->UNK(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x5C, Instruction("NOP_ABSX", [this]{ this->NOP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0xA3, Instruction("LAX_IDRX", [this]{ this->LAX(); }, [this]{ this->mod_idrx(); }, 6)},
        {0xF7, Instruction("UNK_ZPX", [this]{ this->UNK(); }, [this]{ this->mod_zpx(); }, 6)},
        {0x30, Instruction("BMI_REL", [this]{ this->BMI(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xAD, Instruction("LDA_ABS", [this]{ this->LDA(); }, [this]{ this->mod_abs(); }, 4)},
        {0x11, Instruction("ORA_IDRY", [this]{ this->ORA(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0xD0, Instruction("BNE_REL", [this]{ this->BMI(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xC6, Instruction("DEC_ZP", [this]{ this->DEC(); }, [this]{ this->mod_zp(); }, 5)},
        {0xBD, Instruction("LDA_ABSX", [this]{ this->LDA(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x85, Instruction("STA_ZP", [this]{ this->STA(); }, [this]{ this->mod_zp(); }, 3)},
        {0x1A, Instruction("NOP_???", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0x5F, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 7)},
        {0xA7, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 3)},
        {0xFA, Instruction("NOP_???", [this]{ this->NOP(); }, [this]{ this->mod_imp(); }, 2)},
        {0x10, Instruction("BPL_REL", [this]{ this->BPL(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xD6, Instruction("DEC_ZPX", [this]{ this->DEC(); }, [this]{ this->mod_zpx(); }, 6)},
        {0xB9, Instruction("LDA_ABSY", [this]{ this->LDA(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x48, Instruction("PHA_IMP", [this]{ this->PHA(); }, [this]{ this->mod_imp(); }, 3)},
        {0x95, Instruction("STA_ZPX", [this]{ this->STA(); }, [this]{ this->mod_zpx(); }, 4)},
        {0x1B, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 7)},
        {0x62, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0xAB, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0xFB, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 7)},
        {0x50, Instruction("BVC_REL", [this]{ this->BVC(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xCE, Instruction("DEC_ABS", [this]{ this->DEC(); }, [this]{ this->mod_abs(); }, 6)},
        {0xA1, Instruction("LDA_IDRX", [this]{ this->LDA(); }, [this]{ this->mod_idrx(); }, 6)},
        {0x08, Instruction("PHP_IMP", [this]{ this->PHP(); }, [this]{ this->mod_imp(); }, 3)},
        {0x8D, Instruction("STA_ABS", [this]{ this->STA(); }, [this]{ this->mod_abs(); }, 4)},
        {0x1C, Instruction("NOP_ABSX", [this]{ this->NOP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x63, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 8)},
        {0xAF, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 4)},
        {0xFC, Instruction("NOP_ABSX", [this]{ this->NOP(); }, [this]{ this->mod_absx(); }, 4, AdditionalCycles::PageCrossed)},
        {0x70, Instruction("BVS_REL", [this]{ this->BVS(); }, [this]{ this->mod_rel(); }, 2, AdditionalCycles::NewPage)},
        {0xDE, Instruction("DEC_ABSX", [this]{ this->DEC(); }, [this]{ this->mod_absx(); }, 7)},
        {0xB1, Instruction("LDA_IDRY", [this]{ this->LDA(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0x68, Instruction("PLA_IMP", [this]{ this->PLA(); }, [this]{ this->mod_imp(); }, 4)},
        {0x9D, Instruction("STA_ABSX", [this]{ this->STA(); }, [this]{ this->mod_absx(); }, 5)},
        {0x1F, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 7)},
        {0x64, Instruction("NOP_ZP", [this]{ this->NOP(); }, [this]{ this->mod_zp(); }, 3)},
        {0xB2, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0xFF, Instruction("UNK_ABSX", [this]{ this->UNK(); }, [this]{ this->mod_absx(); }, 7)},
        {0x28, Instruction("PLP_IMP", [this]{ this->PLP(); }, [this]{ this->mod_imp(); }, 4)},
        {0x99, Instruction("STA_ABSY", [this]{ this->STA(); }, [this]{ this->mod_absy(); }, 5)},
        {0x22, Instruction("UNK_???", [this]{ this->UNK(); }, [this]{ this->mod_imp(); }, 2)},
        {0x67, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 5)},
        {0xB3, Instruction("UNK_IDRY", [this]{ this->UNK(); }, [this]{ this->mod_idry(); }, 5, AdditionalCycles::PageCrossed)},
        {0x24, Instruction("BIT_ZP", [this]{ this->BIT(); }, [this]{ this->mod_zp(); }, 3)},
        {0xCA, Instruction("DEX_IMP", [this]{ this->DEX(); }, [this]{ this->mod_imp(); }, 2)},
        {0x81, Instruction("STA_IDRX", [this]{ this->STA(); }, [this]{ this->mod_idrx(); }, 6)},
        {0x23, Instruction("UNK_IDRX", [this]{ this->UNK(); }, [this]{ this->mod_idrx(); }, 8)},
        {0x6B, Instruction("UNK_IMD", [this]{ this->UNK(); }, [this]{ this->mod_imd(); }, 2)},
        {0xB7, Instruction("UNK_ZPY", [this]{ this->UNK(); }, [this]{ this->mod_zpy(); }, 4)},
        {0x2C, Instruction("BIT_ABS", [this]{ this->BIT(); }, [this]{ this->mod_abs(); }, 4)},
        {0x88, Instruction("DEY_IMP", [this]{ this->DEY(); }, [this]{ this->mod_imp(); }, 2)},
        {0x91, Instruction("STA_IDRY", [this]{ this->STA(); }, [this]{ this->mod_idry(); }, 6)},
        {0x27, Instruction("UNK_ZP", [this]{ this->UNK(); }, [this]{ this->mod_zp(); }, 5)},
        {0x6F, Instruction("UNK_ABS", [this]{ this->UNK(); }, [this]{ this->mod_abs(); }, 6)},
        {0xBB, Instruction("UNK_ABSY", [this]{ this->UNK(); }, [this]{ this->mod_absy(); }, 4, AdditionalCycles::PageCrossed)},
        {0x00, Instruction("BRK_IMP", [this]{ this->BRK(); }, [this]{ this->mod_imp(); }, 7)},
    };

public:
    void connectToBus(std::shared_ptr<MainBus> main_bus);

    void powerUp();

    void cycle();

    void write(uint16_t address, uint8_t data);

    [[nodiscard]] uint8_t read(uint16_t address) const;

    [[nodiscard]] bool extractFlag(Flags flag) const;

    void setFlag(Flags flag, bool flag_value);

    void stackPush(uint8_t data);

    [[nodiscard]] uint8_t stackPop();

    void reset();

    void irq();

    void nmi();

    // Instructions

    void ADC(); // Adds memory content and carry flag to accumulator,
    // if overflow occurs on bit 7, then carry flag is set to 1,
    // zero flag is set when A = 0, negative flag is set when bit 7 is 1

    void AND(); // AND operation on accumulator, zero flag is set when A = 0,
    // negative flag is set when bit 7 is 1

    void ASL(); // Arithmetic shift left by 1 position on accumulator or memory,
    // bit 0 is set to 0 and bit 7 is placed in carry flag,
    // zero flag is set when A = 0, negative flag is set when bit 7 of result is 1

    void BCC(); // If carry flag is 0 then pc is set to pc + operand

    void BCS(); // Same as BCC but is executed when carry flag is 1

    void BEQ(); // If zero flag is 1 then pc = pc + operand

    void BIT(); // If Memory AND Accumulator = 0 then zero flag is set, V flag is set to 6 bit of Memory
    // and N flag is set to 7 bit of memory

    void BMI(); // If negative flag is set then pc = pc + operand

    void BNE(); // If zero flag is set then pc = pc + operand

    void BPL(); // If negative flag is 0 then pc += operand

    void BRK(); // PC and status flags are pushed into the stack,
    // interrupt vector $FFFE/F is loaded to PC and break flag is set to 1

    void BVC(); // Branch if V flag is clear

    void BVS(); // Branch if V flag is set

    void CLC(); // Set carry flag to 0

    void CLD(); // Decimal flag is set to 0

    void CLI(); // Interrupt flag is set to 0

    void CLV(); // Overflow flag is set to 0

    void CMP(); // Compares accumulator with memory, if A == M zero flag is set to 1,
    // if A >= M carry flag is set to 1, if 7 bit of result is set then
    // negative flag is set to 1

    void CPX(); // Same as CMP but compares X register with memory

    void CPY(); // Same as CPX but compares Y register with memory

    void DEC(); // Decrement value in memory, zero flag = 1 if value is 0,
    // negative flag = 1 if bit 7 of result is set

    void DEX(); // Decrement X register, flags are set in same way as DEC

    void DEY(); // Same as DEX but peformed on Y register

    void EOR(); // XOR performed on accumulator and value in memory, zero flag = 1 if
    // accumulator is 0, negative flag = 1 if bit 7 of result is set to 1

    void INC(); // Increment value in memory, same flag settings as in DEC

    void INX(); // Increment X register, same flags as INC

    void INY(); // Increment Y register, same flags as INX

    void JMP(); // Sets the PC to address specified by operand

    void JSR(); // Pushes return address onto the stack and sets PC to
    // target memory address

    void LDA(); // Loads a value from memory to accumulator, zero flag = 1
    // if A = 0, negative flag = 1 if bit 7 of result is set to 1

    void LDX(); // Same as LDA but loads to X register

    void LDY(); // Same as LDX but loads to Y register

    void LSR(); // Shift right of accumulator or memory, carry flag = content of old bit 0,
    // zero flag = 1 if result is 0, negative flag = 1 if bit 7 is set to 1

    void NOP(); // No operation, increments PC += 2

    void ORA(); // OR performed on accumulator with value from memory, zero flag = 1 if a = 0,
    // negative flag = 1 if bit 7 is set to 1

    void PHA(); // Pushes copy of accumulator onto the stack

    void PHP(); // Pushes flags byte onto the stack

    void PLA(); // Pulls byte from stack and puts it into the accumulator
    // zero flag if a = 0, negative flag if bit 7 is 1

    void PLP(); // Pulls byte of processor status from stack into processor flags byte

    void ROL(); // Move each bit from A or M to the left, bit 0 = current carry flag,
    // carry flag = old bit 7, zero flag = 1 if a = 0, negative flag if bit 7 is 1

    void ROR(); // Move each bit from A or M to the right, bit 7 = current carry flag,
    // carry flag = old bit 0, zero flag if a = 0, negative flag if bit 7 is 1

    void RTI(); // Used in the end of interrupt routine, first pulls from stack
    // byte of processor flags, then PC

    void RTS(); // Return from subroutine, pulls PC - 1 from the stack

    void SBC(); // Subtract with carry, subtract memory value from A

    void SEC(); // Set carry flag to 1

    void SED(); // Set decimal flag to 1

    void SEI(); // Set interrupt disable flag to 1

    void STA() const; // Store accumulator in memory address

    void STX() const; // Store X register in memory address

    void STY() const; // Store Y register

    void TAX(); // Transfer accumulator to X, zero flag if A = 0,
    // negative flag if bit 7 is 1

    void TAY(); // Same as TAX but performed on Y register

    void TSX(); // Transfers stack pointer to X register, zero flag if X = 0,
    // negative flag if bit 7 of X is set to 1

    void TXA(); // Transfers X to accumulator, zero flag if A = 0,
    // negative flag if bit 7 of A is 1

    void TXS(); // Transfers X to stack pointer

    void TYA(); // Transfers Y to accumulator, zero flag if A = 0,
    // negative flag if bit 7 of A is 1

    void UNK() const; // Unknown - same as NOP, but executed by instructions with illegal opcodes

    void LAX() const;

    // Addressing modes

    void mod_abs(); // Absolute - use 2 bytes operand as address e.g. JMP $a000

    void mod_acc(); // Accumulator - operation performed on accumulator

    void mod_zp(); // Zero page - use 1 byte operand as address e.g. STA $01

    void mod_zpx(); // Zero page X - use 2 bytes operand, first byte is start location at memory
    // second is an offset e.g. STA $01, X where X = #$02, will store
    // accumulator in memory address $03

    void mod_zpy(); // Zero page Y - same as ZPX but can only be used with
    // for storing, loading from/to X register e.g. STX, LDX

    void mod_absx(); // Absolute X - same as zero page, but uses 2 bytes address $a000, X

    void mod_absy(); // Absolute Y - same as ABSX but uses an Y register

    void mod_imd(); // Immediate - storing operand value in certain register specified
    // by opcode e.g. LDX #$01, LDY #$01, stores 1 in X and Y registers

    void mod_rel(); // Relative - it takes number of bytes as operand that
    // are specifying an offset from current instruction which is added to PC,
    // it's used for branching

    void mod_imp() const; // Implicit - instuctions like for example INX don't take any operand,
    // memory location is implied by the opcode

    void mod_idr(); // Indirect - takes an absolute address as a location of least significant
    // byte of target address, next byte is the most significant byte of an address,
    // this addressing mode concatenates those two bytes into address of
    // target memory location e.g. $2000 = $01, $2001 = $02, LDX ($2000)
    // concatenate bytes from two memory cells into address $0201 and
    // load content of this cell into X register
    // It works only with JMP instruction

    void mod_idrx(); // Indexed indirect X - combination of zero page X and indirect addressing
    // e.g. $03 = $a0, $04 = $ff, X = $02, LDA($01, X) will load to accumulator
    // content of $ffa0 memory cell

    void mod_idry(); // Indirect indexed Y - similar to indexed indirect X, but
    // content of Y register is added after derefernce
    // for example $01 = $a0, $02 = $bc, Y = $01, LDA($01),Y ,  will
    // load to accumulator content of memory cell with address $bca1
};
