#include <algorithm>
#include <chip8.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>

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

Chip8::Chip8() : rand_gen(std::random_device{}()), rand_byte(0, 255) {
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

    pc += 0x02;
    pc &= 0x0FFF;

    uint8_t op = (opcode & 0xF000) >> 12;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);
    uint8_t nn = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    switch (op) {
        case 0x00:
            switch (opcode) {
                case 0x00E0:
                    display.fill(0);
                    break;
                case 0x00EE:
                    sp = (sp - 1) & 0x0F;
                    pc = stack[sp];
                    break;
            }
            break;
        case 0x01:
            pc = nnn;
            break;
        case 0x02:
            stack[sp] = pc;
            sp = (sp + 1) & 0x0F;
            pc = nnn;
            break;
        case 0x03:
            if (registers[x] == nn) {
                pc += 0x02;
            }
            break;
        case 0x04:
            if (registers[x] != nn) {
                pc += 0x02;
            }
            break;
        case 0x05:
            if (registers[x] == registers[y]) {
                pc += 0x02;
            }
            break;
        case 0x06:
            registers[x] = nn;
            break;
        case 0x07:
            registers[x] += nn;
            break;
        case 0x08:
            switch (n) {
                case 0x00:
                    registers[x] = registers[y];
                    break;
                case 0x01:
                    registers[x] = registers[x] | registers[y];
                    break;
                case 0x02:
                    registers[x] = registers[x] & registers[y];
                    break;
                case 0x03:
                    registers[x] = registers[x] ^ registers[y];
                    break;
                case 0x04: {
                    uint16_t sum = registers[x] + registers[y];
                    registers[x] = sum;
                    registers[0x0F] = (sum > 0xFF) ? 0x01 : 0x00;
                    break;
                }
                case 0x05: {
                    uint8_t flag = (registers[x] < registers[y]) ? 0x00 : 0x01;
                    uint8_t sub = registers[x] - registers[y];
                    registers[x] = sub;
                    registers[0x0F] = flag;
                    break;
                }
                case 0x06: {
                    uint8_t flag = registers[x] & 0x01;
                    registers[x] >>= 0x01;
                    registers[0x0F] = flag;
                    break;
                }
                case 0x07: {
                    uint8_t flag = (registers[y] < registers[x]) ? 0x00 : 0x01;
                    uint8_t sub = registers[y] - registers[x];
                    registers[x] = sub;
                    registers[0x0F] = flag;
                    break;
                }
                case 0x0E: {
                    uint8_t flag = (registers[x] & 0x80) >> 7;
                    registers[x] <<= 0x01;
                    registers[0x0F] = flag;
                    break;
                }
            }
            break;
        case 0x09:
            if (registers[x] != registers[y]) {
                pc += 0x02;
            }
            break;
        case 0x0A:
            index = nnn;
            break;
        case 0x0B:
            pc = nnn + registers[0x00];
            break;
        case 0x0C: {
            registers[x] = static_cast<uint8_t>(rand_byte(rand_gen)) & nn;
            break;
        }
        case 0x0D: {
            uint8_t x_coord = registers[x] & (DISPLAY_WIDTH - 1);
            uint8_t y_coord = registers[y] & (DISPLAY_HEIGHT - 1);

            registers[0xF] = 0x00;

            for (uint8_t i = 0; i < n; i++) {
                if (y_coord + i >= DISPLAY_HEIGHT) {
                    break;
                }

                uint8_t sprite_row = memory[(index + i) & 0x0FFF];

                for (uint8_t j = 0; j < 8; j++) {
                    if (x_coord + j >= DISPLAY_WIDTH) {
                        break;
                    }

                    uint8_t sprite_px = (sprite_row >> (7 - j)) & 1;

                    if (sprite_px) {
                        size_t display_idx = (y_coord + i) * DISPLAY_WIDTH + (x_coord + j);

                        registers[0xF] |= display[display_idx];

                        display[display_idx] ^= 1;
                    }
                }
            }
            break;
        }
        case 0x0E:
            switch (nn) {
                case 0x9E:
                    if (keypad[registers[x] & 0x0F]) {
                        pc += 0x02;
                    }
                    break;
                case 0xA1:
                    if (!keypad[registers[x] & 0x0F]) {
                        pc += 0x02;
                    }
                    break;
            }
            break;
        case 0x0F:
            switch (nn) {
                case 0x07:
                    registers[x] = delay_timer;
                    break;
                case 0x0A: {
                    uint8_t key_pressed = 0x00;
                    for (uint8_t i = 0; i < keypad.size(); i++) {
                        if (keypad[i]) {
                            registers[x] = i;
                            key_pressed = 0x01;
                        }
                    }
                    pc -= key_pressed ? 0x00 : 0x02;
                    break;
                }
                case 0x15:
                    delay_timer = registers[x];
                    break;
                case 0x18:
                    sound_timer = registers[x];
                    break;
                case 0x1E: {
                    uint16_t sum = index + registers[x];
                    index = sum;

                    // If overflows above addressable range set carry flag
                    registers[0x0F] = (sum > 0x0FFF) ? 0x01 : 0x00;
                    break;
                }
                case 0x29:
                    index = FONTSET_ADDRESS + ((registers[x] & 0x0F) * 0x05);
                    break;
                case 0x33: {
                    memory[index & 0x0FFF] = registers[x] / 100;
                    memory[(index + 1) & 0x0FFF] = (registers[x] / 10) % 10;
                    memory[(index + 2) & 0x0FFF] = registers[x] % 10;
                    break;
                }
                case 0x55:
                    for (uint8_t i = 0x00; i <= x; i++) {
                        memory[(index + i) & 0x0FFF] = registers[i];
                    }
                    break;
                case 0x65:
                    for (uint8_t i = 0x00; i <= x; i++) {
                        registers[i] = memory[(index + i) & 0x0FFF];
                    }
                    break;
            }
            break;
    }
}
