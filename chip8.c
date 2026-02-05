#include "chip8.h"

void init(Chip8* c) {
    srand(time(NULL));
    //ensure memory, register file, stack and screen are cleared
    memset(c->memory,   0, sizeof(c->memory));
    memset(c->V,        0, sizeof(c->V));
    memset(c->stack,    0, sizeof(c->stack));
    memset(c->display,  0, sizeof(c->display));
    memset(c->keypad,   0, sizeof(c->keypad));

    //clear special registers & timers
    c->SP = -1;
    c->PC = PROGRAM_START;
    c->I  = 0;
    c->delay_timer = 0;
    c->sound_timer = 0;
    c->shift_mode = false;
    c->debug_mode = false;

    for (size_t i = FONT_OFFSET; i < FONTSET_SIZE; i++) {
        c->memory[i] = FONTSET[i];
    }
}

void load_rom(Chip8* c, const char* filename) {
    FILE* fp;
    fp = fopen(filename, "rb");

    if (!fp) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fread(&c->memory[c->PC], 1, DISPLAY_SIZE, fp);
    fclose(fp);
}

void write_short(Chip8* c, uint16_t ins) {
    uint8_t  low = LOW(ins);
    uint8_t high = HIGH(ins);
    c->memory[c->PC++] = high;
    c->memory[c->PC++] = low;
}

void debug(Chip8* c, size_t mem_start, size_t mem_end, bool reg) {
    if (reg) {
        for (size_t i = 0; i < V_REG_FILE_SIZE; i++) {
            printf("V%zu = %u\n", i, c->V[i]);
        }
    }
        
    uint16_t save_PC = c->PC;
    for (size_t i = mem_start; i < mem_end; i++) {
        printf("memory location: %03x | instruction: %04x \n", c->PC, read_short(c));
        c->PC += 2;
    }
    c->PC = save_PC;

    printf("Current: %x", read_short(c));

    printf("\nStack Pointer: %i\n", c->SP);
    printf("Program Counter: %x\n", c->PC);
    printf("Index Register: %x\n", c->I);

    printf("\nDisplay Timer: %u\n", c->delay_timer);
    printf("Sound Timer: %u\n", c->sound_timer);
}

uint16_t read_short(Chip8* c) {
    uint16_t u16 = c->memory[c->PC] << 8 | c->memory[c->PC + 1];
    return u16;
}

uint16_t fetch_short(Chip8* c) {
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

void decode_short(Chip8* c, uint16_t curr_ins) {
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
                    memset(c->display, 0, sizeof(c->display));
                    break;
                }
                //0x00EE Return
                case 0x0EE: {
                    int16_t addr = stack_pop(c);
                    assert(addr >= 0 && "error: return instruction on empty\n");
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
            c->PC = NNN;
            break;
        }
        case 0x2: {
            stack_push(c, c->PC);
            c->PC = NNN;
            break;
        }
        case 0x3: {
            if (c->V[X] == NN) {
                c->PC += 2;
            }
            break;
        }
        case 0x4: {
            if (c->V[X] != NN) {
                c->PC += 2;
            }
            break;
        }
        case 0x5: {
            if (c->V[X] == c->V[Y]) {
                c->PC += 2;
            }
            break;
        }
        case 0x6: {
            c->V[X] = NN;
            break;
        }
        case 0x7: {
            c->V[X] += NN;
            break;
        }
        case 0x8: {
            switch (N) {
                case 0x0: {
                    c->V[X] = c->V[Y];
                    break;
                }
                case 0x1: {
                    c->V[X] = c->V[X] | c->V[Y];
                    break;
                }
                case 0x2: {
                    c->V[X] = c->V[X] & c->V[Y];
                    break;
                }
                case 0x3: {
                    c->V[X] = c->V[X] ^ c->V[Y];
                    break;
                }
                case 0x4: {
                    int sum = c->V[X] + c->V[Y];
                    c->V[X] = sum;
                    c->V[0xF]= (sum > 255) ? 1 : 0;
                    break;
                }
                case 0x5: {
                    int diff = c->V[X] - c->V[Y];
                    c->V[X] = diff;
                    c->V[0xF] = (c->V[X] > c->V[Y]) ? 1 : 0;
                    break;
                }
                case 0x6: {
                    if (c->shift_mode) {
                        c->V[X] = c->V[Y];
                    }
                    uint8_t lsb = c->V[X] & 1;
                    c->V[X] >>= 1;
                    c->V[0xF] = lsb;
                    break;
                }
                case 0x7: {
                    int diff = c->V[Y] - c->V[X];
                    c->V[X] = diff;
                    c->V[0xF] = (c->V[Y] > c->V[X]) ? 1 : 0;
                    break;
                }
                case 0xE: {
                    if (c->shift_mode) {
                        c->V[X] = c->V[Y];
                    }
                    uint8_t msb = (c->V[X] >> 7) & 1;
                    c->V[X] <<= 1;
                    c->V[0xF] = msb;
                    break;
                }

                default: {
                    break;
                }     
            }
            break;
        }
        case 0x9: {
            if (c->V[X] != c->V[Y]) {
                c->PC += 2;
            }
            break;
        }
        case 0xA: {
            c->I = NNN;
            break;
        }
        case 0xB: {
            c->PC = NNN + c->V[0];
            break;
        }
        case 0xC: {
            uint16_t r = rand() & NN;
            c->V[X] = r;
            break;
        }
        case 0xD: {
            //sprite is N pixels tall
            uint8_t sprite_height = 0x00F & NNN;
            uint8_t x_coord = c->V[X] % DISPLAY_WIDTH;
            uint8_t y_coord = c->V[Y] % DISPLAY_HEIGHT;

            c->V[0xF] = 0;

            for (size_t row = 0; row < sprite_height; row++) {
                //fetch one byte/row of sprite data at sprite index I from memory
                uint8_t sprite_byte = c->memory[c->I + row];
                for (size_t col = 0; col < 8; col++) {
                    //bitwise shift current sprite byte by col to isolate the current pixel at (col, row)
                    uint8_t sprite_pixel = sprite_byte & (0x80 >> col);

                    if (sprite_pixel) {
                        int px = (x_coord + col) % DISPLAY_WIDTH;
                        int py = (y_coord + row) % DISPLAY_HEIGHT;
                        int pixel_address = py * DISPLAY_WIDTH + px;
                        //if the pixel we are trying to draw is already on, set the F flag to 1
                        if (c->display[pixel_address]) {
                            c->V[0xF] = 1;
                        } 

                        c->display[pixel_address] ^= 1;
                    }
                }
            }

            break;
        }
        case 0xE: {
            switch (NN) {
                case 0x9E: {
                    if (c->keypad[c->V[X]]) {
                        c->PC += 2;
                    }
                    break;
                }
                case 0xA1: { 
                    if (!c->keypad[c->V[X]]) {
                        c->PC += 2;
                    }
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        case 0xF: {
            switch (NN) {
                case 0x07: {
                    c->V[X] = c->delay_timer;
                    break;
                }
                case 0x0A: {
                    for (size_t i = 0; i < KEYPAD_SIZE; i++) {
                       if (c->keypad[i]) {
                            c->V[X] = i;
                            break;
                       }
                    }
                    c->PC -= 2;
                    break;
                }
                case 0x15: {
                    c->delay_timer = c->V[X];
                    break;
                }
                case 0x18: {
                    c->sound_timer = c->V[X];
                    break;
                }
                case 0x1E: {
                    c->I += c->V[X];
                    break;
                }
                case 0x29: {
                    c->I = FONT_OFFSET + (c->V[X] & 0xF) * 5;
                    break;
                }
                case 0x33: {
                    uint8_t x = c->V[X];
                    assert(c->I+2<MEMORY_SIZE);
                    c->memory[c->I] = x / 100;
                    c->memory[c->I + 1] = (x / 10) % 10;
                    c->memory[c->I + 2] = x % 10;
                    break;
                }
                case 0x55: {
                    for (size_t i = 0; i <= X; i++) {
                        c->memory[c->I + i] = c->V[i];
                    }
                    break;
                }
                case 0x65: {
                    for (size_t i = 0; i <= X; i++) {
                        c->V[i] = c->memory[c->I + i];
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default: {
            break;
        }
    }
}

void emulate_cycle(Chip8* c) {
    uint16_t curr = fetch_short(c);
    decode_short(c, curr);
}

void stack_push(Chip8* c, uint16_t data) {
    assert(!is_stack_full(c) && "cannot push value to stack, stack is full\n");

    c->stack[++c->SP] = data;
}

uint16_t stack_pop(Chip8* c) {
    assert(!is_stack_empty(c) && "cannot pop from empty stack\n"); 

    int16_t res = stack_top(c);
    c->SP--;
    return res;
}

int16_t stack_top(Chip8* c) {
    return c->stack[c->SP];
}

bool is_stack_empty(Chip8* c) {
    return c->SP == -1;
}

bool is_stack_full(Chip8* c) {
    return c->SP >= STACK_SIZE - 1;
}
