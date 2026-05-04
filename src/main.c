#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "bus.h"
#include "rom.h"
#include "apu.h"
#include "ppu.h"

int main()
{
    APU apu;
    ROM rom;
    CPU cpu;
    PPU ppu;

    if (!rom_load(&rom, "Donkey Kong.nes"))
    {
        printf("Failed to load ROM\n");
        return 1;
    }

    bus_connect_rom(&rom);
    apu_init(&apu);
    bus_connect_apu(&apu);
    ppu_init(&ppu);
    ppu_connect_chr(&ppu, rom.chr_rom);
    ppu.mirroring = rom.mirroring;
    bus_connect_ppu(&ppu);
    cpu_init(&cpu);
    cpu.pc = read(0xFFFC) | (read(0xFFFD) << 8);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window *window = SDL_CreateWindow(
        "SJCNes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, 256, 240);

    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof(want));
    want.freq = 44100;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 512;
    want.callback = NULL;
    SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_PauseAudioDevice(audio_dev, 0);

    int running = 1;
    SDL_Event event;
    uint8_t controller_state = 0;
    int frame = 0;

#define CPU_FREQ 1789773
#define SAMPLE_RATE 44100
    float sample_counter = 0;
    float audio_step = (float)CPU_FREQ / SAMPLE_RATE;
    static float t = 0;

    while (running)
    {
        cpu_step(&cpu);
        apu_step(&apu);
        ppu_step(&ppu);
        ppu_step(&ppu);
        ppu_step(&ppu);

        sample_counter += 1.0f;
        if (sample_counter >= audio_step)
        {
            sample_counter -= audio_step;
            if (SDL_GetQueuedAudioSize(audio_dev) < 4096)
            {
                float sample = apu_sample(&apu);
                SDL_QueueAudio(audio_dev, &sample, sizeof(float));
            }
        }

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
                        break;
                    case SDL_SCANCODE_X:
                        controller_state |= 0x02;
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        controller_state |= 0x04;
                        break;
                    case SDL_SCANCODE_RETURN:
                        controller_state |= 0x08;
                        break;
                    case SDL_SCANCODE_UP:
                        controller_state |= 0x10;
                        break;
                    case SDL_SCANCODE_DOWN:
                        controller_state |= 0x20;
                        break;
                    case SDL_SCANCODE_LEFT:
                        controller_state |= 0x40;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        controller_state |= 0x80;
                        break;
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

    SDL_CloseAudioDevice(audio_dev);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}