#include "chip8.h"

void init(Chip8* c) {
    //ensure memory, register file, stack and screen are cleared
    memset(c->memory, 0, sizeof(c->memory));
    memset(c->V, 0, sizeof(c->V));
    memset(c->stack, 0, sizeof(c->stack));
    memset(c->screen, 0, sizeof(c->screen));

    //clear special registers & timers
    c->SP = -1;
    c->PC = 0x200;
    c->I = 0;
    c->DT = 0;
    c->ST = 0;

    for (int i = 0; i < FONTSET_SIZE; i++) {
        c->memory[i] = FONTSET[i];
    }
}

void write(Chip8* c, uint16_t ins) {
    
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

// W: The first nibble. Tells us what kind of instruction we are looking at.
// X: The second nibble. Used to look up one of the 16 registers (VX) from V0 through VF.
// Y: The third nibble. Also used to look up one of the 16 registers (VY) from V0 through VF.
// N: The fourth nibble. A 4-bit number.
// NN: The second byte (third and fourth nibbles). An 8-bit immediate number.
// NNN: The second, third and fourth nibbles. A 12-bit immediate memory address.

void decode(Chip8* c) {
    uint16_t curr_ins = fetch(c);
    uint8_t W = EXTRACT_W(curr_ins);
    uint8_t X = EXTRACT_X(curr_ins);
    uint8_t Y = EXTRACT_Y(curr_ins);
    uint8_t N = EXTRACT_N(curr_ins);
    uint16_t NN = EXTRACT_NN(curr_ins);
    uint16_t NNN = EXTRACT_NNN(curr_ins);

    switch (W) {
        case 0x0: {
            switch (NNN) {
                //0x00E0 Clear screen
                case 0x0E0: {
                    memset(c->screen, 0, sizeof(c->screen));
                    break;
                }

                //0x00EE Return
                case 0x0EE: {

                    break;
                }
                default: {
                    break;
                }  
            }
            break;
        }
        case 0x1: {
            c->PC = NNN;
            break;
        }
        case 0x2: {
            stack_push(c, c->PC);
            c->PC = NNN;
            break;
        }
        case 0x3: {
            break;
        }
        case 0x4: {
            break;
        }
        case 0x5: {
            break;
        }
        case 0x6: {
            break;
        }
        case 0x7: {
            break;
        }
        case 0x8: {
            break;
        }
        case 0x9: {
            break;
        }
        case 0xA: {
            break;
        }
        case 0xB: {
            break;
        }
        case 0xC: {
            break;
        }
        case 0xD: {
            break;
        }
        case 0xE: {
            break;
        }
        case 0xF: {
            break;
        }
        default: {
            break;
        }
    }
}

void stack_push(Chip8* c, uint16_t data) {
    if (!is_stack_full(c)) {
        c->stack[++c->SP] = data;
        return;
    }

    printf("cannot push value %03x to stack, stack is full\n", data);
}

void stack_pop(Chip8* c) {
    if (!is_stack_empty(c)) {
        c->SP--;
        return;
    }

    printf("cannot pop from empty stack\n");
}

int16_t stack_top(Chip8* c) {
    if (!is_stack_empty(c)) return c->stack[c->SP];
    
    printf("stack is empty\n");
    return -1;
}

bool is_stack_empty(Chip8* c) {
    return c->SP == -1;
}

bool is_stack_full(Chip8* c) {
    return c->SP >= STACK_SIZE - 1;
}
