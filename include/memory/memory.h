#pragma once

#include <array>
#include <cstdint>

class Cartridge;

// Physical memory storage.
// Cartridge ROM and save memory are handled by Cartridge.
struct GBAMemory
{
    std::array<uint8_t, 0x4000> BIOS{};        // 16 KB
    std::array<uint8_t, 0x40000> eRAM{};      // 256 KB
    std::array<uint8_t, 0x8000> iRAM{};       // 32 KB
    std::array<uint8_t, 0x400> io_Registers{}; // 1 KB

    std::array<uint8_t, 0x400> PaletteRAM{};  // 1 KB
    std::array<uint8_t, 0x18000> VRAM{};      // 96 KB
    std::array<uint8_t, 0x400> OAM{};         // 1 KB
};

class Memory
{
public:
    explicit Memory(Cartridge& cartridge);

    // CPU memory reads
    uint8_t  read8(uint32_t address) const;
    uint16_t read16(uint32_t address) const;
    uint32_t read32(uint32_t address) const;

    // CPU memory writes
    void write8(uint32_t address, uint8_t value);
    void write16(uint32_t address, uint16_t value);
    void write32(uint32_t address, uint32_t value);

private:
    GBAMemory memory_{};
    Cartridge& cartridge_;

    // Resolve an address to internal system memory.
    uint8_t* getInternalPointer(uint32_t address);
    const uint8_t* getInternalPointer(uint32_t address) const;
};