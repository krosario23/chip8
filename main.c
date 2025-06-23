#include "chip8.h"

int main() {
    Chip8 system;
    init(&system);
    debug(&system, 0, 5);

    return 0;
}