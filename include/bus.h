#pragma once
#include <stdint.h>
#include "rom.h"

uint8_t read(uint16_t address);
void write(uint16_t address, uint8_t data);
void bus_connect_rom(ROM *rom);

extern uint8_t mem[2048];