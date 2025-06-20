#include "chip8.h"


int main() {
    Chip8 system;
    init(&system);
    //debug(&system);
    int x = 0x1234;
    printf("%02x %02x\n", LOW(x), HIGH(x));
    
    return 0;
}