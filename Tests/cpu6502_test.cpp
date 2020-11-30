#include "pch.h"
#include "../NES/CPU6502.cpp"
#include "../NES/Bus.cpp"



struct CPU6502Test :public::testing::Test {

	CPU6502* processor;

	void SetUp() override{
		
		processor = new CPU6502();
		processor->target_address = 0x600;
		processor->bus.write(processor->target_address, 0xFF);
		processor->sp = 0xFF;
	}

	void TearDown() override{
		delete processor;
	}
};


TEST_F(CPU6502Test, StackPush) {

	uint16_t address = processor->STACK_BEGINNING + processor->sp;

	processor->stack_push(0x8);

	ASSERT_EQ(processor->sp, 0xFE);
	ASSERT_EQ(processor->bus.read(address), 0x8);
}


TEST_F(CPU6502Test, StackPull) {

	processor->stack_push(0x8);
	processor->stack_push(0x12);

	ASSERT_EQ(processor->stack_pull(), 0x12);
	ASSERT_EQ(processor->stack_pull(), 0x8);
	ASSERT_EQ(processor->sp, 0xFF);
}


TEST_F(CPU6502Test, CheckIfOverflow) {

	processor->acc = 0x50;
	processor->bus.write(processor->target_address, 0x50);

	processor->check_if_overflow();

	ASSERT_EQ(processor->extract_flag(CPU6502::Flags::Overflow), true);

	processor->bus.write(processor->target_address, 0x90);

	processor->check_if_overflow();

	ASSERT_EQ(processor->extract_flag(CPU6502::Flags::Overflow), false);

	processor->acc = 0xd0;

	processor->check_if_overflow();

	ASSERT_EQ(processor->extract_flag(CPU6502::Flags::Overflow), true);
}


TEST_F(CPU6502Test, And) {

	processor->acc = 0b01111111;
	processor->AND();

	ASSERT_EQ(processor->acc, 0b01111111);
	ASSERT_EQ(processor->flags, 0b00000000);

	processor->acc = 0b00;
	processor->AND();

	ASSERT_EQ(processor->acc, 0b00000000);
	ASSERT_EQ(processor->flags, 0b00000010);

	processor->acc = 0b10000000;
	processor->AND();

	ASSERT_EQ(processor->acc, 0b10000000);
	ASSERT_EQ(processor->flags, 0b10000000);
}


TEST_F(CPU6502Test, AslOnAcc) {

	processor->acc_memory_switch = true;
	processor->acc = 0b10000001;	
	processor->ASL();

	ASSERT_EQ(processor->acc, 0b00000010);
	ASSERT_EQ(processor->flags, 0b00000001);

	processor->acc = 0b01000000;
	processor->ASL();

	ASSERT_EQ(processor->acc, 0b10000000);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->acc = 0;
	processor->ASL();

	ASSERT_EQ(processor->acc, 0);
	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, AslOnMemory) {

	processor->acc_memory_switch = false;
	processor->bus.write(processor->target_address, 0b10000001);
	processor->ASL();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0b00000010);
	ASSERT_EQ(processor->flags, 0b00000001);

	processor->bus.write(processor->target_address, 0b01000000);
	processor->ASL();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0b10000000);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 0b10000000);
	processor->ASL();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0);
	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, BccCarryUnset) {

	processor->instr_operand = 8;
	processor->BCC();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, BccCarrySet) {

	processor->instr_operand = 8;
	processor->flags = 0b00000001;
	processor->BCC();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BcsCarryUnset) {

	processor->instr_operand = 8;
	processor->BCS();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BcsCarrySet) {

	processor->instr_operand = 8;
	processor->flags = 0b00000001;
	processor->BCS();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, BeqZeroUnset) {

	processor->instr_operand = 8;
	processor->BEQ();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BeqZeroSet) {

	processor->instr_operand = 8;
	processor->flags = 0b00000010;
	processor->BEQ();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, Bit) {

	processor->bus.write(processor->target_address, 0b11000000);
	processor->BIT();

	ASSERT_EQ(processor->flags, 0b11000010);
}


TEST_F(CPU6502Test, BmiNegativeUnset) {

	processor->instr_operand = 8;
	processor->flags = 0b10000000;
	processor->BMI();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, BmiNegativeSet) {

	processor->instr_operand = 8;
	processor->BMI();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BneZeroUnset) {

	processor->instr_operand = 8;
	processor->BNE();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, BneZeroSet) {

	processor->instr_operand = 8;
	processor->flags = 0b00000010;
	processor->BNE();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BplNegativeUnset) {

	processor->instr_operand = 8;
	processor->BPL();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, BplNegativeSet) {

	processor->instr_operand = 8;
	processor->flags = 0b10000000;
	processor->BPL();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, Brk) {

	processor->pc = 0x5678;
	processor->flags = 0b11001100;
	processor->bus.write(0xFFFE, 0x10);
	processor->bus.write(0xFFFF, 0x60);
	processor->BRK();

	ASSERT_EQ(processor->stack_pull(), 0b11011100);
	ASSERT_EQ(processor->stack_pull(), 0x78);
	ASSERT_EQ(processor->stack_pull(), 0x56);
	ASSERT_EQ(processor->pc, 0x6010);
}


TEST_F(CPU6502Test, BvcUnset) {

	processor->instr_operand = 8;
	processor->BVC();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, BvcSet) {

	processor->instr_operand = 8;
	processor->flags = 0b01000000;
	processor->BVC();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BvsUnset) {

	processor->instr_operand = 8;
	processor->BVS();

	ASSERT_EQ(processor->pc, 0);
}


TEST_F(CPU6502Test, BvsSet) {

	processor->instr_operand = 8;
	processor->flags = 0b01000000;
	processor->BVS();

	ASSERT_EQ(processor->pc, 8);
}


TEST_F(CPU6502Test, Clc) {

	processor->flags = 0b00000001;
	processor->CLC();

	ASSERT_EQ(processor->flags, 0);
}


TEST_F(CPU6502Test, Cld) {

	processor->flags = 0b00001000;
	processor->CLD();

	ASSERT_EQ(processor->flags, 0);
}


TEST_F(CPU6502Test, Cli) {

	processor->flags = 0b00000100;
	processor->CLI();

	ASSERT_EQ(processor->flags, 0);
}


TEST_F(CPU6502Test, Clv) {

	processor->flags = 0b01000000;
	processor->CLV();

	ASSERT_EQ(processor->flags, 0);
}


TEST_F(CPU6502Test, Cmp) {

	processor->acc = 0x40;
	processor->bus.write(processor->target_address, 0x45);

	processor->CMP();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->acc = 0x50;

	processor->CMP();

	ASSERT_EQ(processor->flags, 0b00000001);

	processor->acc = 0x45;

	processor->CMP();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, Cpx) {

	processor->x = 0x40;
	processor->bus.write(processor->target_address, 0x45);

	processor->CPX();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->x = 0x50;

	processor->CPX();

	ASSERT_EQ(processor->flags, 0b00000001);

	processor->x = 0x45;

	processor->CPX();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, Cpy) {

	processor->y = 0x40;
	processor->bus.write(processor->target_address, 0x45);

	processor->CPY();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->y = 0x50;

	processor->CPY();

	ASSERT_EQ(processor->flags, 0b00000001);

	processor->y = 0x45;

	processor->CPY();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, Dec) {

	processor->DEC();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0xFE);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 1);

	processor->DEC();

	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Dex) {

	processor->x = 0xFF;
	processor->DEX();

	ASSERT_EQ(processor->x, 0xFE);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->x = 1;

	processor->DEX();

	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Dey) {

	processor->y = 0xFF;
	processor->DEY();

	ASSERT_EQ(processor->y, 0xFE);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->y = 1;

	processor->DEY();

	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Eor) {

	processor->acc = 0b10101010;
	processor->bus.write(processor->target_address, 0b11010100);

	processor->EOR();

	ASSERT_EQ(processor->acc, 0b01111110);
}


TEST_F(CPU6502Test, Inc) {

	processor->INC();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0);
	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Inx) {

	processor->x = 0x8;
	processor->INX();

	ASSERT_EQ(processor->x, 9);
	ASSERT_EQ(processor->flags, 0b00000000);
}


TEST_F(CPU6502Test, Iny) {

	processor->y = 0x8;
	processor->INY();

	ASSERT_EQ(processor->y, 9);
	ASSERT_EQ(processor->flags, 0b00000000);
}


TEST_F(CPU6502Test, Jmp) {

	processor->target_address = 0x200;

	processor->JMP();

	ASSERT_EQ(processor->pc, 0x200);
}


TEST_F(CPU6502Test, Jsr) {

	processor->target_address = 0x610;

	processor->JSR();

	ASSERT_EQ(processor->pc, 0x610);
	ASSERT_EQ(processor->stack_pull(), 0x2);
}


TEST_F(CPU6502Test, Lda) {

	processor->LDA();

	ASSERT_EQ(processor->acc, 0xFF);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 0);

	processor->LDA();

	ASSERT_EQ(processor->acc, 0);
	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Ldx) {

	processor->LDX();

	ASSERT_EQ(processor->x, 0xFF);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 0);

	processor->LDX();

	ASSERT_EQ(processor->x, 0);
	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Ldy) {

	processor->LDY();

	ASSERT_EQ(processor->y, 0xFF);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 0);

	processor->LDY();

	ASSERT_EQ(processor->y, 0);
	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, LsrOnAcc) {

	processor->acc_memory_switch = true;

	processor->acc = 0b10000001;
	processor->LSR();

	ASSERT_EQ(processor->acc, 0b01000000);
	ASSERT_EQ(processor->flags, 0b00000001);

	processor->acc = 0b00000001;
	processor->LSR();

	ASSERT_EQ(processor->acc, 0b00000000);
	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, LsrOnMemory) {

	processor->bus.write(processor->target_address, 0b10000001);
	processor->LSR();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0b01000000);
	ASSERT_EQ(processor->flags, 0b00000001);

	processor->bus.write(processor->target_address, 0b00000001);
	processor->LSR();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0b00000000);
	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, Ora) {

	processor->acc = 0x80;
	processor->ORA();

	ASSERT_EQ(processor->acc, 0xFF);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->acc = 0;
	processor->bus.write(processor->target_address, 0);

	processor->ORA();

	ASSERT_EQ(processor->acc, 0);
	ASSERT_EQ(processor->flags, 0b00000010);
}


TEST_F(CPU6502Test, Pha) {

	processor->acc = 0x12;
	processor->PHA();

	ASSERT_EQ(processor->stack_pull(), 0x12);
}


TEST_F(CPU6502Test, Php) {

	processor->flags = 0b10101010;

	processor->PHP();

	ASSERT_EQ(processor->stack_pull(), 0b10101010);
}


TEST_F(CPU6502Test, Pla) {

	processor->stack_push(0x24);
	processor->PLA();

	ASSERT_EQ(processor->acc, 0x24);
}


TEST_F(CPU6502Test, Plp) {

	processor->stack_push(0b10101010);
	processor->PLP();

	ASSERT_EQ(processor->flags, 0b10101010);
}


TEST_F(CPU6502Test, RolOnAcc) {

	processor->acc_memory_switch = true;

	processor->set_flag(CPU6502::Flags::Carry, true);

	processor->acc = 0b00101010;

	processor->ROL();

	ASSERT_EQ(processor->flags, 0b00000000);

	processor->acc = 0b01000000;

	processor->ROL();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->acc = 0b10000000;

	processor->ROL();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, RolOnMemory) {

	processor->set_flag(CPU6502::Flags::Carry, true);

	processor->bus.write(processor->target_address, 0b00101010);

	processor->ROL();

	ASSERT_EQ(processor->flags, 0b00000000);

	processor->bus.write(processor->target_address, 0b01000000);

	processor->ROL();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 0b10000000);

	processor->ROL();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, RorOnAcc) {

	processor->acc_memory_switch = true;

	processor->set_flag(CPU6502::Flags::Carry, true);

	processor->acc = 0b00101010;

	processor->ROR();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->acc = 0b00000001;

	processor->ROR();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, RorOnMemory) {

	processor->set_flag(CPU6502::Flags::Carry, true);

	processor->bus.write(processor->target_address, 0b00101010);

	processor->ROR();

	ASSERT_EQ(processor->flags, 0b10000000);

	processor->bus.write(processor->target_address, 0b00000001);

	processor->ROR();

	ASSERT_EQ(processor->flags, 0b00000011);
}


TEST_F(CPU6502Test, Rti) {

	processor->flags = 0b10000001;
	processor->pc = 0x678;

	processor->BRK();
	
	processor->flags = 0;
	processor->pc = 0;

	processor->RTI();

	processor->set_flag(CPU6502::Flags::Break, false);

	ASSERT_EQ(processor->flags, 0b10000001);
	ASSERT_EQ(processor->pc, 0x678);
}


TEST_F(CPU6502Test, Rts) {

	processor->pc = 0x678;

	processor->JSR();

	processor->RTS();

	ASSERT_EQ(processor->pc, 0x67A);
}