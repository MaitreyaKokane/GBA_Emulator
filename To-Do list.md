# GBA Emulator Implementation To-Do List

## Completed
- [x] SDL3 display setup
- [x] ImGui UI framework
- [x] Window management (fullscreen/windowed/borderless)
- [x] OpenGL context setup
- [x] Menu bar (File, Display)
- [x] Basic event handling

## To Implement

### 1. Memory System (Foundation)
- [ ] Implement GBA memory map
  - [ ] BIOS (0x00000000 - 0x00003FFF)
  - [ ] EWRAM (0x02000000 - 0x0203FFFF)
  - [ ] IWRAM (0x03000000 - 0x03007FFF)
  - [ ] I/O Registers (0x04000000 - 0x040003FF)
  - [ ] Palette RAM (0x05000000 - 0x050003FF)
  - [ ] VRAM (0x06000000 - 0x06017FFF)
  - [ ] OAM (0x07000000 - 0x070003FF)
- [ ] Add read/write functions with proper address mapping
- [ ] Handle memory regions and their access patterns
- [ ] Implement wait states for different memory regions

### 2. ARM7TDMI CPU (Core)
- [ ] Implement registers (R0-R15, CPSR, SPSR)
- [ ] ARM instruction set decoding and execution
  - [ ] Data processing instructions
  - [ ] Memory access instructions (LDR/STR)
  - [ ] Branch instructions
  - [ ] Multiply instructions
- [ ] Thumb instruction set decoding and execution
  - [ ] Thumb to ARM switching
  - [ ] Thumb instruction decoder
- [ ] Pipeline and cycle counting
- [ ] Exception handling
  - [ ] IRQ
  - [ ] FIQ
  - [ ] SWI
  - [ ] Undefined instruction

### 3. Cartridge Loading (Input)
- [ ] Load .gba ROM files
- [ ] Parse ROM header
  - [ ] Title (0xA0-0xAB)
  - [ ] Game code (0xB2-0xB5)
  - [ ] Maker code (0xB6-0xB7)
- [ ] Handle different ROM sizes
- [ ] Implement save game types
  - [ ] SRAM
  - [ ] Flash memory
  - [ ] EEPROM
- [ ] Add file dialog in ImGui for ROM selection

### 4. PPU (Display)
- [ ] Implement background rendering
  - [ ] Mode 0 (4 bg layers)
  - [ ] Mode 1 (3 bg layers + sprites)
  - [ ] Mode 2 (2 bg layers, affine)
  - [ ] Mode 3 (fullscreen bitmap)
  - [ ] Mode 4 (fullscreen bitmap, double buffering)
  - [ ] Mode 5 (smaller bitmap, double buffering)
- [ ] Sprite rendering
  - [ ] Regular sprites
  - [ ] Affine sprites
- [ ] Color palettes
  - [ ] Background palettes
  - [ ] Sprite palettes
- [ ] Scanline rendering
- [ ] Blend modes
  - [ ] Alpha blending
  - [ ] Brightness effects
- [ ] Render GBA framebuffer to ImGui texture

### 5. Input (Interaction)
- [ ] Map keyboard keys to GBA buttons
  - [ ] Z: A Button
  - [ ] X: B Button
  - [ ] Enter: Start
  - [ ] Shift: Select
  - [ ] Arrow Keys: D-Pad
  - [ ] A: L Button
  - [ ] S: R Button
- [ ] Handle controller input
- [ ] Pass input state to the emulator
- [ ] Add input display in ImGui UI

### 6. APU (Audio) - Optional
- [ ] Sound channels
  - [ ] Square wave channels 1 & 2
  - [ ] Wave channel 3
  - [ ] Noise channel 4
- [ ] Audio output via SDL
- [ ] Volume control
- [ ] Sound enable/disable

### 7. Integration & UI
- [ ] Create main emulator class integrating all components
- [ ] Add ROM loader UI
- [ ] Add emulator controls (Play, Pause, Reset)
- [ ] Add save state UI
- [ ] Add display options (scaling, filters)
- [ ] Add debugger UI (optional)
  - [ ] Register view
  - [ ] Memory view
  - [ ] Disassembly view

## Testing Checklist
- [ ] Test with homebrew ROMs
- [ ] Test with commercial ROMs
- [ ] Verify timing accuracy
- [ ] Test save/load functionality
- [ ] Test audio output
