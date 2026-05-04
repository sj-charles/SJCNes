#pragma once
#include <stdint.h>

typedef struct {
    uint8_t shift;        
    uint8_t shift_count;  
    uint8_t control;    
    uint8_t chr_bank0; 
    uint8_t chr_bank1; 
    uint8_t prg_bank; 
} MMC1;

typedef struct {
    uint8_t prg_rom_banks;
    uint8_t chr_rom_banks;
    uint8_t *prg_rom;
    uint8_t *chr_rom;
    uint8_t mapper_id;
    uint8_t mirroring;  
    MMC1 mmc1;
} ROM;



int rom_load(ROM *rom, const char *path);