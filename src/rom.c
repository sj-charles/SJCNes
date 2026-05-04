#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rom.h"

int rom_load(ROM *rom, const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return 0;

    uint8_t header[16];

    if (fread(header, 1, 16, file) != 16) {
        fclose(file);
        return 0;
    }

    if (header[0] != 'N' ||
        header[1] != 'E' ||
        header[2] != 'S' ||
        header[3] != 0x1A) {
        fclose(file);
        return 0;
    }

    printf("Header bytes 6,7: %02X %02X\n", header[6], header[7]);
    
    rom->prg_rom_banks = header[4];
    rom->chr_rom_banks = header[5];
    rom->mirroring = header[6] & 0x01;

    if (header[6] & 0x04)
        fseek(file, 512, SEEK_CUR);

    rom->mapper_id = (header[6] >> 4) | (header[7] & 0xF0);
    rom->mmc1.shift = 0;
    rom->mmc1.shift_count = 0;
    rom->mmc1.control = 0x0C;
    rom->mmc1.chr_bank0 = 0;
    rom->mmc1.chr_bank1 = 0;
    rom->mmc1.prg_bank = 0;

    int prg_size = rom->prg_rom_banks * 16384;
    rom->prg_rom = malloc(prg_size);
    fread(rom->prg_rom, 1, prg_size, file);

    int chr_size;

    if (rom->chr_rom_banks == 0) {
        chr_size = 8192;
        rom->chr_rom = malloc(chr_size);
        memset(rom->chr_rom, 0, chr_size);
    } else {
        chr_size = rom->chr_rom_banks * 8192;
        rom->chr_rom = malloc(chr_size);
        fread(rom->chr_rom, 1, chr_size, file);
    }

    fclose(file);
    return 1;
}