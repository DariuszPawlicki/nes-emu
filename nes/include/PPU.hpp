#pragma once

#include "Cartridge.hpp"
#include "utils/ChipsCommons.hpp"

#include <array>
#include <memory>
#include <unordered_map>


class PPU {
public:
	// Accessing PPU bus
	void ppuWrite(uint16_t address, uint8_t data);
	uint8_t ppuRead(uint16_t address) const;

	// Accessing memory mapped PPU registers, connected to CPU bus
	void cpuWrite(uint16_t address, uint8_t data);
	uint8_t cpuRead(uint16_t address) const;

	void connectToCartridge(std::shared_ptr<Cartridge> cartridge);

	std::shared_ptr<Cartridge> cartridge;

private:
	constexpr inline static uint16_t PPUCTRL_REGISTER_ADDRESS{ 0x2000 };
	constexpr inline static uint16_t PPUMASK_REGISTER_ADDRESS{ 0x2001 };
	constexpr inline static uint16_t PPUSTATUS_REGISTER_ADDRESS{ 0x2002 };
	constexpr inline static uint16_t OAMADDR_REGISTER_ADDRESS{ 0x2003 };
	constexpr inline static uint16_t OAMDATA_REGISTER_ADDRESS{ 0x2004 };
	constexpr inline static uint16_t PPUSCROLL_REGISTER_ADDRESS{ 0x2005 };
	constexpr inline static uint16_t PPUADDR_REGISTER_ADDRESS{ 0x2006 };
	constexpr inline static uint16_t PPUDATA_REGISTER_ADDRESS{ 0x2007 };
	constexpr inline static uint16_t OAMDMA_REGISTER_ADDRESS{ 0x4014 };

	std::unordered_map<uint16_t, std::pair<std::string, chips_commons::Register<8>>> ppu_registers{
		{PPUCTRL_REGISTER_ADDRESS, {"PPUCTRL", {}}},
		{PPUMASK_REGISTER_ADDRESS, {"PPUMASK", {}}},
		{PPUSTATUS_REGISTER_ADDRESS, {"PPUSTATUS", {}}},
		{OAMADDR_REGISTER_ADDRESS, {"OAMADDR", {}}},
		{OAMDATA_REGISTER_ADDRESS, {"OAMDATA", {}}},
		{PPUSCROLL_REGISTER_ADDRESS, {"PPUSCROLL", {}}},
		{PPUADDR_REGISTER_ADDRESS, {"PPUADDR", {}}},
		{PPUDATA_REGISTER_ADDRESS, {"PPUDATA", {}}},
		{OAMDMA_REGISTER_ADDRESS, {"OAMDMA", {}}}
	};

	/*
		Background - made of 8x8 pixels tiles, screen consists of 32x30 tiles.

		Nametable - layout of frame's background consits of 32x30 indexes,
		each of those indexes are referencing the entry in pattern table,
		it's changing frame-by-frame.

		Pattern Table - contains the shape of each background tile, it's residing in ROM,
		it cannot be changed.
		Each tile in pattern table is made of 16 bytes - 2 planes 8 bytes each.

		Palette - NES system palette contains 64 colors, a single frame
		has it's own palette which is subset of system palette.
		System palette is static, frame palette is dynamic.
		Frame palette contains 32 colors - 8 groups of 4 colors in each,
		palettes 0-3 are background palettes and palettes 4-7 are for the sprites.

		Attribute Table - tiles which make up a background are
		grouped into 4x4 tiles blocks.
		Blocks are divided in four regions of 2x2 tiles,
		each byte in attribute table describes four regions of
		specific block - byte is divided on four 2 bit values
		one for each block region, this values are specyfying
		which color palette goes to block region.

		Sprites - CPU sends them to PPU and PPU puts them into
		it's memory.
	*/

	std::array<uint8_t, 2 * 1024> name_table{};
	std::array<uint8_t, 32> palette{};
};