#include "chip8.h"

void chip_init(Chip8* c) {
    //ensure memory, register file, stack and screen are cleared
    memset(c->memory, 0, sizeof(c->memory));
    memset(c->V, 0, sizeof(c->V));
    memset(c->stack, 0, sizeof(c->stack));
    memset(c->screen, 0, sizeof(c->screen));

    //clear special registers & timers
    c->SP = 0;
    c->PC = 0x200;
    c->I = 0;
    c->DT = 0;
    c->DT = 0;
}

void chip_debug(Chip8* c) {
    for (uint8_t i = 0; i < V_REG_FILE_SIZE; i++) {
        printf("V%u = %u\n", i, c->V[i]);
    }

    printf("\nStack Pointer: %u\n", c->SP);
    printf("Program Counter: %u\n", c->PC);
    printf("Index Register: %u\n", c->I);

    printf("\nDisplay Timer: %u\n", c->DT);
    printf("Sound Timer: %u\n", c->ST);
}

uint16_t chip_fetch(Chip8* c) {
    return c->memory[c->PC];
    c->PC += 2;
}