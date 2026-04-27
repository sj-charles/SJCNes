#include <stdio.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "bus.h"
#include "rom.h"
#include "ppu.h"

int main() {
    ROM rom;
    CPU cpu;
    PPU ppu;

    if (!rom_load(&rom, "Pac-Man.nes")) {
        printf("Failed to load ROM\n");
        return 1;
    }

    bus_connect_rom(&rom);
    ppu_init(&ppu);
    ppu_connect_chr(&ppu, rom.chr_rom);
    bus_connect_ppu(&ppu);
    cpu_init(&cpu);
    cpu.pc = read(0xFFFC) | (read(0xFFFD) << 8);

    // SDL2 setup
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
        "SJCNes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 480, 0);  
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, 256, 240);

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        int current_frame = ppu.frame;
        while (ppu.frame == current_frame) {
            cpu_step(&cpu);
            ppu_step(&ppu);
            ppu_step(&ppu);
            ppu_step(&ppu);
            if (ppu.nmi_pending) {
                ppu.nmi_pending = 0;
                cpu_nmi(&cpu);
            }
        }

        SDL_UpdateTexture(texture, NULL, ppu.framebuffer, 256 * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}