#include "chip8.h"

int main() {
    Chip8 system;
    chip_init(&system);
    //chip_debug(&system);

    for (int i = 0; i < 100; i++) {
        printf("%02x\n", chip_fetch(&system));
    }
    return 0;
}