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

		void ABS(); // Absolute
		void ZP();  // Zero page
		void ZPX(); // Zero page X
		void ZPY(); // Zero page Y
		void ABSX(); // Absolute X
		void ABSY(); // Absolute Y
		void IMD(); // Immediate
		void REL(); // Relative
		void IMP(); // Implied
		void IDR(); // Indirect
		void IIDRX(); // Indexed indirect X
		void IDRIY(); // Indirect indexed Y
};