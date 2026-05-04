#include <stdio.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "bus.h"
#include "rom.h"
#include "ppu.h"

int main()
{
    ROM rom;
    CPU cpu;
    PPU ppu;

    if (!rom_load(&rom, "Donkey Kong.nes"))
    {
        printf("Failed to load ROM\n");
        return 1;
    }

    bus_connect_rom(&rom);
    ppu_init(&ppu);
    ppu_connect_chr(&ppu, rom.chr_rom);
    ppu.mirroring = rom.mirroring;
    bus_connect_ppu(&ppu);
    cpu_init(&cpu);
    cpu.pc = read(0xFFFC) | (read(0xFFFD) << 8);

    fflush(stdout);
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
    uint8_t controller_state = 0;
    int frame = 0;

    while (running)
    {
        // in main loop, print first 20 unique PC values
        static int pc_count = 0;
        static uint16_t seen_pcs[100] = {0};
        if (pc_count < 100)
        {
            int found = 0;
            for (int i = 0; i < pc_count; i++)
                if (seen_pcs[i] == cpu.pc)
                {
                    found = 1;
                    break;
                }
            if (!found)
            {
                seen_pcs[pc_count++] = cpu.pc;
                // printf("PC[%d]: %04X opcode=%02X\n", pc_count, cpu.pc, read(cpu.pc));
            }
        }

        cpu_step(&cpu);
        ppu_step(&ppu);
        ppu_step(&ppu);
        ppu_step(&ppu);

        if (ppu.nmi_pending)
        {
            ppu.nmi_pending = 0;
            bus_set_controller(0, controller_state);
            bus_latch_controllers();
            cpu_nmi(&cpu);
        }

        if (ppu.frame != frame)
        {
            frame = ppu.frame;

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = 0;
                if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_Z:
                        controller_state |= 0x01;
                        break; // A
                    case SDL_SCANCODE_X:
                        controller_state |= 0x02;
                        break; // B
                    case SDL_SCANCODE_RSHIFT:
                        controller_state |= 0x04;
                        break; // Select
                    case SDL_SCANCODE_RETURN:
                        controller_state |= 0x08;
                        break; // Start
                    case SDL_SCANCODE_UP:
                        controller_state |= 0x10;
                        break; // Up
                    case SDL_SCANCODE_DOWN:
                        controller_state |= 0x20;
                        break; // Down
                    case SDL_SCANCODE_LEFT:
                        controller_state |= 0x40;
                        break; // Left
                    case SDL_SCANCODE_RIGHT:
                        controller_state |= 0x80;
                        break; // Right
                    default:
                        break;
                    }
                }
                if (event.type == SDL_KEYUP)
                {
                    switch (event.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_Z:
                        controller_state &= ~0x01;
                        break;
                    case SDL_SCANCODE_X:
                        controller_state &= ~0x02;
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        controller_state &= ~0x04;
                        break;
                    case SDL_SCANCODE_RETURN:
                        controller_state &= ~0x08;
                        break;
                    case SDL_SCANCODE_UP:
                        controller_state &= ~0x10;
                        break;
                    case SDL_SCANCODE_DOWN:
                        controller_state &= ~0x20;
                        break;
                    case SDL_SCANCODE_LEFT:
                        controller_state &= ~0x40;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        controller_state &= ~0x80;
                        break;
                    default:
                        break;
                    }
                }
            }

            bus_set_controller(0, controller_state);
            bus_latch_controllers();

            SDL_UpdateTexture(texture, NULL, ppu.framebuffer, 256 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}