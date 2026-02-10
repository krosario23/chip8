#include "chip8.h"

int main(int argc, char* argv[]) {
    Chip8 sys;
    init(&sys);
    load_rom(&sys, "../roms/fuse.ch8");

    printf("argc = %i\n", argc);

    if (argc > 1) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i],"-debug") == 0) {
                sys.debug_mode = true;
            }
            if (strcmp(argv[i],"-shift") == 0) {
                sys.shift_mode = true;
            }
        }
    }
    
    SDL_Window *window = NULL;
    SDL_Surface *win_surf = NULL;
    SDL_Surface *frame_buffer = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL_Init: %s", SDL_GetError());
        return 1;
    }
    
    window = SDL_CreateWindow("Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    win_surf = SDL_GetWindowSurface(window);

    if (!win_surf) {
        fprintf(stderr, "SDL_GetWindowSurface: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Color palette[2] = {
        {   0,   0,   0, 255 }, //black
        { 255, 255, 255, 255 }  //white
    };

    const uint64_t PERF_FREQ = SDL_GetPerformanceFrequency();
    uint64_t prev = SDL_GetPerformanceCounter();

    const double cpu_period = (double)PERF_FREQ/(double)CPU_HZ;
    const double timer_period = (double)PERF_FREQ/(double)TIMER_HZ;
    const double render_period = (double)PERF_FREQ/(double)RENDER_HZ;

    double cpu_acc = 0.0, timer_acc = 0.0, render_acc = 0.0;
    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE: {
                    running = 0;
                    break;
                }
                default:
                    break;
                }

                for (int i = 0; i < KEYPAD_SIZE; i++) {
                    if (e.key.keysym.sym == KEYMAP[i]) {
                        sys.keypad[i] = true;
                    }
                }
            }
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < KEYPAD_SIZE; i++) {
                    if (e.key.keysym.sym == KEYMAP[i]) {
                        sys.keypad[i] = false;
                    }
                }
            } 
        }
        uint64_t curr = SDL_GetPerformanceCounter();
        uint64_t elapsed = curr - prev;
        prev = curr;

        cpu_acc += (double)elapsed;
        timer_acc += (double)elapsed;
        render_acc += (double)elapsed;

        while (cpu_acc >= cpu_period) {
            emulate_cycle(&sys);
            if (sys.debug_mode) {
                debug(&sys,0,0,0);
                int c = getc(stdin);
                if ((char)c == 'q') exit(0);
            }   
            cpu_acc -= cpu_period;
        }

        while (timer_acc >= timer_period) {
            if (sys.delay_timer > 0) sys.delay_timer--;
            if (sys.sound_timer > 0) sys.sound_timer--;
            timer_acc -= timer_period;
        }

        if (render_acc >= render_period) {
            frame_buffer = SDL_CreateRGBSurfaceWithFormatFrom(sys.display, DISPLAY_WIDTH, DISPLAY_HEIGHT, 8, DISPLAY_WIDTH, SDL_PIXELFORMAT_INDEX8);

            if (!frame_buffer) {
                fprintf(stderr, "CreateRGBSurfaceFrom: %s\n", SDL_GetError());
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            if (!frame_buffer->format->palette) {
                fprintf(stderr, "framebuffer error: No palette detected on surface\n");
            } else {        
                if(SDL_SetPaletteColors(frame_buffer->format->palette, palette, 0, 2) !=0) {
                    fprintf(stderr, "SDL_SetPaletteColors: %s\n", SDL_GetError());
                }
            }

            SDL_Surface *conv = SDL_ConvertSurface(frame_buffer, win_surf->format, 0);
            if (!conv) {
                fprintf(stderr, "SDL_ConvertSurface failed: %s\n", SDL_GetError());
                return 1;
            }


            SDL_Rect dest = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

            if (SDL_BlitScaled(conv, NULL, win_surf, &dest) < 0) {
                fprintf(stderr, "SDL_BlitScaled failed: %s\n", SDL_GetError());
            }

            SDL_UpdateWindowSurface(window);
            SDL_FreeSurface(conv);
            render_acc = 0.0;
        }
        SDL_Delay(16);
    }

    SDL_FreeSurface(frame_buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}