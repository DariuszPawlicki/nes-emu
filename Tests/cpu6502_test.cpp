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

TEST_F(CPU6502Test, And) {

	processor->acc = 0b01111111;
	processor->AND();

	ASSERT_EQ(processor->acc, 0b01111111);
	ASSERT_EQ(processor->flags, 0b00000000);

	processor->flags = 0;
	processor->acc = 0b00;
	processor->AND();

	ASSERT_EQ(processor->acc, 0b00000000);
	ASSERT_EQ(processor->flags, 0b00000010);

	processor->flags = 0;
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

	processor->flags = 0;
	processor->acc = 0b01000000;
	processor->ASL();

	ASSERT_EQ(processor->acc, 0b10000000);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->flags = 0;
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

	processor->flags = 0;
	processor->bus.write(processor->target_address, 0b01000000);
	processor->ASL();

	ASSERT_EQ(processor->bus.read(processor->target_address), 0b10000000);
	ASSERT_EQ(processor->flags, 0b10000000);

	processor->flags = 0;
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

	
}