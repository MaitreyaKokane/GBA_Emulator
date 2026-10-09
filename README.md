# GBA Emulator

A Game Boy Advance emulator built with C++, SDL3, and ImGui.

## Current Status

**Work in Progress** - This project is under active development.

### Completed
- ✅ SDL3 display setup with OpenGL context
- ✅ ImGui UI framework integration
- ✅ Window management (windowed, fullscreen, borderless fullscreen)
- ✅ Menu bar with File and Display options
- ✅ Basic event handling
- ✅ DPI scaling support
- ✅ Project structure and build configuration

### To Implement
- Memory system (BIOS, EWRAM, IWRAM, I/O, Palette, VRAM, OAM)
- ARM7TDMI CPU (ARM and Thumb instruction sets)
- Cartridge loading and ROM parsing
- PPU (backgrounds, sprites, scanline rendering)
- Input handling and button mapping
- APU (audio channels and output)
- Save state functionality

## Project Structure

```
GBA_Emulator/
├── CMakeLists.txt          # CMake configuration with SDL3, ImGui, and OpenGL
├── BUILD.md               # Build instructions
├── .gitignore
├── LICENSE
├── README.md
├── src/                   # Source files
│   ├── main.cpp          # SDL3 + ImGui entry point with display setup
│   ├── cpu/
│   │   └── arm7tdmi.cpp  # ARM7TDMI CPU implementation (to be implemented)
│   ├── memory/
│   │   └── memory.cpp     # Memory system implementation (to be implemented)
│   ├── ppu/
│   │   └── ppu.cpp        # Pixel Processing Unit implementation (to be implemented)
│   ├── apu/
│   │   └── apu.cpp        # Audio Processing Unit implementation (to be implemented)
│   ├── input/
│   │   └── input.cpp      # Input handling implementation (to be implemented)
│   ├── cartridge/
│   │   └── cartridge.cpp  # ROM cartridge loading implementation (to be implemented)
│   └── emulator.cpp       # Main emulator integration (to be implemented)
├── include/              # Header files
│   ├── cpu/
│   │   └── arm7tdmi.h
│   ├── memory/
│   │   └── memory.h
│   ├── ppu/
│   │   └── ppu.h
│   ├── apu/
│   │   └── apu.h
│   ├── input/
│   │   └── input.h
│   ├── cartridge/
│   │   └── cartridge.h
│   └── emulator.h
├── external/             # External dependencies
│   ├── SDL/              # SDL3 library
│   └── imgui/            # ImGui library
└── tests/                # For unit tests
```

## Building

See [BUILD.md](BUILD.md) for detailed build instructions.

### Quick Start

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable will be located at `build/bin/Release/GBA_Emulator.exe` (Windows) or `build/bin/GBA_Emulator` (Linux/macOS).

## Dependencies

- **SDL3** - Included in `external/SDL/` (from https://github.com/libsdl-org/SDL)
- **ImGui** - Included in `external/imgui/` (from https://github.com/ocornut/imgui, docking branch)
- **OpenGL** - Usually comes with OS or graphics drivers

## Features

- Cross-platform (Windows, Linux, macOS)
- Modern C++20 codebase
- ImGui-based user interface
- Multiple fullscreen modes (windowed, exclusive, borderless)
- High-DPI display support
- F11 toggle for fullscreen

## Controls

- **F11**: Toggle fullscreen mode
- **Ctrl+O**: Open ROM (to be implemented)
- **Ctrl+S**: Save state (to be implemented)

## License

See [LICENSE](LICENSE) file for details.

## Contributing

This project is currently in early development. Implementation of the core emulator components (CPU, Memory, PPU, etc.) is in progress.

## Acknowledgments

- [SDL3](https://github.com/libsdl-org/SDL) - Simple DirectMedia Layer
- [ImGui](https://github.com/ocornut/imgui) - Immediate Mode GUI library