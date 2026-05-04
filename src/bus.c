#include "bus.h"
#include "rom.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

static uint8_t controller[2] = {0, 0};
static uint8_t controller_shift[2] = {0, 0};
static uint8_t strobe = 0;
static uint8_t prg_ram[8192] = {0};

void bus_set_controller(int port, uint8_t state)
{
    controller[port] = state;
}

void bus_latch_controllers(void)
{
    controller_shift[0] = controller[0];
    controller_shift[1] = controller[1];
}

uint8_t mem[2048];
static ROM *cart = 0;

void bus_connect_rom(ROM *rom)
{
    cart = rom;
}

static PPU *ppu_ref = NULL;

void bus_connect_ppu(PPU *ppu)
{
    ppu_ref = ppu;
}

uint8_t read(uint16_t address)
{
    if (address <= 0x07FF)
        return mem[address];

    if (address <= 0x1FFF)
        return mem[address & 0x07FF];

    if (address >= 0x2000 && address <= 0x3FFF && ppu_ref != NULL)
        return ppu_read_register(ppu_ref, 0x2000 + (address % 8));
    if (address == 0x4016)
    {
        uint8_t val = controller_shift[0] & 1;
        if (!strobe)
            controller_shift[0] >>= 1;
        if (val)
            //printf("4016 read: val=%d shift=%02X\n", val, controller_shift[0]);
        return 0x40 | val;
    }
    if (address == 0x4017)
    {
        uint8_t val = controller_shift[1] & 1;
        if (!strobe)
            controller_shift[1] >>= 1;
        return 0x40 | val;
    }

    if (address >= 0x6000 && address < 0x8000)
        return prg_ram[address % 8192];

    if (address >= 0x8000 && cart != 0)
    {
        if (cart->mapper_id == 1)
        {
            uint8_t prg_mode = (cart->mmc1.control >> 2) & 0x03;
            uint8_t bank = cart->mmc1.prg_bank & 0x0F;
            uint32_t offset;

            if (prg_mode == 2)
            {
                if (address < 0xC000)
                    offset = address - 0x8000;
                else
                    offset = (bank * 0x4000) + (address - 0xC000);
            }
            else if (prg_mode == 3)
            {
                if (address >= 0xC000)
                    offset = (cart->prg_rom_banks - 1) * 0x4000 + (address - 0xC000);
                else
                    offset = (bank * 0x4000) + (address - 0x8000);
            }
            else
            {
                offset = ((bank >> 1) * 0x8000) + (address - 0x8000);
            }
            return cart->prg_rom[offset];
        }

        uint16_t offset = address - 0x8000;
        if (cart->prg_rom_banks == 1)
            offset &= 0x3FFF;
        return cart->prg_rom[offset];
    }

    return 0;
}

void mmc1_write(uint16_t address, uint8_t data)
{
    if (data & 0x80)
    {
        cart->mmc1.shift = 0;
        cart->mmc1.shift_count = 0;
        cart->mmc1.control |= 0x0C;
        return;
    }

    cart->mmc1.shift = (cart->mmc1.shift >> 1) | ((data & 1) << 4);
    cart->mmc1.shift_count++;

    if (cart->mmc1.shift_count == 5)
    {
        uint8_t value = cart->mmc1.shift;
        if (address <= 0x9FFF)
            cart->mmc1.control = value;
        else if (address <= 0xBFFF)
            cart->mmc1.chr_bank0 = value;
        else if (address <= 0xDFFF)
            cart->mmc1.chr_bank1 = value;
        else
            cart->mmc1.prg_bank = value;

        cart->mmc1.shift = 0;
        cart->mmc1.shift_count = 0;
    }
}

void write(uint16_t address, uint8_t data)
{
    if (address <= 0x07FF)
    {
        mem[address] = data;
        return;
    }

    if (address >= 0x2000 && address <= 0x3FFF && ppu_ref != NULL)
    {
        ppu_write_register(ppu_ref, 0x2000 + (address % 8), data);
        return;
    }

    if (address == 0x4014)
    {
        uint16_t base = data << 8;
        for (int i = 0; i < 256; i++)
            ppu_ref->oam[i] = read(base + i);
        return;
    }

    if (address == 0x4016)
    {
        strobe = data & 1;
        if (strobe)
        {
            controller_shift[0] = controller[0];
            controller_shift[1] = controller[1];
        }
        return;
    }
    if (address <= 0x1FFF)
    {
        mem[address & 0x07FF] = data;
        return;
    }

    if (address >= 0x6000 && address < 0x8000)
    {
        prg_ram[address % 8192] = data;
        return;
    }

    if (address >= 0x8000)
    {
        if (cart->mapper_id == 1)
            mmc1_write(address, data);
        return;
    }
}