#include <chip8.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

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
    auto delay = std::chrono::microseconds(1400);

    /**
      while (true) {
      chip8.tick();

      std::this_thread::sleep_for(delay);
      }
     */

    chip8.tick();
    chip8.tick();
    chip8.tick();
    chip8.tick();
    chip8.tick();
    chip8.tick();
    chip8.tick();
    chip8.tick();

    return 0;
}
