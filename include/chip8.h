#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>

class Chip8 {
  public:
    static constexpr size_t MEMORY_SIZE = 4096;
    static constexpr size_t START_ADDRESS = 0x200;
    static constexpr size_t FONTSET_ADDRESS = 0x50;
    static constexpr size_t DISPLAY_WIDTH = 64;
    static constexpr size_t DISPLAY_HEIGHT = 32;

    Chip8();

    void reset();

    bool load_rom(const std::filesystem::path& filepath);

    std::array<uint8_t, 16> keypad{};

    std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> display{};

  private:
    uint16_t pc{START_ADDRESS};
    uint16_t opcode{};
    uint16_t index{};
    std::array<uint8_t, 16> registers{};
    std::array<uint8_t, MEMORY_SIZE> memory{};

    uint8_t sp{};
    std::array<uint16_t, 16> stack{};

    uint8_t delay_timer{};
    uint8_t sound_timer{};
};
