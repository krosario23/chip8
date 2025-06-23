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

    for (size_t i = 0; i < FONTSET_SIZE; i++) {
        c->memory[i] = FONTSET[i];
    }
}

void write(Chip8* c, uint16_t ins) {
    uint8_t low = LOW(ins);
    uint8_t high = HIGH(ins);
    c->memory[c->PC++] = high;
    c->memory[c->PC++] = low;
}

void debug(Chip8* c, size_t start, size_t end) {
    for (size_t i = 0; i < V_REG_FILE_SIZE; i++) {
        printf("V%zu = %u\n", i, c->V[i]);
    }

    printf("PC before run: %u\n", c->PC);
    uint16_t save_PC = c->PC;
    for (size_t i = start; i < end; i++) {
        printf("memory location: %03x | instruction: %04x \n", c->PC, read_short(c));
        c->PC += 2;
    }
    c->PC = save_PC;


    printf("\nStack Pointer: %i\n", c->SP);
    printf("Program Counter: %u\n", c->PC);
    printf("Index Register: %u\n", c->I);

    printf("\nDisplay Timer: %u\n", c->DT);
    printf("Sound Timer: %u\n", c->ST);
}

uint16_t read_short(Chip8* c) {
    uint16_t u16 = c->memory[c->PC] << 8 | c->memory[c->PC + 1];
    return u16;
}

uint16_t fetch(Chip8* c) {
    uint16_t res = read_short(c);
    c->PC += 2;
    return res;
}

// W: The first nibble. Tells us what kind of instruction we are looking at.
// X: The second nibble. Used to look up one of the 16 registers (VX) from V0 through VF.
// Y: The third nibble. Also used to look up one of the 16 registers (VY) from V0 through VF.
// N: The fourth nibble. A 4-bit number.
// NN: The second byte (third and fourth nibbles). An 8-bit immediate number.
// NNN: The second, third and fourth nibbles. A 12-bit immediate memory address.

void decode(Chip8* c, uint16_t curr_ins) {
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
                    //printf("clear screen instruction\n");
                    memset(c->screen, 0, sizeof(c->screen));
                    break;
                }

                //0x00EE Return
                case 0x0EE: {
                    //printf("return instruction\n");
                    int16_t addr = stack_pop(c);
                    if (!addr) {
                        printf("error: return instruction on empty\n");
                    }
                    c->PC = addr;
                    break;
                }
                default: {
                    break;
                }  
            }
            break;
        }
        case 0x1: {
            //printf("jump instruction\n");
            c->PC = NNN;
            break;
        }
        case 0x2: {
            //printf("call instruction\n");
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
   //printf("push: %04x\n", data);
    if (!is_stack_full(c)) {
        c->stack[++c->SP] = data;
        return;
    }

    printf("cannot push value %03x to stack, stack is full\n", data);
}

int16_t stack_pop(Chip8* c) {
    if (!is_stack_empty(c)) {
        int16_t res = stack_top(c);
        c->SP--;
        return res;
    }

    printf("cannot pop from empty stack\n");
    return -1;
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
