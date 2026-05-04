#pragma once
#include <stdint.h>
#include "rom.h"
#include "ppu.h"
#include "apu.h"

uint8_t read(uint16_t address);
void write(uint16_t address, uint8_t data);
void bus_connect_rom(ROM *rom);
void bus_connect_ppu(PPU *ppu);
void bus_connect_apu(APU *apu);
void bus_set_controller(int port, uint8_t state);
void bus_latch_controllers(void);
extern uint8_t mem[2048];
