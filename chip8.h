#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>

#define LOW(x) ((x) & 0xFF)
#define HIGH(x) ((x) >> 8)

#define EXTRACT_W(instruction) ((instruction & 0xF000) >> 12)
#define EXTRACT_X(instruction) ((instruction & 0x0F00) >> 8)
#define EXTRACT_Y(instruction) ((instruction & 0x00F0) >> 4)
#define EXTRACT_N(instruction) ((instruction & 0x000F))
#define EXTRACT_NN(instruction) ((instruction & 0x0FF0) >> 4)
#define EXTRACT_NNN(instruction) ((instruction & 0x0FFF))

#define MEMORY_SIZE 4096
#define V_REG_FILE_SIZE 16
#define STACK_SIZE 16
#define KEYBOARD_SIZE 16

#define FONTSET_SIZE 80
#define PROGRAM_START 0x200
#define TIMER_MAX 255

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

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
    uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    uint8_t keyboard[KEYBOARD_SIZE];

    int8_t SP;
    uint16_t PC;
    uint16_t I;

    uint8_t DT;
    uint8_t ST;
} Chip8;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint32_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
} Chip8_SDL;

void init(Chip8* c);
void debug(Chip8* c);
void write(Chip8* c, uint16_t ins);
uint16_t fetch(Chip8* c);
void decode(Chip8* c);
void stack_push(Chip8* c, uint16_t data);
void stack_pop(Chip8* c);
int16_t stack_top(Chip8* c);
bool is_stack_empty(Chip8* c);
bool is_stack_full(Chip8* c);