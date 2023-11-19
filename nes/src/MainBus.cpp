#include "MainBus.hpp"


void MainBus::connectWithCPU() {
    cpu.connectToBus(shared_from_this());
}

void MainBus::powerUp() { cpu.powerUp(); }

uint8_t MainBus::read(uint16_t address) const {
    uint8_t data;

    if (address >= 0x0000 && address <= 0x1FFF) {
        // CPU ram
        data = cpu_ram[address & 0x07FF]; // Memory mirroring
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        // PPU registers
        data = ppu.cpuRead(address & 0x2007);
    }
    else if(address == 0x4014) {
        data = ppu.cpuRead(address);
    }
    else if (address >= 0x4000 && address <= 0x4017) {
        // APU range
        data = 0; // Placeholder for debugging, waiting for future implementation
    }
    else if (address >= 0x4018 && address <= 0x401F) {
        // APU and I/O functionality that is normally disabled.
        data = 0; // Placeholder for debugging, waiting for future implementation
    }
    else {
        // Cartridge space
        data = cartridge->cpuRead(address);
    }

    return data;
}

void MainBus::write(uint16_t address, uint8_t data) {
    // TODO
    if (address >= 0x0000 && address <= 0x1FFF) {
        cpu_ram[address & 0x07FF] = data;
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        // PPU registers
        ppu.cpuWrite(address & 0x2007, data);
    }
    else if (address == 0x4014) {
        ppu.cpuWrite(address, data);
    }
    else if (address >= 0x4000 && address <= 0x4017) {
        // APU range
    }
    else if (address >= 0x4018 && address <= 0x401F) {
        // APU and I/O functionality that is normally disabled.
    }
    else {
        // Cartridge space
        cartridge->cpuWrite(address, data);
    }
}

void MainBus::insertCartridge(std::shared_ptr<Cartridge> cartridge) {
    for (auto& item: cpu_ram) {
        item = 0;
    }

    this->ppu.connectToCartridge(cartridge);
    this->cartridge = std::move(cartridge);
}
