#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Cartridge
{
public:
    static constexpr std::size_t MaxROMSize = 32u * 1024u * 1024u;
    static constexpr std::size_t SaveMemorySize = 64u * 1024u;

    Cartridge();

    // Load a .gba ROM from disk.
    bool loadFromFile(const std::string& path);

    // Cartridge status and information.
    bool isLoaded() const noexcept;
    std::size_t getROMSize() const noexcept;

    const std::string& getTitle() const noexcept;
    const std::string& getGameCode() const noexcept;
    const std::string& getMakerCode() const noexcept;

    // ROM is read-only.
    uint8_t readROM8(uint32_t offset) const noexcept;

    // Basic byte-addressable save-memory interface.
    uint8_t readSave8(uint32_t offset) const noexcept;
    void writeSave8(uint32_t offset, uint8_t value) noexcept;

private:
    std::vector<uint8_t> rom_;
    std::vector<uint8_t> saveMemory_;

    std::string title_;
    std::string gameCode_;
    std::string makerCode_;
};