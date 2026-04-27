#include "bus.h"
#include "rom.h"
#include <stdint.h>
#include <stddef.h>

uint8_t mem[2048];
static ROM *cart = 0;

void bus_connect_rom(ROM *rom) {
    cart = rom;
}

static PPU *ppu_ref = NULL;

void bus_connect_ppu(PPU *ppu) {
    ppu_ref = ppu;
}

uint8_t read(uint16_t address) {
    if (address <= 0x07FF) {
        return mem[address];
    }
    if (address <= 0x1FFF) {
        return mem[address & 0x07FF];
    }
    if (address >= 0x2000 && address <= 0x3FFF && ppu_ref != NULL) {
        return ppu_read_register(ppu_ref, address & 0x2007);
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
    if (address >= 0x2000 && address <= 0x3FFF && ppu_ref != NULL) {
        ppu_write_register(ppu_ref, address & 0x2007, data);
        return;
    }
    if (address <= 0x1FFF) {
        mem[address & 0x07FF] = data;
        return;
    }
}