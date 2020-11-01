#pragma once

#include <cstdint>
#include <string>
#include <map>

class CPU6502
{

	typedef void (CPU6502::*op_ptr)();
	typedef void (CPU6502::*adrmod_ptr)();


	struct Instruction {

		std::string op_name;
		op_ptr operation;
		adrmod_ptr adr_mod;
		uint8_t cycles;
		bool add_cycle;

		Instruction(std::string op_name, op_ptr operation,
				    adrmod_ptr adr_mod, uint8_t cycles, bool add_cycle) : 
									op_name(op_name), operation(operation),
									adr_mod(adr_mod), cycles(cycles), add_cycle(add_cycle) {}

		Instruction(std::string op_name, op_ptr operation,
					adrmod_ptr adr_mod, uint8_t cycles) :
									op_name(op_name), operation(operation),
									adr_mod(adr_mod), cycles(cycles), add_cycle(false) {}

		Instruction(){}
	};


	public:

		uint16_t pc{ 0 };
		uint8_t sp{ 0 };
		uint8_t acc{ 0 };
		uint8_t x{ 0 };
		uint8_t y{ 0 };
		uint8_t flags{ 0 }; // Processor status flags - starting from most significant bit -
		                    // N V B D I Z C - Negative, Overflow, Break cmd, Decimal mode, 
		                    // Interrupt disable, Zero flag, Carry flag

		std::map<uint8_t, Instruction> op_map{

			{0x69, Instruction("ADC_IMD", &CPU6502::ADC, &CPU6502::mod_imd, 2)},
			{0x65, Instruction("ADC_ZP", &CPU6502::ADC, &CPU6502::mod_zp, 3)},
			{0x75, Instruction("ADC_ZPX", &CPU6502::ADC, &CPU6502::mod_zpx, 4)},
			{0x6D, Instruction("ADC_ABS", &CPU6502::ADC, &CPU6502::mod_abs, 4)},
			{0x7D, Instruction("ADC_ABSX", &CPU6502::ADC, &CPU6502::mod_absx, 4, true)}, // +1 if page crossed
			{0x79, Instruction("ADC_ABSY", &CPU6502::ADC, &CPU6502::mod_absy, 4, true)}, // +1
			{0x61, Instruction("ADC_IIDRX", &CPU6502::ADC, &CPU6502::mod_iidrx, 6)}, 
			{0x71, Instruction("ADC_IDRIY", &CPU6502::ADC, &CPU6502::mod_idriy, 5, true)}, // +1

			{0x29, Instruction("AND_IMD", &CPU6502::AND, &CPU6502::mod_imd, 2)},
			{0x25, Instruction("AND_ZP", &CPU6502::AND, &CPU6502::mod_zp, 3)},
			{0x35, Instruction("AND_ZPX", &CPU6502::AND, &CPU6502::mod_zpx, 4)},
			{0x2D, Instruction("AND_ABS", &CPU6502::AND, &CPU6502::mod_abs, 4)},
			{0x3D, Instruction("AND_ABSX", &CPU6502::AND, &CPU6502::mod_absx, 4, true)}, // +1
			{0x39, Instruction("AND_ABSY", &CPU6502::AND, &CPU6502::mod_absy, 4, true)},
			{0x21, Instruction("AND_IIDRX", &CPU6502::AND, &CPU6502::mod_iidrx, 6)},
			{0x31, Instruction("AND_IDRIY", &CPU6502::AND, &CPU6502::mod_iidrx, 5, true)}

		};


	public:

		CPU6502();

		//INSTRUCTIONS

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


		//ADDRESSING MODES

		void mod_abs(); // Absolute - use 2 bytes operand as address e.g. JMP $a000

		void mod_zp();  // Zero page - use 1 byte operand as address e.g. STA $01

		void mod_zpx(); // Zero page X - use 2 bytes operand, first byte is start location at memory
					// second is an offset e.g. STA $01, X where X = #$02, will store
					// accumulator in memory address $03

		void mod_zpy(); // Zero page Y - same as ZPX but can only be used with
					// for storing, loading from/to X register e.g. STX, LDX

		void mod_absx(); // Absolute X - same as zero page, but uses 2 bytes address $a000, X

		void mod_absy(); // Absolute Y - same as ABSX but uses an Y register

		void mod_imd(); // Immediate - storing operand value in certain register sepcified
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

		void mod_iidrx(); // Indexed indirect X - combination of zero page X and indirect addressing
					  // e.g. $03 = $a0, $04 = $ff, X = $02, LDA($01, X) will load to accumulator
					  // content of $ffa0 memory cell

		void mod_idriy(); // Indirect indexed Y - similar to indexed indirect X, but
					  // content of Y register is added after derefernce
					  // for example $01 = $a0, $02 = $bc, Y = $01, LDA($01),Y ,  will
					  // load to accumulator content of memory cell with address $bca1

};