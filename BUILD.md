# Build Instructions

## Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (MSVC, GCC, or Clang)
- OpenGL (usually comes with OS or graphics drivers)

## Building

The project uses FetchContent to automatically download and configure SDL3 and ImGui. No manual dependency installation required.

### Windows

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux

```bash
# Install OpenGL (if not already present)
sudo apt install libgl1-mesa-dev

mkdir build
cd build
cmake ..
cmake --build .
```

### macOS

```bash
# OpenGL is included with macOS

mkdir build
cd build
cmake ..
cmake --build .
```

## Running

After building, the executable will be in the `build/bin` directory.

```bash
# Windows
./build/bin/Release/GBA_Emulator.exe

# Linux/macOS
./build/bin/GBA_Emulator
```

## Project Structure

- `src/` - Add your GBA emulator source files here
- `include/` - Add your header files here
- `external/` - For any additional external dependencies
- `tests/` - For unit tests

The CMakeLists.txt is configured to automatically find all `.cpp` files in `src/` and all `.h` files in `include/` using `GLOB_RECURSE`.
