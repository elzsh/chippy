#pragma once
#include <cstdint>

class Chip8 {
public:
    Chip8();

private:
    uint8_t memory[4096]{};
};
