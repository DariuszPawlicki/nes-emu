#include "Bus.hpp"


Bus::Bus(){ this->memory = new uint8_t[ 64 * 1024 ]{ 0 }; }

Bus::~Bus() { delete[] this->memory; }

uint8_t Bus::read(uint16_t address) { return this->memory[address]; }

void Bus::write(uint16_t address, uint8_t data) { this->memory[address] = data; }