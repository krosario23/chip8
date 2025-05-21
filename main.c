#include "chip8.h"

int main() {
    Chip8 system;
    init(&system);
    //debug(&system);

    for (int i = 0; i < 100; i++) {
        printf("%04x\n", fetch(&system));
    }
    return 0;
}