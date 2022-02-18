#include "CPU6502.hpp"
#include "CpuBus.hpp"

#include <iostream>

// linijka 3637 - błąd testu

void CPU6502::connect_bus(CpuBus* cpu_bus){ this->cpu_bus = cpu_bus; }


void CPU6502::power_up()
{
	uint8_t vector_lsb = this->cpu_bus->read(0xFFFC); // Setting program counter to address
												   	 //  stored in reset vector
	uint8_t vector_msb = this->cpu_bus->read(0xFFFD);

	this->pc = (uint16_t)(vector_msb << 8) + (uint16_t)vector_lsb;
	this->status.set_value(0x34);
	this->acc = 0;
	this->x = 0;
	this->y = 0;
	this->sp = 0xFD;
	this->cpu_bus->write(0x4015, 0);
	this->cpu_bus->write(0x4017, 0);

	for(int i = 0; i <= 16; i++)
		this->cpu_bus->write(0x4000 + i, 0);
	
	for(int i = 0; i <= 3; i++)
		this->cpu_bus->write(0x4010 + i, 0);
}


void CPU6502::cycle() 
{ 
	// Fetch
	
	this->instr_opcode = this->cpu_bus->read(this->pc); 
	
	// Decode

	CPU6502::Instruction cur_instruction = this->op_map[this->instr_opcode]; 
	std::string addressing_name = cur_instruction.op_name.substr(4, cur_instruction.op_name.length());
	uint8_t operand_bytes = this->operand_bytes[addressing_name];

	this->pc++;

	if(operand_bytes == 1)
	{
		this->instr_operand = this->cpu_bus->read(this->pc);
		this->pc++;
	}
	else if(operand_bytes == 2)
	{
		this->instr_operand = this->cpu_bus->read(this->pc);
		this->pc++;
		this->instr_operand += this->cpu_bus->read(this->pc) << 8;
		this->pc++;
	}

	// Execute

	(this->*(cur_instruction.adr_mod))();	
	(this->*(cur_instruction.operation))();
}

void CPU6502::clear_memory(){  }

void CPU6502::write(uint16_t address, uint8_t data) { this->cpu_bus->write(address, data); }

uint8_t CPU6502::read(uint16_t address){ return this->cpu_bus->read(address); }

bool CPU6502::extract_flag(CPU6502::Flags flag) { return this->status.get_bit(flag); }

void CPU6502::set_flag(CPU6502::Flags flag, bool flag_value) { this->status.set_bit(flag, flag_value); }

void CPU6502::stack_push(uint8_t data) 
{
	this->cpu_bus->write(STACK_BEGINNING + this->sp, data);
	this->sp--;
}

uint8_t CPU6502::stack_pull() 
{
	uint8_t data = this->cpu_bus->read(STACK_BEGINNING + (++this->sp));
	return data;
}

void CPU6502::reset()
{
	if(this->extract_flag(InterruptDisable) == false)
	{
		uint8_t vector_lsb = this->cpu_bus->read(0xFFFC);
		uint8_t vector_msb = this->cpu_bus->read(0xFFFD);

		this->pc = (uint16_t)(vector_msb << 8) + (uint16_t)vector_lsb;
		this->set_flag(InterruptDisable, true);
		this->sp -= 3;
	}
}

void CPU6502::irq()
{
	if(this->extract_flag(InterruptDisable) == false)
	{
		uint8_t vector_lsb = this->cpu_bus->read(0xFFFE);
		uint8_t vector_msb = this->cpu_bus->read(0xFFFF);

		this->pc = (uint16_t)(vector_msb << 8) + (uint16_t)vector_lsb;
	}
}

void CPU6502::nmi()
{
	uint8_t vector_lsb = this->cpu_bus->read(0xFFFA);
	uint8_t vector_msb = this->cpu_bus->read(0xFFFB);

	this->pc = (uint16_t)(vector_msb << 8) + (uint16_t)vector_lsb;
}

void CPU6502::ADC()
{
	uint8_t memory_data = this->data_extracted;
	bool carry = this->extract_flag(Carry);
	uint16_t result = this->acc + memory_data + carry;

	bool acc_7b = (this->acc & 0x80) >> 7;
	bool memory_7b = (memory_data & 0x80) >> 7;
	bool r_7b = (result & 0x80) >> 7;
	
	this->set_flag(Overflow, ~(acc_7b ^ memory_7b) & (acc_7b ^ r_7b));
	this->set_flag(Carry, result > 255);

	this->acc = result;

	this->set_flag(Zero, this->acc == 0);	
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::AND() 
{
	this->acc &= this->data_extracted;

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::ASL() 
{
	this->set_flag(Carry, this->data_extracted >= 128);

	this->data_extracted <<= 1;

	if(this->op_map[this->instr_opcode].adr_mod == &CPU6502::mod_acc)
		this->acc = this->data_extracted;
	else
		this->cpu_bus->write(this->target_address, this->data_extracted);

	this->set_flag(Zero, this->data_extracted == 0);
	this->set_flag(Negative, this->data_extracted >= 128);	
}

void CPU6502::BCC()
{
	if (this->extract_flag(Carry) == false)
		this->pc = this->instr_operand;
}

void CPU6502::BCS()
{
	if (this->extract_flag(Carry) == true)
		this->pc = this->instr_operand;
}

void CPU6502::BEQ()
{
	if (this->extract_flag(Zero) == true)
		this->pc = this->instr_operand;
}

void CPU6502::BIT()
{
	uint8_t data = this->data_extracted;
	
	this->set_flag(Overflow, ((data & 0x40) >> 6) == 1);
	this->set_flag(Negative, ((data & 0x80) >> 7) == 1);

	data &= this->acc;

	this->set_flag(Zero, data == 0);
}

void CPU6502::BMI()
{
	if (this->extract_flag(Negative) == true)
		this->pc = this->instr_operand;
}

void CPU6502::BNE()
{
	if (this->extract_flag(Zero) == false)
		this->pc = this->instr_operand;
}

void CPU6502::BPL()
{
	if (this->extract_flag(Negative) == false)
		this->pc = this->instr_operand;
}

void CPU6502::BRK()
{
	this->stack_push((this->pc >> 8) & 0x00FF);
	this->stack_push(this->pc & 0x00FF);

	uint8_t irq_lsb = this->cpu_bus->read(0xFFFE);
	uint8_t irq_msb = this->cpu_bus->read(0xFFFF);

	this->pc = ((uint16_t)irq_msb << 8) + (uint16_t)irq_lsb;

	this->set_flag(InterruptDisable, true);
	this->set_flag(Break, true);
	this->set_flag(Unused, true);

	this->stack_push(this->status.get_value());

	this->set_flag(Break, false);
}

void CPU6502::BVC()
{
	if (this->extract_flag(Overflow) == false)
		this->pc = this->instr_operand;
}

void CPU6502::BVS()
{
	if (this->extract_flag(Overflow) == true)
		this->pc = this->instr_operand;
}

void CPU6502::CLC()
{
	this->set_flag(Carry, false);
}

void CPU6502::CLD()
{
	this->set_flag(Decimal, false);
}

void CPU6502::CLI()
{
	this->set_flag(InterruptDisable, false);
}

void CPU6502::CLV()
{
	this->set_flag(Overflow, false);
}

void CPU6502::CMP()
{
	uint8_t data = this->data_extracted;

	this->set_flag(Carry, this->acc >= data);
	this->set_flag(Zero, this->acc == data);
	this->set_flag(Negative, (uint8_t)(this->acc - data) >= 128);
}

void CPU6502::CPX()
{
	uint8_t data = this->data_extracted;

	this->set_flag(Carry, this->x >= data);
	this->set_flag(Zero, this->x == data);
	this->set_flag(Negative, (uint8_t)(this->x - data) >= 128);
}

void CPU6502::CPY()
{
	uint8_t data = this->data_extracted;

	this->set_flag(Carry, this->y >= data);
	this->set_flag(Zero, this->y == data);
	this->set_flag(Negative, (uint8_t)(this->y - data) >= 128);
}

void CPU6502::DEC()
{
	this->data_extracted--;

	this->cpu_bus->write(this->target_address, this->data_extracted);

	this->set_flag(Zero, this->data_extracted == 0);
	this->set_flag(Negative, this->data_extracted >= 128);
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
	this->acc ^= this->data_extracted;

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::INC()
{
	this->data_extracted++;

	this->cpu_bus->write(this->target_address, this->data_extracted);

	this->set_flag(Zero, this->data_extracted == 0);
	this->set_flag(Negative, this->data_extracted >= 128);
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
	this->pc = this->instr_operand;
}

void CPU6502::JSR()
{	
	this->pc--;

	this->stack_push(this->pc >> 8);
	this->stack_push(this->pc & 0x00FF);

	this->pc = this->instr_operand;
}

void CPU6502::LDA()
{
	this->acc = this->data_extracted;

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::LDX()
{
	this->x = this->data_extracted;

	this->set_flag(Zero, this->x == 0);
	this->set_flag(Negative, this->x >= 128);
}

void CPU6502::LDY()
{
	this->y = this->data_extracted;

	this->set_flag(Zero, this->y == 0);
	this->set_flag(Negative, this->y >= 128);
}

void CPU6502::LSR()
{
	this->set_flag(Carry, this->data_extracted % 2);

	this->data_extracted >>= 1;

	if(this->op_map[this->instr_opcode].adr_mod == &CPU6502::mod_acc)
		this->acc = this->data_extracted;
	else
		this->cpu_bus->write(this->target_address, this->data_extracted);

	this->set_flag(Zero, this->data_extracted == 0);
	this->set_flag(Negative, this->data_extracted >= 128);
}

void CPU6502::NOP()
{
	return;
}

void CPU6502::ORA()
{
	this->acc |= this->data_extracted;

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::PHA()
{
	this->stack_push(this->acc);
}

void CPU6502::PHP()
{
	this->set_flag(Unused, true);
	this->set_flag(Break, true);

	this->stack_push(this->status.get_value());

	this->set_flag(Unused, false);
	this->set_flag(Break, false);
}

void CPU6502::PLA()
{
	this->acc = this->stack_pull();

	this->set_flag(Zero, this->acc == 0);
	this->set_flag(Negative, this->acc >= 128);
}

void CPU6502::PLP()
{
	this->status.set_value(this->stack_pull());
	this->set_flag(Unused, true);
}

void CPU6502::ROL()
{
	bool carry = this->extract_flag(Carry);

	this->status.set_value(this->status.get_value() & ~0x1);
	this->status.set_value(this->status.get_value() | (this->data_extracted & 0x80) >> 7);

	this->data_extracted <<= 1;
	this->data_extracted += carry;

	if(this->op_map[this->instr_opcode].adr_mod == &CPU6502::mod_acc)
		this->acc = this->data_extracted;
	else
		this->cpu_bus->write(this->target_address, this->data_extracted);

	this->set_flag(Zero, this->data_extracted == 0);
	this->set_flag(Negative, this->data_extracted >= 128);
}

void CPU6502::ROR()
{
	bool carry = this->extract_flag(Carry);

	this->status.set_value(this->status.get_value() & ~0x1);
	this->status.set_value(this->status.get_value() | (this->data_extracted & 0x1));

	this->data_extracted >>= 1;
	this->data_extracted |= carry << 7;

	if(this->op_map[this->instr_opcode].adr_mod == &CPU6502::mod_acc)
		this->acc = this->data_extracted;
	else
		this->cpu_bus->write(this->target_address, this->data_extracted);

	this->set_flag(Zero, this->data_extracted == 0);
	this->set_flag(Negative, this->data_extracted >= 128);
}

void CPU6502::RTI()
{
	this->status.set_value(this->stack_pull());
	
	this->set_flag(Unused, false);
	this->set_flag(Break, false);

	this->pc = (uint16_t)this->stack_pull() + ((uint16_t)this->stack_pull() << 8);
}

void CPU6502::RTS()
{
	this->pc = (uint16_t)this->stack_pull();
	this->pc += ((uint16_t)this->stack_pull() << 8) + 1;
}

void CPU6502::SBC()
{
	uint8_t memory_data = this->data_extracted;	
	bool carry = this->extract_flag(Carry);
	uint16_t result = this->acc + ~memory_data + carry;

	bool acc_7b = (this->acc & 0x80) >> 7;
	bool memory_7b = (memory_data & 0x80) >> 7;
	bool r_7b = (result & 0x80) >> 7;

	this->set_flag(Overflow, (acc_7b ^ memory_7b) & (acc_7b ^ r_7b));
	this->set_flag(Carry, !(result > 255));

	this->acc = result;

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
	this->set_flag(InterruptDisable, true);
}

void CPU6502::STA()
{
	this->cpu_bus->write(this->target_address, this->acc);
}

void CPU6502::STX()
{
	this->cpu_bus->write(this->target_address, this->x);
}

void CPU6502::STY()
{
	this->cpu_bus->write(this->target_address, this->y);
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

void CPU6502::UNK()
{
	return;
}

void CPU6502::LAX()
{
	
}

void CPU6502::mod_abs()
{
	this->target_address = this->instr_operand;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_acc()
{
	this->data_extracted = this->acc;
}

void CPU6502::mod_zp()
{
	this->target_address = (this->instr_operand & 0x00FF);
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_zpx()
{	  
	this->target_address = (this->instr_operand + this->x) & 0x00FF;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_zpy()
{
	this->target_address = (this->instr_operand + this->y) & 0x00FF;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_absx()
{
	this->target_address = this->instr_operand + this->x;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_absy()
{
	this->target_address = this->instr_operand + this->y;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_imd() 
{
	this->data_extracted = instr_operand;
}

void CPU6502::mod_rel()
{
	int8_t signed_operand = this->instr_operand;
	this->instr_operand = signed_operand + this->pc;
}

void CPU6502::mod_imp()
{
	return;
}

void CPU6502::mod_idr()
{
	uint8_t least_significant_byte = this->cpu_bus->read(this->instr_operand);
	uint8_t most_significant_byte;

	/* Implementation of hardware bug. If least significant byte is 0xFF,
	   then page is not crossed when reading most significant byte, instead
	   address is wrapped, so if operand is $02FF, then lsb is read from
	   $02FF and msb is read from $0200 instead of $0300. */

	if((this->instr_operand & 0x00FF) == 0x00FF)		
		most_significant_byte = this->cpu_bus->read(this->instr_operand & 0xFF00);
	else
		most_significant_byte = this->cpu_bus->read(this->instr_operand + 1);

	this->target_address = ((uint16_t)most_significant_byte << 8) + (uint16_t)least_significant_byte;
	this->instr_operand = this->target_address;
}

void CPU6502::mod_idrx()
{
	uint8_t lsb_loc = this->instr_operand + this->x;
	uint8_t msb_loc = this->instr_operand + this->x + 1;

	uint8_t least_significant_byte = this->cpu_bus->read(lsb_loc);
	uint8_t most_significant_byte = this->cpu_bus->read(msb_loc);

	this->target_address = ((uint16_t)most_significant_byte << 8) + (uint16_t)least_significant_byte;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}

void CPU6502::mod_idry()
{
	uint8_t lsb_loc = (this->instr_operand & 0x00FF);
	uint8_t msb_loc = (this->instr_operand & 0x00FF) + 1;

	uint8_t least_significant_byte = this->cpu_bus->read(lsb_loc);
	uint8_t most_significant_byte = this->cpu_bus->read(msb_loc);

	this->target_address = ((uint16_t)most_significant_byte << 8) + (uint16_t)least_significant_byte + this->y;
	this->data_extracted = this->cpu_bus->read(this->target_address);
}