#include "chip8.h"

void init(Chip8* c) {
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

    for (int i = 0; i < FONTSET_SIZE; i++) {
        c->memory[i] = FONTSET[i];
    }
}

void debug(Chip8* c) {
    for (uint8_t i = 0; i < V_REG_FILE_SIZE; i++) {
        printf("V%u = %u\n", i, c->V[i]);
    }

    printf("\nStack Pointer: %u\n", c->SP);
    printf("Program Counter: %u\n", c->PC);
    printf("Index Register: %u\n", c->I);

    printf("\nDisplay Timer: %u\n", c->DT);
    printf("Sound Timer: %u\n", c->ST);
}

uint16_t fetch(Chip8* c) {
    uint16_t res = c->memory[c->PC] << 8 | c->memory[c->PC + 1];
    c->PC += 2;
    return res;
}

