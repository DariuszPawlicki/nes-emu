#pragma once

#include "Byte.hpp"

#include <string>
#include <fstream>
#include <unordered_map>


class CpuBus;

class CPU6502
{
	typedef void (CPU6502::*func_ptr)();

public:

	const uint16_t PRG_ROM_BEGINNING = 0x8000;
	const uint16_t STACK_BEGINNING = 0x0100;

	enum AdditionalCycles {
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
		func_ptr operation;
		func_ptr adr_mod;
		uint8_t cycles;
		AdditionalCycles additional_cycle;

		Instruction(std::string op_name, func_ptr operation,
					func_ptr adr_mod, uint8_t cycles, AdditionalCycles additional_cycle) :
														op_name(op_name), operation(operation),
														adr_mod(adr_mod), 
														cycles(cycles), additional_cycle(additional_cycle) {}

		Instruction(std::string op_name, func_ptr operation,
					func_ptr adr_mod, uint8_t cycles) :
										op_name(op_name), operation(operation),
										adr_mod(adr_mod), 
										cycles(cycles), additional_cycle(Default) {}

		Instruction() {}
	};

	std::unordered_map<std::string, uint8_t> operand_bytes{
		{ "IMD", 1 }, { "ZP", 1 }, { "ZPX", 1 },
		{ "ABS", 2 }, { "ABSX", 2 }, { "ABSY", 2 },
		{ "IDRX", 1 }, { "IDRY", 1 }, { "REL", 1 },
		{ "IMP", 0 }, { "ACC", 0 }, { "IDR", 2 },
		{ "ZPY", 1 }, { "???", 0 },
	};

	CpuBus* cpu_bus = nullptr;

	uint16_t instr_operand{ 0 };
	uint16_t target_address{ 0 };
	uint8_t instr_opcode{ 0 };
	uint8_t data_extracted{ 0 };  // Data extracted by addressing mode

	// Registers

	uint16_t pc{ 0 };
	uint8_t sp{ 0 };
	uint8_t acc{ 0 };
	uint8_t x{ 0 };
	uint8_t y{ 0 };

	Byte status;   // Processor status flags - starting from most significant bit -
				  //  N V U B D I Z C - Negative, Overflow, Unused, Break cmd, Decimal mode, 
			     //   Interrupt disable, Zero flag, Carry flag

	std::unordered_map<uint8_t, Instruction> op_map{

		{0x69, Instruction("ADC_IMD", &CPU6502::ADC, &CPU6502::mod_imd, 2)},						{0x18, Instruction("CLC_IMP", &CPU6502::CLC, &CPU6502::mod_imp, 2)},							{0x49, Instruction("EOR_IMD", &CPU6502::EOR, &CPU6502::mod_imd, 2)},							{0xA2, Instruction("LDX_IMD", &CPU6502::LDX, &CPU6502::mod_imd, 2)},							{0x2A, Instruction("ROL_ACC", &CPU6502::ROL, &CPU6502::mod_acc, 2)},							{0x86, Instruction("STX_ZP", &CPU6502::STX, &CPU6502::mod_zp, 3)},								{0x2B, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},						{0x72, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},							{0xBF, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 4, PageCrossed)},
		{0x65, Instruction("ADC_ZP", &CPU6502::ADC, &CPU6502::mod_zp, 3)},							{0xD8, Instruction("CLD_IMP", &CPU6502::CLD, &CPU6502::mod_imp, 2)},							{0x45, Instruction("EOR_ZP", &CPU6502::EOR, &CPU6502::mod_zp, 3)},								{0xA6, Instruction("LDX_ZP", &CPU6502::LDX, &CPU6502::mod_zp, 3)},								{0x26, Instruction("ROL_ZP", &CPU6502::ROL, &CPU6502::mod_zp, 5)},								{0x96, Instruction("STX_ZPY", &CPU6502::STX, &CPU6502::mod_zpy, 4)},							{0x2F, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 6)},						{0x73, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 8)},							{0xC2, Instruction("NOP_IMD", &CPU6502::NOP, &CPU6502::mod_imd, 2)},
		{0x75, Instruction("ADC_ZPX", &CPU6502::ADC, &CPU6502::mod_zpx, 4)},						{0x58, Instruction("CLI_IMP", &CPU6502::CLI, &CPU6502::mod_imp, 2)},							{0x55, Instruction("EOR_ZPX", &CPU6502::EOR, &CPU6502::mod_zpx, 4)},							{0xB6, Instruction("LDX_ZPY", &CPU6502::LDX, &CPU6502::mod_zpy, 4)},							{0x36, Instruction("ROL_ZPX", &CPU6502::ROL, &CPU6502::mod_zpx, 6)},							{0x8E, Instruction("STX_ABS", &CPU6502::STX, &CPU6502::mod_abs, 4)},							{0x32, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},						{0x74, Instruction("NOP_ZPX", &CPU6502::NOP, &CPU6502::mod_zpx, 4)},							{0xC3, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 8)},
		{0x6D, Instruction("ADC_ABS", &CPU6502::ADC, &CPU6502::mod_abs, 4)},						{0xB8, Instruction("CLV_IMP", &CPU6502::CLV, &CPU6502::mod_imp, 2)},							{0x4D, Instruction("EOR_ABS", &CPU6502::EOR, &CPU6502::mod_abs, 4)},							{0xAE, Instruction("LDX_ABS", &CPU6502::LDX, &CPU6502::mod_abs, 4)},							{0x2E, Instruction("ROL_ABS", &CPU6502::ROL, &CPU6502::mod_abs, 6)},
		{0x7D, Instruction("ADC_ABSX", &CPU6502::ADC, &CPU6502::mod_absx, 4, PageCrossed)},			{0x18, Instruction("CLC_IMP", &CPU6502::CLC, &CPU6502::mod_imp, 2)},							{0x5D, Instruction("EOR_ABSX", &CPU6502::EOR, &CPU6502::mod_absx, 4, PageCrossed)},				{0xBE, Instruction("LDX_ABSY", &CPU6502::LDX, &CPU6502::mod_absy, 4, PageCrossed)},				{0x3E, Instruction("ROL_ABSX", &CPU6502::ROL, &CPU6502::mod_absx, 7)},							{0x84, Instruction("STY_ZP", &CPU6502::STY, &CPU6502::mod_zp, 3)},								{0x33, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 8)},						{0x77, Instruction("UNK_ZPX", &CPU6502::UNK, &CPU6502::mod_zpx, 6)},							{0xC7, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 5)},
		{0x79, Instruction("ADC_ABSY", &CPU6502::ADC, &CPU6502::mod_absy, 4, PageCrossed)},																											{0x59, Instruction("EOR_ABSY", &CPU6502::EOR, &CPU6502::mod_absy, 4, PageCrossed)},																																																				{0x94, Instruction("STY_ZPX", &CPU6502::STY, &CPU6502::mod_zpx, 4)},							{0x34, Instruction("NOP_ZPX", &CPU6502::NOP, &CPU6502::mod_zpx, 4)},						{0x7A, Instruction("NOP_???", &CPU6502::NOP, &CPU6502::mod_imp, 2)},							{0xCB, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},
		{0x61, Instruction("ADC_IDRX", &CPU6502::ADC, &CPU6502::mod_idrx, 6)},																														{0x41, Instruction("EOR_IDRX", &CPU6502::EOR, &CPU6502::mod_idrx, 6)},							{0xA0, Instruction("LDY_IMD", &CPU6502::LDY, &CPU6502::mod_imd, 2)},							{0x6A, Instruction("ROR_ACC", &CPU6502::ROR, &CPU6502::mod_acc, 2)},							{0x8C, Instruction("STY_ABS", &CPU6502::STY, &CPU6502::mod_abs, 4)},							{0x37, Instruction("UNK_ZPX", &CPU6502::UNK, &CPU6502::mod_zpx, 6)},						{0x7B, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 7)},							{0xCF, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 6)},
		{0x71, Instruction("ADC_IDRY", &CPU6502::ADC, &CPU6502::mod_idry, 5, PageCrossed)},																											{0x51, Instruction("EOR_IDRY", &CPU6502::EOR, &CPU6502::mod_idry, 5, PageCrossed)},				{0xA4, Instruction("LDY_ZP", &CPU6502::LDY, &CPU6502::mod_zp, 3)},								{0x66, Instruction("ROR_ZP", &CPU6502::ROR, &CPU6502::mod_zp, 5)},
																																																																									{0xB4, Instruction("LDY_ZPX", &CPU6502::LDY, &CPU6502::mod_zpx, 4)},							{0x76, Instruction("ROR_ZPX", &CPU6502::ROR, &CPU6502::mod_zpx, 6)},							{0xAA, Instruction("TAX_IMP", &CPU6502::TAX, &CPU6502::mod_imp, 2)},							{0x3A, Instruction("NOP_???", &CPU6502::NOP, &CPU6502::mod_imp, 2)},						{0x7C, Instruction("NOP_ABSX", &CPU6502::NOP, &CPU6502::mod_absx, 4, PageCrossed)},				{0xD2, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},
		{0x29, Instruction("AND_IMD", &CPU6502::AND, &CPU6502::mod_imd, 2)},						{0xC9, Instruction("CMP_IMD", &CPU6502::CMP, &CPU6502::mod_imd, 2)},							{0xE6, Instruction("INC_ZP", &CPU6502::INC, &CPU6502::mod_zp, 5)},								{0xAC, Instruction("LDY_ABS", &CPU6502::LDY, &CPU6502::mod_abs, 4)},							{0x6E, Instruction("ROR_ABS", &CPU6502::ROR, &CPU6502::mod_abs, 6)},							{0xA8, Instruction("TAY_IMP", &CPU6502::TAY, &CPU6502::mod_imp, 2)},							{0x3B, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 7)},						{0x7F, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 7)},							{0xD3, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 8)},
		{0x25, Instruction("AND_ZP", &CPU6502::AND, &CPU6502::mod_zp, 3)},							{0xC5, Instruction("CMP_ZP", &CPU6502::CMP, &CPU6502::mod_zp, 3)},								{0xF6, Instruction("INC_ZPX", &CPU6502::INC, &CPU6502::mod_zpx, 6)},							{0xBC, Instruction("LDY_ABSX", &CPU6502::LDY, &CPU6502::mod_absx, 4, PageCrossed)},				{0x7E, Instruction("ROR_ABSX", &CPU6502::ROR, &CPU6502::mod_absx, 7)},							{0xBA, Instruction("TSX_IMP", &CPU6502::TSX, &CPU6502::mod_imp, 2)},							{0x3C, Instruction("NOP_ABSX", &CPU6502::NOP, &CPU6502::mod_absx, 4, PageCrossed)},			{0x80, Instruction("NOP_IMD", &CPU6502::NOP, &CPU6502::mod_imd, 2)},							{0xD4, Instruction("NOP_ZPX", &CPU6502::NOP, &CPU6502::mod_zpx, 4)},
		{0x35, Instruction("AND_ZPX", &CPU6502::AND, &CPU6502::mod_zpx, 4)},						{0xD5, Instruction("CMP_ZPX", &CPU6502::CMP, &CPU6502::mod_zpx, 4)},							{0xEE, Instruction("INC_ABS", &CPU6502::INC, &CPU6502::mod_abs, 6)},																																																							{0x8A, Instruction("TXA_IMP", &CPU6502::TXA, &CPU6502::mod_imp, 2)},							{0x3F, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 7)},						{0x82, Instruction("NOP_IMD", &CPU6502::NOP, &CPU6502::mod_imd, 2)},							{0xD7, Instruction("UNK_ZPX", &CPU6502::UNK, &CPU6502::mod_zpx, 6)},
		{0x2D, Instruction("AND_ABS", &CPU6502::AND, &CPU6502::mod_abs, 4)},						{0xCD, Instruction("CMP_ABS", &CPU6502::CMP, &CPU6502::mod_abs, 4)},							{0xFE, Instruction("INC_ABSX", &CPU6502::INC, &CPU6502::mod_absx, 7)},							{0x4A, Instruction("LSR_ACC", &CPU6502::LSR, &CPU6502::mod_acc, 2)},							{0x40, Instruction("RTI_IMP", &CPU6502::RTI, &CPU6502::mod_imp, 6)},							{0x9A, Instruction("TXS_IMP", &CPU6502::TXS, &CPU6502::mod_imp, 2)},							{0x42, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},						{0x83, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 6)},							{0xDA, Instruction("NOP_???", &CPU6502::NOP, &CPU6502::mod_imp, 2)},
		{0x3D, Instruction("AND_ABSX", &CPU6502::AND, &CPU6502::mod_absx, 4, PageCrossed)},			{0xDD, Instruction("CMP_ABSX", &CPU6502::CMP, &CPU6502::mod_absx, 4, PageCrossed)},																												{0x46, Instruction("LSR_ZP", &CPU6502::LSR, &CPU6502::mod_zp, 5)},								{0x60, Instruction("RTS_IMP", &CPU6502::RTS, &CPU6502::mod_imp, 6)},							{0x98, Instruction("TYA_IMP", &CPU6502::TYA, &CPU6502::mod_imp, 2)},							{0x43, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 8)},						{0x87, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 3)},								{0xDB, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 7)},
		{0x39, Instruction("AND_ABSY", &CPU6502::AND, &CPU6502::mod_absy, 4, PageCrossed)},			{0xD9, Instruction("CMP_ABSY", &CPU6502::CMP, &CPU6502::mod_absy, 4, PageCrossed)},				{0xE8, Instruction("INX_IMP", &CPU6502::INX, &CPU6502::mod_imp, 2)},							{0x56, Instruction("LSR_ZPX", &CPU6502::LSR, &CPU6502::mod_zpx, 6)},
		{0x21, Instruction("AND_IDRX", &CPU6502::AND, &CPU6502::mod_idrx, 6)},						{0xC1, Instruction("CMP_IDRX", &CPU6502::CMP, &CPU6502::mod_idrx, 6)},							{0xC8, Instruction("INY_IMP", &CPU6502::INY, &CPU6502::mod_imp, 2)},							{0x4E, Instruction("LSR_ABS", &CPU6502::LSR, &CPU6502::mod_abs, 6)},							{0xE9, Instruction("SBC_IMD", &CPU6502::SBC, &CPU6502::mod_imd, 2)},							{0x02, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},							{0x44, Instruction("NOP_ZP", &CPU6502::NOP, &CPU6502::mod_zp, 3)},							{0x89, Instruction("NOP_IMD", &CPU6502::NOP, &CPU6502::mod_imd, 2)},							{0xDC, Instruction("NOP_ABSX", &CPU6502::NOP, &CPU6502::mod_absx, 4, PageCrossed)},
		{0x31, Instruction("AND_IDRY", &CPU6502::AND, &CPU6502::mod_idry, 5, PageCrossed)},			{0xD1, Instruction("CMP_IDRY", &CPU6502::CMP, &CPU6502::mod_idry, 5, PageCrossed)},																												{0x5E, Instruction("LSR_ABSX", &CPU6502::LSR, &CPU6502::mod_absx, 7)},							{0xE5, Instruction("SBC_ZP", &CPU6502::SBC, &CPU6502::mod_zp, 3)},								{0x03, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 8)},							{0x47, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 5)},							{0x8B, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},							{0xDF, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 7)},
																																																																																																	{0xF5, Instruction("SBC_ZPX", &CPU6502::SBC, &CPU6502::mod_zpx, 4)},							{0x04, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 3)},								{0x4B, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},						{0x8F, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 4)},							{0xE2, Instruction("NOP_IMD", &CPU6502::NOP, &CPU6502::mod_imd, 2)},
		{0x0A, Instruction("ASL_ACC", &CPU6502::ASL, &CPU6502::mod_acc, 2)},						{0xE0, Instruction("CPX_IMD", &CPU6502::CPX, &CPU6502::mod_imd, 2)},							{0x4C, Instruction("JMP_ABS", &CPU6502::JMP, &CPU6502::mod_abs, 3)},							{0xEA, Instruction("NOP_IMP", &CPU6502::NOP, &CPU6502::mod_imp, 2)},							{0xED, Instruction("SBC_ABS", &CPU6502::SBC, &CPU6502::mod_abs, 4)},							{0x07, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 5)},								{0x4F, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 6)},						{0x92, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},							{0xE3, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 8)},						
		{0x06, Instruction("ASL_ZP", &CPU6502::ASL, &CPU6502::mod_zp, 5)},							{0xE4, Instruction("CPX_ZP", &CPU6502::CPX, &CPU6502::mod_zp, 3)},								{0x6C, Instruction("JMP_IDR", &CPU6502::JMP, &CPU6502::mod_idr, 5)},																															{0xFD, Instruction("SBC_ABSX", &CPU6502::SBC, &CPU6502::mod_absx, 4, PageCrossed)},				{0x0B, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},							{0x52, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},						{0x93, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 6)},							{0xE7, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 5)},
		{0x16, Instruction("ASL_ZPX", &CPU6502::ASL, &CPU6502::mod_zpx, 6)},						{0xEC, Instruction("CPX_ABS", &CPU6502::CPX, &CPU6502::mod_abs, 4)},							{0x20, Instruction("JSR_ABS", &CPU6502::JSR, &CPU6502::mod_abs, 6)},							{0x09, Instruction("ORA_IMD", &CPU6502::ORA, &CPU6502::mod_imd, 2)},							{0xF9, Instruction("SBC_ABSY", &CPU6502::SBC, &CPU6502::mod_absy, 4, PageCrossed)},				{0x0C, Instruction("NOP_ABS", &CPU6502::NOP, &CPU6502::mod_abs, 4)},							{0x53, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 8)},						{0x97, Instruction("UNK_ZPY", &CPU6502::UNK, &CPU6502::mod_zpy, 4)},							{0xEB, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},
		{0x0E, Instruction("ASL_ABS", &CPU6502::ASL, &CPU6502::mod_abs, 6)},																																																						{0x05, Instruction("ORA_ZP", &CPU6502::ORA, &CPU6502::mod_zp, 3)},								{0xE1, Instruction("SBC_IDRX", &CPU6502::SBC, &CPU6502::mod_idrx, 6)},							{0x0F, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 6)},							{0x54, Instruction("NOP_ZPX", &CPU6502::NOP, &CPU6502::mod_zpx, 6)},						{0x9B, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 5)},							{0xEF, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 6)},
		{0x1E, Instruction("ASL_ABSX", &CPU6502::ASL, &CPU6502::mod_absx, 7)},																																																						{0x15, Instruction("ORA_ZPX", &CPU6502::ORA, &CPU6502::mod_zpx, 4)},							{0xF1, Instruction("SBC_IDRY", &CPU6502::SBC, &CPU6502::mod_idry, 5, PageCrossed)},				{0x12, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},							{0x57, Instruction("UNK_ZPX", &CPU6502::UNK, &CPU6502::mod_zpx, 6)},						{0x9C, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 5)},							{0xF2, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},
																																																																									{0x0D, Instruction("ORA_ABS", &CPU6502::ORA, &CPU6502::mod_abs, 4)},
		{0x90, Instruction("BCC_REL", &CPU6502::BCC, &CPU6502::mod_rel, 2, NewPage)},				{0xC0, Instruction("CPY_IMD", &CPU6502::CPY, &CPU6502::mod_imd, 2)},							{0xA9, Instruction("LDA_IMD", &CPU6502::LDA, &CPU6502::mod_imd, 2)},							{0x1D, Instruction("ORA_ABSX", &CPU6502::ORA, &CPU6502::mod_absx, 4, PageCrossed)},				{0x38, Instruction("SEC_IMP", &CPU6502::SEC, &CPU6502::mod_imp, 2)},							{0x13, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 8)},							{0x5A, Instruction("NOP_???", &CPU6502::NOP, &CPU6502::mod_imp, 2)},						{0x9E, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 5)},							{0xF3, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 8)},
		{0xB0, Instruction("BCS_REL", &CPU6502::BCS, &CPU6502::mod_rel, 2, NewPage)},				{0xC4, Instruction("CPY_ZP", &CPU6502::CPY, &CPU6502::mod_zp, 3)},								{0xA5, Instruction("LDA_ZP", &CPU6502::LDA, &CPU6502::mod_zp, 3)},								{0x19, Instruction("ORA_ABSY", &CPU6502::ORA, &CPU6502::mod_absy, 4, PageCrossed)},				{0xF8, Instruction("SED_IMP", &CPU6502::SED, &CPU6502::mod_imp, 2)},							{0x14, Instruction("NOP_ZPX", &CPU6502::NOP, &CPU6502::mod_zpx, 4)},							{0x5B, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 7)},						{0x9F, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 5)},							{0xF4, Instruction("NOP_ZPX", &CPU6502::NOP, &CPU6502::mod_zpx, 4)},
		{0xF0, Instruction("BEQ_REL", &CPU6502::BEQ, &CPU6502::mod_rel, 2, NewPage)},				{0xCC, Instruction("CPY_ABS", &CPU6502::CPY, &CPU6502::mod_abs, 4)},							{0xB5, Instruction("LDA_ZPX", &CPU6502::LDA, &CPU6502::mod_zpx, 4)},							{0x01, Instruction("ORA_IDRX", &CPU6502::ORA, &CPU6502::mod_idrx, 6)},							{0x78, Instruction("SEI_IMP", &CPU6502::SEI, &CPU6502::mod_imp, 2)},							{0x17, Instruction("UNK_ZPX", &CPU6502::UNK, &CPU6502::mod_zpx, 6)},							{0x5C, Instruction("NOP_ABSX", &CPU6502::NOP, &CPU6502::mod_absx, 4, PageCrossed)},			{0xA3, Instruction("LAX_IDRX", &CPU6502::LAX, &CPU6502::mod_idrx, 6)},							{0xF7, Instruction("UNK_ZPX", &CPU6502::UNK, &CPU6502::mod_zpx, 6)},
		{0x30, Instruction("BMI_REL", &CPU6502::BMI, &CPU6502::mod_rel, 2, NewPage)},																												{0xAD, Instruction("LDA_ABS", &CPU6502::LDA, &CPU6502::mod_abs, 4)},							{0x11, Instruction("ORA_IDRY", &CPU6502::ORA, &CPU6502::mod_idry, 5, PageCrossed)},
		{0xD0, Instruction("BNE_REL", &CPU6502::BNE, &CPU6502::mod_rel, 2, NewPage)},				{0xC6, Instruction("DEC_ZP", &CPU6502::DEC, &CPU6502::mod_zp, 5)},								{0xBD, Instruction("LDA_ABSX", &CPU6502::LDA, &CPU6502::mod_absx, 4, PageCrossed)},																												{0x85, Instruction("STA_ZP", &CPU6502::STA, &CPU6502::mod_zp, 3)},								{0x1A, Instruction("NOP_???", &CPU6502::NOP, &CPU6502::mod_imp, 2)},							{0x5F, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 7)},						{0xA7, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 3)},								{0xFA, Instruction("NOP_???", &CPU6502::NOP, &CPU6502::mod_imp, 2)},
		{0x10, Instruction("BPL_REL", &CPU6502::BPL, &CPU6502::mod_rel, 2, NewPage)},				{0xD6, Instruction("DEC_ZPX", &CPU6502::DEC, &CPU6502::mod_zpx, 6)},							{0xB9, Instruction("LDA_ABSY", &CPU6502::LDA, &CPU6502::mod_absy, 4, PageCrossed)},				{0x48, Instruction("PHA_IMP", &CPU6502::PHA, &CPU6502::mod_imp, 3)},							{0x95, Instruction("STA_ZPX", &CPU6502::STA, &CPU6502::mod_zpx, 4)},							{0x1B, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 7)},							{0x62, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},						{0xAB, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},							{0xFB, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 7)},
		{0x50, Instruction("BVC_REL", &CPU6502::BVC, &CPU6502::mod_rel, 2, NewPage)},				{0xCE, Instruction("DEC_ABS", &CPU6502::DEC, &CPU6502::mod_abs, 6)},							{0xA1, Instruction("LDA_IDRX", &CPU6502::LDA, &CPU6502::mod_idrx, 6)},							{0x08, Instruction("PHP_IMP", &CPU6502::PHP, &CPU6502::mod_imp, 3)},							{0x8D, Instruction("STA_ABS", &CPU6502::STA, &CPU6502::mod_abs, 4)},							{0x1C, Instruction("NOP_ABSX", &CPU6502::NOP, &CPU6502::mod_absx, 4, PageCrossed)},				{0x63, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 8)},						{0xAF, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 4)},							{0xFC, Instruction("NOP_ABSX", &CPU6502::NOP, &CPU6502::mod_absx, 4, PageCrossed)},
		{0x70, Instruction("BVS_REL", &CPU6502::BVS, &CPU6502::mod_rel, 2, NewPage)},				{0xDE, Instruction("DEC_ABSX", &CPU6502::DEC, &CPU6502::mod_absx, 7)},							{0xB1, Instruction("LDA_IDRY", &CPU6502::LDA, &CPU6502::mod_idry, 5, PageCrossed)},				{0x68, Instruction("PLA_IMP", &CPU6502::PLA, &CPU6502::mod_imp, 4)},							{0x9D, Instruction("STA_ABSX", &CPU6502::STA, &CPU6502::mod_absx, 5)},							{0x1F, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 7)},							{0x64, Instruction("NOP_ZP", &CPU6502::NOP, &CPU6502::mod_zp, 3)},							{0xB2, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},							{0xFF, Instruction("UNK_ABSX", &CPU6502::UNK, &CPU6502::mod_absx, 7)},
																																																																									{0x28, Instruction("PLP_IMP", &CPU6502::PLP, &CPU6502::mod_imp, 4)},							{0x99, Instruction("STA_ABSY", &CPU6502::STA, &CPU6502::mod_absy, 5)},							{0x22, Instruction("UNK_???", &CPU6502::UNK, &CPU6502::mod_imp, 2)},							{0x67, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 5)},							{0xB3, Instruction("UNK_IDRY", &CPU6502::UNK, &CPU6502::mod_idry, 5, PageCrossed)},
		{0x24, Instruction("BIT_ZP", &CPU6502::BIT, &CPU6502::mod_zp, 3)},							{0xCA, Instruction("DEX_IMP", &CPU6502::DEX, &CPU6502::mod_imp, 2)},																																																							{0x81, Instruction("STA_IDRX", &CPU6502::STA, &CPU6502::mod_idrx, 6)},							{0x23, Instruction("UNK_IDRX", &CPU6502::UNK, &CPU6502::mod_idrx, 8)},							{0x6B, Instruction("UNK_IMD", &CPU6502::UNK, &CPU6502::mod_imd, 2)},						{0xB7, Instruction("UNK_ZPY", &CPU6502::UNK, &CPU6502::mod_zpy, 4)},
		{0x2C, Instruction("BIT_ABS", &CPU6502::BIT, &CPU6502::mod_abs, 4)},						{0x88, Instruction("DEY_IMP", &CPU6502::DEY, &CPU6502::mod_imp, 2)},																																																							{0x91, Instruction("STA_IDRY", &CPU6502::STA, &CPU6502::mod_idry, 6)},							{0x27, Instruction("UNK_ZP", &CPU6502::UNK, &CPU6502::mod_zp, 5)},								{0x6F, Instruction("UNK_ABS", &CPU6502::UNK, &CPU6502::mod_abs, 6)},						{0xBB, Instruction("UNK_ABSY", &CPU6502::UNK, &CPU6502::mod_absy, 4, PageCrossed)},
		{0x00, Instruction("BRK_IMP", &CPU6502::BRK, &CPU6502::mod_imp, 7)},
	};

public:

	void connectBus(CpuBus* cpu_bus);
	void powerUp();
	void cycle();
	void clearMemory();
	void write(uint16_t address, uint8_t data);
	uint8_t read(uint16_t address);
	
	bool extractFlag(Flags flag);
	void setFlag(Flags flag, bool flag_value);
	void stackPush(uint8_t data);
	uint8_t stackPull();

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

	void STA(); // Store accumulator in memory address

	void STX(); // Store X register in memory address

	void STY(); // Store Y register

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

	void UNK(); // Unknown - same as NOP, but executed by instructions with illegal opcodes

	void LAX();

	// Addressing modes

	void mod_abs(); // Absolute - use 2 bytes operand as address e.g. JMP $a000

	void mod_acc(); // Accumulator - operation performed on accumulator

	void mod_zp();  // Zero page - use 1 byte operand as address e.g. STA $01

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

	void mod_imp(); // Implicit - instuctions like for example INX don't take any operand,
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