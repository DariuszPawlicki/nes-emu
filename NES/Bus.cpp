#include "Bus.hpp"




uint8_t Bus::read(uint16_t address) { return this->memory[address]; }

void Bus::write(uint16_t address, uint8_t data) { this->memory[address] = data; }