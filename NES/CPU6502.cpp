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
			return (this->flags & 0x40) >> 6;
			break;

		case Negative:
			return (this->flags & 0x80) >> 7;
			break;
	}
}


void CPU6502::set_flag(Flags flag, bool flag_value) {

	switch (flag) {

		case Carry:
			if (flag_value)
			{
				this->flags |= 0x1;
			}
			else 
			{
				this->flags &= ~0x1;
			}
			break;

		case Zero:
			if (flag_value)
			{
				this->flags |= 0x2;
			}
			else
			{
				this->flags &= ~0x2;
			}
			break;

		case Interrupt:
			if (flag_value)
			{
				this->flags |= 0x4;
			}
			else
			{
				this->flags &= ~0x4;
			}
			break;

		case Decimal:
			if (flag_value)
			{
				this->flags |= 0x8;
			}
			else
			{
				this->flags &= ~0x8;
			}
			break;

		case Break:
			if (flag_value)
			{
				this->flags |= 0x10;
			}
			else
			{
				this->flags &= ~0x10;
			}
			break;

		case Overflow:
			if (flag_value)
			{
				this->flags |= 0x40;
			}
			else
			{
				this->flags &= ~0x40;
			}
			break;

		case Negative:
			if (flag_value)
			{
				this->flags |= 0x80;
			}
			else
			{
				this->flags &= ~0x80;
			}
			break;
	}
}


void CPU6502::stack_push(uint8_t data) {

	this->bus.write(STACK_BEGINNING + this->sp, data);
	this->sp--;
}


uint8_t CPU6502::stack_pull() {

	uint8_t data = this->bus.read(STACK_BEGINNING + (++this->sp));

	return data;
}


void CPU6502::check_if_overflow() {
	
	uint8_t data = this->bus.read(this->target_address);

	bool acc_7b = (this->acc & 0x80) >> 7;
	bool memory_7b = (data & 0x80) >> 7;
	bool r_7b = ((this->acc + data) & 0x80) >> 7;

	this->set_flag(Overflow, ~(acc_7b ^ memory_7b) & (acc_7b ^ r_7b));
}


void CPU6502::ADC() {

	uint8_t memory_data = this->bus.read(this->target_address);

	this->set_flag(Carry, (uint16_t)this->acc + memory_data + this->extract_flag(Carry) > 255);

	this->check_if_overflow();

	this->acc += memory_data + this->extract_flag(Carry);

	this->set_flag(Zero, this->acc == 0);	
	this->set_flag(Negative, this->acc >= 128);
}


void CPU6502::AND() {

	this->acc &= this->bus.read(this->target_address);

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}


void CPU6502::ASL() {

	if (this->acc_memory_switch == true) {

		this->set_flag(Carry, this->acc >= 128);

		this->acc <<= 1;

		this->set_flag(Zero, this->acc == 0);
		this->set_flag(Negative, this->acc >= 128);
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		this->set_flag(Carry, data >= 128);

		data <<= 1;

		this->bus.write(this->target_address, data);

		this->set_flag(Zero, data == 0);
		this->set_flag(Negative, data >= 128);
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
	
	this->set_flag(Overflow, (data & 0x40) >> 6 == 1);
	this->set_flag(Negative, (data & 0x80) >> 7 == 1);

	data &= this->acc;

	this->set_flag(Zero, data == 0);
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
	this->stack_push((this->pc >> 8) & 0x00FF);
	this->stack_push(this->pc & 0x00FF);

	uint8_t irq_lsb = this->bus.read(0xFFFE);
	uint8_t irq_msb = this->bus.read(0xFFFF);

	this->pc = ((uint16_t)irq_msb << 8) + (uint16_t)irq_lsb;

	this->set_flag(Break, true);

	this->stack_push(this->flags);
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
	this->flags ^= 0x40;
}

void CPU6502::CMP()
{
	uint8_t data = this->bus.read(this->target_address);

	this->set_flag(Carry, this->acc >= data);
	this->set_flag(Zero, this->acc == data);
	this->set_flag(Negative, (uint8_t)(this->acc - data) >= 128);
}

void CPU6502::CPX()
{
	uint8_t data = this->bus.read(this->target_address);

	this->set_flag(Carry, this->x >= data);
	this->set_flag(Zero, this->x == data);
	this->set_flag(Negative, (uint8_t)(this->x - data) >= 128);
}

void CPU6502::CPY()
{
	uint8_t data = this->bus.read(this->target_address);

	this->set_flag(Carry, this->y >= data);
	this->set_flag(Zero, this->y == data);
	this->set_flag(Negative, (uint8_t)(this->y - data) >= 128);
}

void CPU6502::DEC()
{
	uint8_t data = this->bus.read(this->target_address);

	this->bus.write(this->target_address, --data);

	this->set_flag(Zero, data == 0);
	this->set_flag(Negative, data >= 128);
}

void CPU6502::DEX()
{
	this->x--;

	this->set_flag(Zero, this->x == 0);
	this->set_flag(Negative, this->x >= 128);
}

void CPU6502::DEY()
{
	this->y--;

	this->set_flag(Zero, this->y == 0);
	this->set_flag(Negative, this->y >= 128);
}

void CPU6502::EOR()
{
	this->acc ^= this->bus.read(this->target_address);

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::INC()
{
	uint8_t data = this->bus.read(this->target_address);

	this->bus.write(this->target_address, ++data);

	this->set_flag(Zero, data == 0);
	this->set_flag(Negative, data >= 128);
}

void CPU6502::INX()
{
	this->x++;

	this->set_flag(Zero, this->x == 0);
	this->set_flag(Negative, this->x >= 128);
}

void CPU6502::INY()
{
	this->y++;

	this->set_flag(Zero, this->y == 0);
	this->set_flag(Negative, this->y >= 128);
}

void CPU6502::JMP()
{
	this->pc = this->target_address;
}

void CPU6502::JSR()
{	
	this->pc += 2;

	this->stack_push(this->pc >> 8);
	this->stack_push(this->pc & 0x00FF);

	this->pc = this->target_address;
}

void CPU6502::LDA()
{
	this->acc = this->bus.read(this->target_address);

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::LDX()
{
	this->x = this->bus.read(this->target_address);

	this->set_flag(Zero, this->x == 0);
	this->set_flag(Negative, this->x >= 128);
}

void CPU6502::LDY()
{
	this->y = this->bus.read(this->target_address);

	this->set_flag(Zero, this->y == 0);
	this->set_flag(Negative, this->y >= 128);
}

void CPU6502::LSR()
{
	if (this->acc_memory_switch == true) {

		this->set_flag(Carry, this->acc % 2);

		this->acc >>= 1;

		this->set_flag(Zero, this->acc == 0);
		this->set_flag(Negative, this->acc >= 128);
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		this->set_flag(Carry, data % 2);

		data >>= 1;

		this->bus.write(this->target_address, data);

		this->set_flag(Zero, data == 0);
		this->set_flag(Negative, data >= 128);
	}
}

void CPU6502::NOP()
{
	return;
}

void CPU6502::ORA()
{
	this->acc |= this->bus.read(this->target_address);

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
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

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::PLP()
{
	this->flags = this->stack_pull();
}

void CPU6502::ROL()
{
	bool carry = this->extract_flag(Carry);

	this->flags &= ~0x1;

	uint8_t tmp;

	if (this->acc_memory_switch == true) {

		this->flags |= (this->acc & 0x80) >> 7;

		tmp = this->acc << 1;
		tmp += carry;
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		this->flags |= (data & 0x80) >> 7;

		tmp = data << 1;
		tmp += carry;		
	}

	this->set_flag(Zero, tmp == 0);
	this->set_flag(Negative, tmp >= 128);
}

void CPU6502::ROR()
{
	bool carry = this->extract_flag(Carry);

	this->flags &= ~0x1;

	uint8_t tmp;

	if (this->acc_memory_switch == true) {

		this->flags |= (this->acc & 0x1);

		tmp = this->acc >> 1;
		tmp |= carry << 7;
	}
	else {

		uint8_t data = this->bus.read(this->target_address);

		this->flags |= (data & 0x1);

		tmp = data >> 1;
		tmp |= carry << 7;		
	}

	this->set_flag(Zero, tmp == 0);
	this->set_flag(Negative, tmp >= 128);
}

void CPU6502::RTI()
{
	this->flags = this->stack_pull();
	this->pc = (uint16_t)this->stack_pull() + ((uint16_t)this->stack_pull() << 8);
}

void CPU6502::RTS()
{
	this->pc = (uint16_t)this->stack_pull() + ((uint16_t)this->stack_pull() << 8);
}

void CPU6502::SBC()
{
	this->check_if_overflow();

	this->acc -= this->bus.read(this->target_address) - ~(this->extract_flag(Carry));

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::SEC()
{
	this->set_flag(Carry, true);
}

void CPU6502::SED()
{
	this->set_flag(Decimal, true);
}

void CPU6502::SEI()
{
	this->set_flag(Interrupt, true);
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

	this->set_flag(Zero, this->x == 0);
	this->set_flag(Negative, this->x >= 128);
}

void CPU6502::TAY()
{
	this->y = this->acc;

	this->set_flag(Zero, this->y == 0);
	this->set_flag(Negative, this->y >= 128);
}

void CPU6502::TSX()
{
	this->x = this->sp;

	this->set_flag(Zero, this->x == 0);
	this->set_flag(Negative, this->x >= 128);
}

void CPU6502::TXA()
{
	this->acc = this->x;

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::TXS()
{
	this->sp = this->x;
}

void CPU6502::TYA()
{
	this->acc = this->y;

	this->set_flag(Zero, this->y == 0);
	this->set_flag(Negative, this->y >= 128);
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

	this->target_address = ((uint16_t)most_significant_bit << 8) + (uint16_t)least_significant_bit;
}

void CPU6502::mod_iidrx()
{
	uint8_t lsb_loc = (this->instr_operand & 0x00FF) + this->x;
	uint8_t msb_loc = (this->instr_operand & 0x00FF) + (this->x + 1);

	uint8_t least_significant_bit = this->bus.read(lsb_loc);
	uint8_t most_significant_bit = this->bus.read(msb_loc);

	this->target_address = ((uint16_t)most_significant_bit << 8) + (uint16_t)least_significant_bit;
}

void CPU6502::mod_idriy()
{
	uint8_t lsb_loc = (this->instr_operand & 0x00FF);
	uint8_t msb_loc = (this->instr_operand & 0x00FF) + 1;

	uint8_t least_significant_bit = this->bus.read(lsb_loc);
	uint8_t most_significant_bit = this->bus.read(msb_loc);

	this->target_address = ((uint16_t)most_significant_bit << 8) + (uint16_t)least_significant_bit + this->y;
}