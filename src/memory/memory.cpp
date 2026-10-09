#include "memory/memory.h"
#include "cartridge/cartridge.h"

Memory::Memory(Cartridge& cartridge)
    : cartridge_(cartridge)
{}

// ------------------------------------------------------------
// Internal memory address decoder
// ------------------------------------------------------------

namespace
{
    template <typename Storage>
    auto resolvePointer(Storage& memory, uint32_t address) noexcept
        -> decltype(memory.BIOS.data())
    {
        // BIOS: 16 KB
        if (address < 0x00004000u)
            return memory.BIOS.data() + address;

        // EWRAM: mirrored throughout 0x02000000-0x02FFFFFF
        if (address >= 0x02000000u &&
            address < 0x03000000u)
        {
            return memory.eRAM.data() + (address & 0x3FFFFu);
        }

        // IWRAM: mirrored throughout 0x03000000-0x03FFFFFF
        if (address >= 0x03000000u &&
            address < 0x04000000u)
        {
            return memory.iRAM.data() + (address & 0x7FFFu);
        }

        // I/O registers
        if (address >= 0x04000000u &&
            address < 0x04000400u)
        {
            return memory.io_Registers.data()
                + (address - 0x04000000u);
        }

        // Palette RAM
        if (address >= 0x05000000u &&
            address < 0x06000000u)
        {
            return memory.PaletteRAM.data()
                + (address & 0x3FFu);
        }

        // VRAM, including the hardware's 32 KB mirror.
        if (address >= 0x06000000u &&
            address < 0x07000000u)
        {
            uint32_t offset = address & 0x1FFFFu;

            if (offset >= 0x18000u)
                offset -= 0x8000u;

            return memory.VRAM.data() + offset;
        }

        // OAM
        if (address >= 0x07000000u &&
            address < 0x08000000u)
        {
            return memory.OAM.data() + (address & 0x3FFu);
        }

        // Cartridge memory is handled separately.
        return nullptr;
    }
}

uint8_t* Memory::getInternalPointer(uint32_t address)
{
    return resolvePointer(memory_, address);
}

const uint8_t* Memory::getInternalPointer(
    uint32_t address) const
{
    return resolvePointer(memory_, address);
}

// ------------------------------------------------------------
// Read operations
// ------------------------------------------------------------

uint8_t Memory::read8(uint32_t address) const
{
    // Cartridge ROM windows: 0x08000000-0x0DFFFFFF
    if (address >= 0x08000000u &&
        address < 0x0E000000u)
    {
        if (!cartridge_.isLoaded())
            return 0xFF;

        const uint32_t offset = address & 0x01FFFFFFu;
        return cartridge_.readROM8(offset);
    }

    // Cartridge save-memory region.
    if (address >= 0x0E000000u &&
        address < 0x10000000u)
    {
        return cartridge_.readSave8(address & 0xFFFFu);
    }

    const uint8_t* pointer = getInternalPointer(address);

    // Simplified behavior for unmapped addresses.
    return pointer ? *pointer : 0xFF;
}

uint16_t Memory::read16(uint32_t address) const
{
    const uint16_t low = read8(address);
    const uint16_t high = read8(address + 1u);

    return static_cast<uint16_t>(
        low | (high << 8));
}

uint32_t Memory::read32(uint32_t address) const
{
    const uint32_t low = read16(address);
    const uint32_t high = read16(address + 2u);

    return low | (high << 16);
}

// ------------------------------------------------------------
// Write operations
// ------------------------------------------------------------

void Memory::write8(uint32_t address, uint8_t value)
{
    // BIOS is read-only.
    if (address < 0x00004000u)
        return;

    // Cartridge ROM is read-only.
    if (address >= 0x08000000u &&
        address < 0x0E000000u)
    {
        return;
    }

    // Cartridge save memory.
    if (address >= 0x0E000000u &&
        address < 0x10000000u)
    {
        cartridge_.writeSave8(
            address & 0xFFFFu, value);
        return;
    }

    uint8_t* pointer = getInternalPointer(address);

    if (pointer)
        *pointer = value;
}

void Memory::write16(uint32_t address, uint16_t value)
{
    write8(address, static_cast<uint8_t>(value));
    write8(address + 1u, static_cast<uint8_t>(value >> 8));
}

void Memory::write32(uint32_t address, uint32_t value)
{
    write16(address, static_cast<uint16_t>(value));
    write16(address + 2u, static_cast<uint16_t>(value >> 16));
}