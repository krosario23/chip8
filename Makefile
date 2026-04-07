CC = gcc

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

CFLAGS = -Wall -Wextra -std=c11 $(SDL_CFLAGS)
TARGET = prog

all:
	$(CC) $(CFLAGS) main.c chip8.c -o $(TARGET) $(SDL_LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean