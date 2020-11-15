#include "CPU6502.hpp"
#include <iostream>




void CPU6502::load_rom(std::string file_path) {

	std::ifstream rom;

	rom.open(file_path, std::ios::binary);

	if (rom.is_open()) {

		rom.seekg(0, rom.end);

		int rom_size = rom.tellg();

		rom.seekg(0, rom.beg);

		char* buffer = new char[rom_size];

		rom.read(buffer, rom_size);		

		for (int i = 0; i < rom_size; i++) {

			this->bus.write(ROM_MEMORY_BEGINNING + i, buffer[i]);
		}

		delete[] buffer;
	}	
}


void CPU6502::fetch() {


}


void CPU6502::decode() {

	if ((this->fetched_instr & 0x00FF0000) == 0) {
		this->instr_opcode = (this->fetched_instr & 0x0000FF00) >> 8;
		this->instr_operand = this->fetched_instr & 0x000000FF;
	}
	else {
		this->instr_opcode = (this->fetched_instr & 0x00FF0000) >> 16;
		this->instr_operand = this->fetched_instr & 0x0000FFFF;
	}
}


void CPU6502::cpu_cycle() { 
	
	Instruction cur_instruction = this->op_map[this->instr_opcode];

	(this->*(cur_instruction.adr_mod))();	
}


bool CPU6502::extract_flag(Flags flag) {

	switch (flag) {

		case Carry:
			return (this->flags & 0x1);
			break;

		case Zero:
			return (this->flags & 0x2) >> 1;
			break;

		case Interrupt:
			return (this->flags & 0x4) >> 2;
			break;

		case Decimal:
			return (this->flags & 0x8) >> 3;
			break;

		case Break:
			return (this->flags & 0x10) >> 4;
			break;

		case Overflow:
			return (this->flags & 0x20) >> 5;
			break;

		case Negative:
			return (this->flags & 0x40) >> 6;
			break;
	}
}


void CPU6502::set_flag(Flags flag) {

	switch (flag) {

		case Carry:
			this->flags |= 0x1;
			break;

		case Zero:
			this->flags |= 0x2;
			break;

		case Interrupt:
			this->flags |= 0x4;
			break;

		case Decimal:
			this->flags |= 0x8;
			break;

		case Break:
			this->flags |= 0x10;
			break;

		case Overflow:
			this->flags |= 0x20;
			break;

		case Negative:
			this->flags |= 0x40;
			break;
	}
}


void CPU6502::stack_push(uint8_t data) {

	this->bus.write(STACK_BEGINNING + this->sp, data);
	this->sp++;
}


uint8_t CPU6502::stack_pull() {

	uint8_t data = this->bus.read(STACK_BEGINNING + this->sp);

	this->sp--;

	return data;
}


void CPU6502::ADC() {

	uint8_t memory_data = this->bus.read(this->target_address);

	if (this->acc + memory_data + this->extract_flag(Carry) > 255) {
		this->set_flag(Carry);
	}

	this->acc += memory_data + this->extract_flag(Carry);

	if (this->acc == 0)
		this->set_flag(Zero);
	
	if (this->acc >= 128)
		this->set_flag(Negative);
}


void CPU6502::AND() {

	this->acc &= this->bus.read(this->target_address);

	if (this->acc == 0)
		this->set_flag(Zero);

	if (this->acc >= 128)
		this->set_flag(Negative);
}


void CPU6502::ASL() {

	if (this->acc_memory_switch == true) {

		if (this->acc >= 128)
			this->set_flag(Carry);

		this->acc <<= 1;

		if (this->acc == 0)
			this->set_flag(Zero);

		if (this->acc >= 128)
			this->set_flag(Negative);
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		if (data >= 128)
			this->set_flag(Carry);

		data <<= 1;

		this->bus.write(this->target_address, data);

		if (data == 0)
			this->set_flag(Zero);

		if (data >= 128)
			this->set_flag(Negative);
	}	
}

void CPU6502::BCC()
{
	if (this->extract_flag(Carry) == false) {
		this->pc += this->instr_operand;
	}
}

void CPU6502::BCS()
{
	if (this->extract_flag(Carry) == true) {
		this->pc += this->instr_operand;
	}
}

void CPU6502::BEQ()
{
	if (this->extract_flag(Zero) == true) {
		this->pc += this->instr_operand;
	}
}

void CPU6502::BIT()
{
	uint8_t data = this->bus.read(this->target_address);
	
	data &= this->acc;

	if (data == 0)
		this->set_flag(Zero);
	
	if ((data & 0x40) >> 6 == 1)
		this->set_flag(Overflow);

	if ((data & 0x80) >> 7 == 1)
		this->set_flag(Negative);
}

void CPU6502::BMI()
{
	if (this->extract_flag(Negative) == true)
		this->pc += this->instr_operand;
}

void CPU6502::BNE()
{
	if (this->extract_flag(Zero) == false)
		this->pc += this->instr_operand;
}

void CPU6502::BPL()
{
	if (this->extract_flag(Negative) == false)
		this->pc += this->instr_operand;
}

void CPU6502::BRK()
{
	this->stack_push(this->pc);

	this->stack_push(this->flags);

	this->set_flag(Break);
}

void CPU6502::BVC()
{
	if (this->extract_flag(Overflow) == false)
		this->pc += this->instr_operand;
}

void CPU6502::BVS()
{
	if (this->extract_flag(Overflow) == true)
		this->pc += this->instr_operand;
}

void CPU6502::CLC()
{
	this->flags ^= 0x1;
}

void CPU6502::CLD()
{
	this->flags ^= 0x8;
}

void CPU6502::CLI()
{
	this->flags ^= 0x4;
}

void CPU6502::CLV()
{
	this->flags ^= 0x20;
}

void CPU6502::CMP()
{
	uint8_t data = this->bus.read(this->target_address);

	if (this->acc >= data)
		this->set_flag(Carry);

	if (this->acc == data)
		this->set_flag(Zero);

	if (this->acc - data >= 128)
		this->set_flag(Negative);
}

void CPU6502::CPX()
{
	uint8_t data = this->bus.read(this->target_address);

	if (this->x >= data)
		this->set_flag(Carry);

	if (this->x == data)
		this->set_flag(Zero);

	if (this->x - data >= 128)
		this->set_flag(Negative);
}

void CPU6502::CPY()
{
	uint8_t data = this->bus.read(this->target_address);

	if (this->y >= data)
		this->set_flag(Carry);

	if (this->y == data)
		this->set_flag(Zero);

	if (this->y - data >= 128)
		this->set_flag(Negative);
}

void CPU6502::DEC()
{
	uint8_t data = this->bus.read(this->target_address);
	
	data--;

	this->bus.write(this->target_address, data);

	if (data == 0)
		this->set_flag(Zero);

	if (data >= 128)
		this->set_flag(Negative);
}

void CPU6502::DEX()
{
	this->x--;

	if (this->x == 0)
		this->set_flag(Zero);

	if (this->x >= 128)
		this->set_flag(Negative);
}

void CPU6502::DEY()
{
	this->y--;

	if (this->y == 0)
		this->set_flag(Zero);

	if (this->y >= 128)
		this->set_flag(Negative);
}

void CPU6502::EOR()
{
	this->acc ^= this->bus.read(this->target_address);

	if (this->acc == 0)
		this->set_flag(Zero);

	if (this->acc >= 128)
		this->set_flag(Negative);
}

void CPU6502::INC()
{
	uint8_t data = this->bus.read(this->target_address);

	data++;

	this->bus.write(this->target_address, data);

	if (data == 0)
		this->set_flag(Zero);

	if (data >= 128)
		this->set_flag(Negative);
}

void CPU6502::INX()
{
	this->x++;

	if (this->x == 0)
		this->set_flag(Zero);

	if (this->x >= 128)
		this->set_flag(Negative);
}

void CPU6502::INY()
{
	this->y++;

	if (this->y == 0)
		this->set_flag(Zero);

	if (this->y >= 128)
		this->set_flag(Negative);
}

void CPU6502::JMP()
{
	this->pc = this->target_address;
}

void CPU6502::JSR()
{
	this->stack_push(this->pc + 2);
	this->pc = this->target_address;
}

void CPU6502::LDA()
{
	this->acc = this->bus.read(this->target_address);

	if (this->acc == 0)
		this->set_flag(Zero);

	if (this->acc >= 128)
		this->set_flag(Negative);
}

void CPU6502::LDX()
{
	this->x = this->bus.read(this->target_address);

	if (this->x == 0)
		this->set_flag(Zero);

	if (this->x >= 128)
		this->set_flag(Negative);
}

void CPU6502::LDY()
{
	this->y = this->bus.read(this->target_address);

	if (this->y == 0)
		this->set_flag(Zero);

	if (this->y >= 128)
		this->set_flag(Negative);
}

void CPU6502::LSR()
{
	if (this->acc_memory_switch == true) {

		if (this->acc % 2 == 0)
			this->set_flag(Carry);

		this->acc >>= 1;

		if (this->acc == 0)
			this->set_flag(Zero);

		if (this->acc >= 128)
			this->set_flag(Negative);
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		if (data % 2 == 0)
			this->set_flag(Carry);

		data >>= 1;

		this->bus.write(this->target_address, data);

		if (data == 0)
			this->set_flag(Zero);

		if (data >= 128)
			this->set_flag(Negative);
	}
}

void CPU6502::NOP()
{
	return;
}

void CPU6502::ORA()
{
	this->acc |= this->bus.read(this->target_address);

	if (this->acc == 0)
		this->set_flag(Zero);

	if (this->acc >= 128)
		this->set_flag(Negative);
}

void CPU6502::PHA()
{
	this->stack_push(this->acc);
}

void CPU6502::PHP()
{
	this->stack_push(this->flags);
}

void CPU6502::PLA()
{
	this->acc = this->stack_pull();

	if (this->acc == 0)
		this->set_flag(Zero);

	if (this->acc >= 128)
		this->set_flag(Negative);
}

void CPU6502::PLP()
{
	this->flags = this->stack_pull();
}

void CPU6502::ROL()
{

}

void CPU6502::ROR()
{
}

void CPU6502::RTI()
{
}

void CPU6502::RTS()
{
}

void CPU6502::SBC()
{
}

void CPU6502::SEC()
{
}

void CPU6502::SED()
{
}

void CPU6502::SEI()
{
}

void CPU6502::STA()
{
}

void CPU6502::STX()
{
}

void CPU6502::STY()
{
}

void CPU6502::TAX()
{
}

void CPU6502::TAY()
{
}

void CPU6502::TSX()
{
}

void CPU6502::TXA()
{
}

void CPU6502::TXS()
{
}

void CPU6502::TYA()
{
}

void CPU6502::mod_abs()
{
}

void CPU6502::mod_acc()
{
	this->acc_memory_switch = true;
}

void CPU6502::mod_zp()
{
}

void CPU6502::mod_zpx()
{
}

void CPU6502::mod_zpy()
{
}

void CPU6502::mod_absx()
{
}

void CPU6502::mod_absy()
{
}


void CPU6502::mod_imd() {

	std::cout << "IMD";
}

void CPU6502::mod_rel()
{
}

void CPU6502::mod_imp()
{
}

void CPU6502::mod_idr()
{
}

void CPU6502::mod_iidrx()
{
}

void CPU6502::mod_idriy()
{
}

