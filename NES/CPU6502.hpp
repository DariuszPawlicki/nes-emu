#pragma once

#include <cstdint>

class CPU6502
{
	private:

		uint16_t pc{ 0 };
		uint8_t sp{ 0 };
		uint8_t acc{ 0 };
		uint8_t x{ 0 };
		uint8_t y{ 0 };
		uint8_t p{ 0 };

	public:

		//INSTRUCTIONS

		//ADDRESSING MODES

		void ABS(); // Absolute - use 2 bytes operand as address e.g. JMP $a000

		void ZP();  // Zero page - use 1 byte operand as address e.g. STA $01

		void ZPX(); // Zero page X - use 2 bytes operand, first byte is start location at memory
		            // second is an offset e.g. STA $01, X where X = #$02, will store
		            // accumulator in memory address $03

		void ZPY(); // Zero page Y - same as ZPX but can only be used with
		            // for storing, loading from/to X register e.g. STX, LDX

		void ABSX(); // Absolute X - same as zero page, but uses 2 bytes address $a000, X

		void ABSY(); // Absolute Y - same as ABSX but uses an Y register

		void IMD(); // Immediate - storing operand value in certain register sepcified
		            // by opcode e.g. LDX #$01, LDY #$01, stores 1 in X and Y registers

		void REL(); // Relative - it takes number of bytes as operand that
		            // are specifying an offset from current instruction which is added to PC,
		            // it's used for branching

		void IMP(); // Implicit - instuctions like for example INX don't take any operand,
		            // memory location is implied by the opcode

		void IDR(); // Indirect - takes an absolute address as a location of least significant
		            // byte of target address, next byte is the most significant byte of an address,
		            // this addressing mode concatenates those two bytes into address of
		            // target memory location e.g. $2000 = $01, $2001 = $02, LDX ($2000) 
					// concatenate bytes from two memory cells into address $0201 and
		            // load content of this cell into X register
		            // It works only with JMP instruction

		void IIDRX(); // Indexed indirect X - combination of zero page X and indirect addressing
		              // e.g. $03 = $a0, $04 = $ff, X = $02, LDA($01, X) will load to accumulator
		              // content of $ffa0 memory cell

		void IDRIY(); // Indirect indexed Y - similar to indexed indirect X, but
		              // content of Y register is added after derefernce
		              // for example $01 = $a0, $02 = $bc, Y = $01, LDA($01),Y ,  will
		              // load to accumulator content of memory cell with address $bca1
};