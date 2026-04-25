#include "bus.h"
#include "rom.h"
#include <stdint.h>

uint8_t mem[2048];
static ROM *cart = 0;

void bus_connect_rom(ROM *rom) {
    cart = rom;
}

uint8_t read(uint16_t address) {
    if (address <= 0x07FF) {
        return mem[address];
    }

    if (address <= 0x1FFF) {
        return mem[address & 0x07FF];
    }

    if (address >= 0x8000 && cart != 0) {
        uint16_t offset = address - 0x8000;

        if (cart->prg_rom_banks == 1) {
            offset &= 0x3FFF;
        }

        return cart->prg_rom[offset];
    }

    return 0;
}

void write(uint16_t address, uint8_t data) {
    if (address <= 0x07FF) {
        mem[address] = data;
        return;
    }

    if (address <= 0x1FFF) {
        mem[address & 0x07FF] = data;
        return;
    }
}