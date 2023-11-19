#include "Cartridge.hpp"

#include <array>


Cartridge::Cartridge(const std::string& rom_path) {
    std::ifstream rom;
    rom.open(rom_path, std::ios::binary);

    if (rom.is_open()) {
        rom_opened_correctly = true;

        std::array<uint8_t, 16> header_data;

        rom.seekg(0);
        rom.read(reinterpret_cast<char*>(header_data.data()), 16);

        for (int i = 0; i < 4; i++) {
            header.nes_const[i] = header_data[i];
        }

        header.prg_rom_size = header_data[4];
        header.chr_rom_size = header_data[5];
        header.mapper_info1 = header_data[6];
        header.mapper_info2 = header_data[7];
        header.prg_ram_size = header_data[8];
        header.tv_info1 = header_data[9];
        header.tv_info2 = header_data[10];

        uint8_t lower_nibble = (header.mapper_info1 & 0b11110000) >> 4;
        uint8_t higher_nibble = (header.mapper_info1 & 0b00001111) << 4;

        mapper_id = higher_nibble + lower_nibble;

        for (int i = 11; i < 16; i++) {
            header.padding[i - 11] = header_data[i];
        }

        if (header.mapper_info1 & 0x04) {
            rom.seekg(512, std::ios_base::cur); // Skipping trainer part if exists
        }

        prg_rom.resize(16384 * header.prg_rom_size);
        chr_rom.resize(8192 * header.chr_rom_size);

        rom.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
        rom.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

        rom.close();

        if (header.prg_rom_size == 1) { // If PRG ROM block count is 1 then this block is duplicated
            prg_rom.insert(prg_rom.end(), prg_rom.begin(), prg_rom.end());
        }
    }
}

uint8_t Cartridge::cpuRead(uint16_t address) {
    /*
       If PRG ROM blocks count is 1, then
       PRG ROM block is duplicated, so
       vector takes 32kB of space - 0x0000 -> 0x7FFF
       Addresses given as arguments are in range - 0x4020 -> 0xFFFF
       so there is a need to map them into vector addressable space
    */
    if (header.prg_rom_size == 1) {
        address &= 0x7FFF;
    }

    return prg_rom[address];
}

void Cartridge::cpuWrite(uint16_t address, uint8_t data) {
}

uint8_t Cartridge::ppuRead(uint16_t address) {
    return {};
}

void Cartridge::ppuWrite(uint16_t address, uint8_t data) {
}
