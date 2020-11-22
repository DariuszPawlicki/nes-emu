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

	    //fetching
	    //pc+=2 or +=3
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
	bool carry = this->extract_flag(Carry);

	this->flags &= ~0x1;

	if (this->acc_memory_switch == true) {

		this->flags |= (this->acc & 0x80) >> 7;
		this->acc <<= 1;
		this->acc += carry;

		if (this->acc == 0)
			this->set_flag(Zero);

		if (this->acc >= 128)
			this->set_flag(Negative);
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		this->flags |= (data & 0x80) >> 7;

		data <<= 1;
		data += carry;

		if (data == 0)
			this->set_flag(Zero);

		if (data >= 128)
			this->set_flag(Negative);
	}
}

void CPU6502::ROR()
{
	bool carry = this->extract_flag(Carry);

	this->flags &= ~0x1;

	if (this->acc_memory_switch == true) {

		this->flags |= (this->acc & 0x1);
		this->acc >>= 1;
		this->acc |= carry << 7;

		if (this->acc == 0)
			this->set_flag(Zero);

		if (this->acc >= 128)
			this->set_flag(Negative);
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		this->flags |= (data & 0x1);
		data >>= 1;
		data |= carry << 7;

		if (data == 0)
			this->set_flag(Zero);

		if (data >= 128)
			this->set_flag(Negative);
	}
}

void CPU6502::RTI()
{
	this->flags = this->stack_pull();
	this->pc = this->stack_pull();
}

void CPU6502::RTS()
{
	this->pc = this->stack_pull();
}

void CPU6502::SBC()
{
	this->acc -= this->bus.read(this->target_address) - ~(this->extract_flag(Carry));
}

void CPU6502::SEC()
{
	this->set_flag(Carry);
}

void CPU6502::SED()
{
	this->set_flag(Decimal);
}

void CPU6502::SEI()
{
	this->set_flag(Interrupt);
}

void CPU6502::STA()
{
	this->bus.write(this->target_address, this->acc);
}

void CPU6502::STX()
{
	this->bus.write(this->target_address, this->x);
}

void CPU6502::STY()
{
	this->bus.write(this->target_address, this->y);
}

void CPU6502::TAX()
{
	this->x = this->acc;

	if (this->x == 0)
		this->set_flag(Zero);

	if (this->x >= 128)
		this->set_flag(Negative);
}

void CPU6502::TAY()
{
	this->y = this->acc;

	if (this->y == 0)
		this->set_flag(Zero);

	if (this->y >= 128)
		this->set_flag(Negative);
}

void CPU6502::TSX()
{
	this->x = this->sp;

	if (this->x == 0)
		this->set_flag(Zero);

	if (this->x >= 128)
		this->set_flag(Negative);
}

void CPU6502::TXA()
{
	this->acc = this->x;

	if (this->acc == 0)
		this->set_flag(Zero);

	if (this->acc >= 128)
		this->set_flag(Negative);
}

void CPU6502::TXS()
{
	this->sp = this->x;

}

void CPU6502::TYA()
{
	this->acc = this->y;

	if (this->y == 0)
		this->set_flag(Zero);

	if (this->y >= 128)
		this->set_flag(Negative);
}

void CPU6502::mod_abs()
{
	this->target_address = this->instr_operand;
}

void CPU6502::mod_acc()
{
	this->acc_memory_switch = true;
}

void CPU6502::mod_zp()
{
	this->target_address = (this->instr_operand & 0x00FF);
}

void CPU6502::mod_zpx()
{
	this->target_address = (this->instr_operand & 0x00FF) + this->x;
}

void CPU6502::mod_zpy()
{
	this->target_address = (this->instr_operand & 0x00FF) + this->y;
}

void CPU6502::mod_absx()
{
	this->target_address = this->instr_operand + this->x;
}

void CPU6502::mod_absy()
{
	this->target_address = this->instr_operand + this->y;
}


void CPU6502::mod_imd() 
{
	return;
}

void CPU6502::mod_rel()
{
	this->pc += (this->instr_operand & 0x00FF);
}

void CPU6502::mod_imp()
{
	return;
}

void CPU6502::mod_idr()
{
	uint8_t least_significant_bit = this->bus.read(this->instr_operand);
	uint8_t most_significant_bit = this->bus.read(this->instr_operand + 1);

	this->target_address = (uint16_t)most_significant_bit + (uint16_t)least_significant_bit;
}

void CPU6502::mod_iidrx()
{
	uint8_t lsb_loc = (this->instr_operand & 0x00FF) + this->x;
	uint8_t msb_loc = (this->instr_operand & 0x00FF) + (this->x + 1);

	uint8_t least_significant_bit = this->bus.read(lsb_loc);
	uint8_t most_significant_bit = this->bus.read(msb_loc);

	this->target_address = (uint16_t)most_significant_bit + (uint16_t)least_significant_bit;
}

void CPU6502::mod_idriy()
{
	uint8_t lsb_loc = (this->instr_operand & 0x00FF);
	uint8_t msb_loc = (this->instr_operand & 0x00FF) + 1;

	uint8_t least_significant_bit = this->bus.read(lsb_loc);
	uint8_t most_significant_bit = this->bus.read(msb_loc);

	this->target_address = (uint16_t)most_significant_bit + (uint16_t)least_significant_bit + this->y;
}