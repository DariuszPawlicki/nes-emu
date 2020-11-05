#include "CPU6502.hpp"
#include <iostream>



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
	
	Instruction cur_instruction = op_map[this->instr_opcode];

	(this->*(cur_instruction.adr_mod))();
	
}


void CPU6502::ADC() {

	uint8_t memory_data = this->bus.read(this->target_address);

	if ((this->acc + memory_data + this->flags[7]) > 255) {
		this->flags[7] = true;
	}

	this->acc += memory_data + this->flags[7];

	if (this->acc == 0)
		this->flags[6] = true;
	
	if (this->acc >= 128) // If bit 7 of result is set to 1 
		this->flags[0] = true;
}


void CPU6502::AND() {

	this->acc &= this->bus.read(this->target_address);

	if (this->acc == 0)
		this->flags[6] = true;

	if (this->acc >= 128)
		this->flags[0] = true;
}


void CPU6502::ASL() {


}


void CPU6502::mod_imd() {

	std::cout << "IMD";
}