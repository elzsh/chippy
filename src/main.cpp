#include "chip8.h"

#include <chrono>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <string>
#include <thread>

void render_terminal(const Chip8& chip8) {
    static std::string buf;
    buf.clear();
    buf.reserve(chip8.DISPLAY_WIDTH * chip8.DISPLAY_HEIGHT * 3 + 64);

    buf.append("\033[2J\033[1;1H");

    for (size_t y = 0; y < chip8.DISPLAY_HEIGHT; y++) {
        for (size_t x = 0; x < chip8.DISPLAY_WIDTH; x++) {
            buf.append(chip8.display[y * chip8.DISPLAY_WIDTH + x] ? "\u2588" : " ");
        }
        buf.push_back('\n');
    }
    std::cout.write(buf.data(), static_cast<std::streamsize>(buf.size()));
}

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_rom>\n";
        return EXIT_FAILURE;
    }

    Chip8 chip8;

    if (!chip8.load_rom(argv[1])) {
        std::cerr << "Error: Failed to load ROM: " << argv[1] << '\n';
        return EXIT_FAILURE;
    }

    // ~700Hz (1.4 milliseconds per instruction)
    auto next_tick = std::chrono::steady_clock::now();
    constexpr auto tick_interval = std::chrono::microseconds(1400);

    uint64_t cycle_count = 0;

    while (true) {
        chip8.tick();

        if (chip8.draw_flag) {
            render_terminal(chip8);
            chip8.draw_flag = 0x00;
        }

        if (++cycle_count % 11 == 0) {
            chip8.tick_timers();
        }

        next_tick += tick_interval;
        std::this_thread::sleep_until(next_tick);
    }
}
