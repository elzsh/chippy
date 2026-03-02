#pragma once
#include <cstdint>

class Chip8 {
  public:
    Chip8();
    void load_rom(const char* filename);

    uint32_t display[64 * 32]{};

    uint8_t keypad[16]{};

  private:
    uint8_t memory[4096]{};
    uint8_t registers[16]{};
    uint16_t index{};
    uint16_t pc{};

    uint16_t stack[16]{};
    uint8_t sp{};

    uint8_t delay_timer{};
    uint8_t sound_timer{};

    uint16_t opcode{};
};
