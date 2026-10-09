#include "cartridge/cartridge.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>

namespace
{
    constexpr std::size_t HeaderSize = 0xC0;

    // Extract a null-terminated or fixed-width ASCII field
    // from the GBA cartridge header.
    std::string ReadHeaderField(
        const std::vector<uint8_t>& rom,
        std::size_t offset,
        std::size_t length)
    {
        if (offset >= rom.size())
            return {};

        const std::size_t end =
            std::min(offset + length, rom.size());

        std::string result;
        result.reserve(end - offset);

        for (std::size_t i = offset; i < end; ++i)
        {
            const uint8_t character = rom[i];

            if (character == 0)
                break;

            result.push_back(static_cast<char>(character));
        }

        while (!result.empty() && result.back() == ' ')
            result.pop_back();

        return result;
    }
}

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------

Cartridge::Cartridge()
    : saveMemory_(SaveMemorySize, 0xFF)
{}

// ------------------------------------------------------------
// ROM loading
// ------------------------------------------------------------

bool Cartridge::loadFromFile(const std::string& path)
{
    // Open at the end so we can determine the file size.
    std::ifstream file(
        path,
        std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cerr << "Error: Cannot open ROM: "
            << path << '\n';
        return false;
    }

    const std::streampos endPosition = file.tellg();

    if (endPosition == std::streampos(-1))
    {
        std::cerr << "Error: Cannot determine ROM size.\n";
        return false;
    }

    const std::streamoff fileSize =
        static_cast<std::streamoff>(endPosition);

    // A GBA cartridge header occupies the first 192 bytes.
    if (fileSize < static_cast<std::streamoff>(HeaderSize) ||
        fileSize > static_cast<std::streamoff>(MaxROMSize))
    {
        std::cerr << "Error: ROM size is outside the "
            "supported range (192 bytes to 32 MiB).\n";
        return false;
    }

    // Load into temporary storage. Preserve the existing ROM
    // if opening or reading the new file fails.
    std::vector<uint8_t> newROM(
        static_cast<std::size_t>(fileSize));

    file.seekg(0, std::ios::beg);

    if (!file.read(
        reinterpret_cast<char*>(newROM.data()),
        static_cast<std::streamsize>(newROM.size())))
    {
        std::cerr << "Error: Failed to read ROM data.\n";
        return false;
    }

    // Commit the new ROM only after a successful read.
    rom_ = std::move(newROM);

    // GBA header metadata.
    // These fields are defined in the cartridge header.
    title_ = ReadHeaderField(rom_, 0xA0, 12);
    gameCode_ = ReadHeaderField(rom_, 0xAC, 4);
    makerCode_ = ReadHeaderField(rom_, 0xB0, 2);

    // The fixed header byte should normally be 0x96.
    if (rom_[0xB2] != 0x96)
    {
        std::cerr << "Warning: Unexpected GBA header "
            "fixed value.\n";
    }

    // Basic temporary save-memory model.
    // Persistent per-game save files are not implemented yet.
    std::fill(
        saveMemory_.begin(),
        saveMemory_.end(),
        static_cast<uint8_t>(0xFF));

    std::cout << "ROM loaded successfully.\n"
        << "Title: " << title_ << '\n'
        << "Game code: " << gameCode_ << '\n'
        << "Maker code: " << makerCode_ << '\n'
        << "ROM size: " << rom_.size()
        << " bytes\n";

    return true;
}

// ------------------------------------------------------------
// Cartridge information
// ------------------------------------------------------------

bool Cartridge::isLoaded() const noexcept
{
    return !rom_.empty();
}

std::size_t Cartridge::getROMSize() const noexcept
{
    return rom_.size();
}

const std::string& Cartridge::getTitle() const noexcept
{
    return title_;
}

const std::string& Cartridge::getGameCode() const noexcept
{
    return gameCode_;
}

const std::string& Cartridge::getMakerCode() const noexcept
{
    return makerCode_;
}

// ------------------------------------------------------------
// ROM reads
// ------------------------------------------------------------

uint8_t Cartridge::readROM8(uint32_t offset) const noexcept
{
    if (rom_.empty())
        return 0xFF;

    // Basic mirroring approximation for smaller ROMs.
    // Exact behavior depends on cartridge size and bus mapping.
    const std::size_t index =
        static_cast<std::size_t>(offset) % rom_.size();

    return rom_[index];
}

// ------------------------------------------------------------
// Save-memory reads and writes
// ------------------------------------------------------------

uint8_t Cartridge::readSave8(uint32_t offset) const noexcept
{
    if (saveMemory_.empty())
        return 0xFF;

    const std::size_t index =
        static_cast<std::size_t>(offset) % saveMemory_.size();

    return saveMemory_[index];
}

void Cartridge::writeSave8(
    uint32_t offset,
    uint8_t value) noexcept
{
    if (saveMemory_.empty())
        return;

    const std::size_t index =
        static_cast<std::size_t>(offset) % saveMemory_.size();

    saveMemory_[index] = value;
}