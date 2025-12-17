#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define LOW(x) ((x) & 0xFF)
#define HIGH(x) ((x) >> 8)

#define EXTRACT_W(instruction) ((instruction & 0xF000) >> 12)
#define EXTRACT_X(instruction) ((instruction & 0x0F00) >> 8)
#define EXTRACT_Y(instruction) ((instruction & 0x00F0) >> 4)
#define EXTRACT_N(instruction) ((instruction & 0x000F))
#define EXTRACT_NN(instruction) ((instruction & 0x00FF))
#define EXTRACT_NNN(instruction) ((instruction & 0x0FFF))

#define MEMORY_SIZE 4096
#define V_REG_FILE_SIZE 16
#define STACK_SIZE 16
#define KEYPAD_SIZE 16

#define FONTSET_SIZE 80
#define PROGRAM_START 0x200
#define TIMER_MAX 255

#define SCREEN_HEIGHT 320
#define SCREEN_WIDTH 640

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define DISPLAY_SIZE DISPLAY_WIDTH * DISPLAY_HEIGHT

#define CPU_HZ 500
#define TIMER_HZ 60
#define RENDER_HZ 60

const static uint8_t KEYMAP[KEYPAD_SIZE] = {
    SDLK_x, //0
    SDLK_1, //1
    SDLK_2, //2
    SDLK_3, //3

    SDLK_q, //4  
    SDLK_w, //5
    SDLK_e, //6
    SDLK_a, //7

    SDLK_s, //8
    SDLK_d, //9
    SDLK_z, //A
    SDLK_c, //B

    SDLK_4, //C
    SDLK_r, //D
    SDLK_f, //E
    SDLK_v, //F
};

const static uint8_t FONTSET[FONTSET_SIZE] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[V_REG_FILE_SIZE];
    uint16_t stack[STACK_SIZE];
    uint8_t display[DISPLAY_WIDTH *  DISPLAY_HEIGHT];
    bool keypad[KEYPAD_SIZE];

    int8_t SP;
    uint16_t PC;
    uint16_t I;

    uint8_t delay_timer;
    uint8_t sound_timer;

    bool shift_mode;
    bool debug_mode;
} Chip8;


void        init(Chip8* c);
void        load_rom(Chip8* c, const char* filename);
void        debug(Chip8* c, size_t mem_start, size_t mem_end, bool reg);

void        write_short(Chip8* c, uint16_t ins);
uint16_t    read_short(Chip8* c);
uint16_t    fetch_short(Chip8* c);
void        decode_short(Chip8* c, uint16_t curr_ins);
void        emulate_cycle(Chip8* c);

void        stack_push(Chip8* c, uint16_t data);
uint16_t    stack_pop(Chip8* c);
int16_t     stack_top(Chip8* c);
bool        is_stack_empty(Chip8* c);
bool        is_stack_full(Chip8* c);