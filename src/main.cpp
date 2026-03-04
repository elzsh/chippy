#include <chip8.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

void render_terminal(const Chip8& chip8) {
    std::cout << "\033[2J\033[1;1H";

    for (size_t y = 0; y < chip8.DISPLAY_HEIGHT; y++) {
        for (size_t x = 0; x < chip8.DISPLAY_WIDTH; x++) {
            std::cout << (+chip8.display[y * chip8.DISPLAY_WIDTH + x] ? "█" : " ");
        }
        std::cout << '\n';
    }
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
    auto cpu_delay = std::chrono::microseconds(1400);

    int cycle_count = 0;

    while (true) {
        chip8.tick();

        if (++cycle_count % 11 == 0) {
            render_terminal(chip8);
        }

        std::this_thread::sleep_for(cpu_delay);
    }

    return 0;
}
