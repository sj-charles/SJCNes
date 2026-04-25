#pragma once
#include <stdint.h>

typedef struct {
    uint8_t prg_rom_banks;
    uint8_t chr_rom_banks;
    uint8_t *prg_rom;
    uint8_t *chr_rom;
} ROM;

int rom_load(ROM *rom, const char *path);