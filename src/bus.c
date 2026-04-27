#include "bus.h"
#include "rom.h"
#include <stdint.h>
#include <stddef.h>

static uint8_t controller[2] = {0, 0};
static uint8_t controller_shift[2] = {0, 0};
static uint8_t strobe = 0;

void bus_set_controller(int port, uint8_t state) {
    controller[port] = state;
}


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
    if (address == 0x4016) {
        uint8_t val = (controller_shift[0] & 0x80) ? 1 : 0;
        controller_shift[0] <<= 1;
        return val;
    }
    if (address == 0x4017) {
        uint8_t val = (controller_shift[1] & 0x80) ? 1 : 0;
        controller_shift[1] <<= 1;
        return val;
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
    if (address == 0x4014) {
        uint16_t base = data << 8;
        for (int i = 0; i < 256; i++) {
            ppu_ref->oam[i] = read(base + i);
        }
        return;
    }
    if(address == 0x4016) {
        if (data & 0x01) {
            strobe = 1;
        } else if (strobe) {
            strobe = 0;
            controller_shift[0] = controller[0];
            controller_shift[1] = controller[1];
        }
        return;
    }
    if (address <= 0x1FFF) {
        mem[address & 0x07FF] = data;
        return;
    }
}


