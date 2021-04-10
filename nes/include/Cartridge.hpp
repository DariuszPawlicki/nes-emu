#pragma once


#include <string>
#include <fstream>
#include <cstdint>


class Cartridge
{
	struct Header {
		uint8_t nes_const[4];
		uint8_t prg_rom_size;
		uint8_t chr_rom_size;
		uint8_t flags6;
		uint8_t flags7;
		uint8_t flags8;
		uint8_t flags9;
		uint8_t flags10;
		uint8_t padding[5];
	};

	public:		
		Header header; // 16 bytes
		uint8_t mapper_id;
		std::ifstream* rom = nullptr;

		bool load_rom(std::string rom_path);				
		void extract_header_info();
};