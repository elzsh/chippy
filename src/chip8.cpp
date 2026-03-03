#include <algorithm>
#include <chip8.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>

namespace {
constexpr std::array<uint8_t, 80> FONTSET = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,    // 0
    0x20, 0x60, 0x20, 0x20, 0x70,    // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,    // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,    // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,    // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,    // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,    // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,    // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,    // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,    // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,    // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,    // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,    // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,    // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,    // E
    0xF0, 0x80, 0xF0, 0x80, 0x80     // F
};
}

void Chip8::reset() {
    memory.fill(0);
    display.fill(0);
    keypad.fill(0);
    registers.fill(0);
    stack.fill(0);

    pc = START_ADDRESS;
    opcode = 0;
    index = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;

    std::copy(FONTSET.begin(), FONTSET.end(), memory.begin() + FONTSET_ADDRESS);
}

Chip8::Chip8() {
    reset();
}

bool Chip8::load_rom(const std::filesystem::path& filepath) {
    std::ifstream rom(filepath, std::ios::binary | std::ios::ate);

    if (!rom.is_open()) {
        return false;
    }

    std::streamsize size = rom.tellg();
    if (size <= 0) {
        return false;
    }

    const size_t max_allowed = MEMORY_SIZE - START_ADDRESS;
    if (static_cast<size_t>(size) > max_allowed) {
        return false;
    }

    reset();
    rom.seekg(0, std::ios::beg);

    if (!rom.read(reinterpret_cast<char*>(memory.data() + START_ADDRESS), size)) {
        return false;
    }

    return true;
}

void Chip8::tick() {
    opcode = memory[pc & 0x0FFF] << 8 | memory[(pc + 1) & 0x0FFF];

    pc += 0x2;
    pc &= 0x0FFF;

    uint8_t op = (opcode & 0xF000) >> 12;    // test
    uint8_t x = (opcode & 0x0F00) >> 8;      // test
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);
    uint8_t nn = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    switch (op) {
        case 0x0:
            switch (opcode) {
                case 0x00E0:
                    display.fill(0);
                    break;
            }
            break;
        case 0x1:
            pc = nnn;
            break;
        case 0x6:
            registers[x] = nn;
            break;
        case 0x7:
            registers[x] += nn;
            break;
        case 0xA:
            index = nnn;
            break;
        case 0xD: {
            break;
        }
    }
}
