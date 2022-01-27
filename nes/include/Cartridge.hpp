#pragma once


#include <string>
#include <fstream>
#include <cstdint>
#include <vector>


class Cartridge
{
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
		Header header; // 16 bytes
		uint8_t mapper_id;
		bool rom_opened_correctly; // Flag for rendering window with error msg
	
	public:
		Cartridge(const std::string& rom_path);

		uint8_t cpu_read(uint16_t address);
		void cpu_write(uint16_t address, uint8_t data);

		uint8_t ppu_read(uint16_t address);
		void ppu_write(uint16_t address, uint8_t data);
	
	private:
		std::vector<uint8_t> prg_rom;
		std::vector<uint8_t> chr_rom;
};