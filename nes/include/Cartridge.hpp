#pragma once

#include "Mapper.hpp"

#include <vector>
#include <string>
#include <fstream>

class Cartridge {
    struct Header {
        uint8_t nes_const[4];
        uint8_t prg_rom_size;
        uint8_t chr_rom_size;
        uint8_t mapper_info1;
        uint8_t mapper_info2;
        uint8_t prg_ram_size;
        uint8_t tv_info1;
        uint8_t tv_info2;
        uint8_t padding[5];
    };

public:
    explicit Cartridge(const std::string& rom_path);
    uint8_t cpuRead(uint16_t address);
    void cpuWrite(uint16_t address, uint8_t data);
    uint8_t ppuRead(uint16_t address);
    void ppuWrite(uint16_t address, uint8_t data);

    Mapper mapper;
    Header header; // 16 bytes
    uint8_t mapper_id;
    bool rom_opened_correctly{false}; // Flag for rendering window with error msg

private:
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
};