#include "ppu.h"
#include "bus.h"
#include "addressing.h"
#include <string.h>
#include <stdint.h>

void ppu_init(PPU *ppu) {
    ppu->PPUMASK = 0;
    ppu->PPUSTATUS = 0;
    ppu->PPUCTRL = 0;
    ppu->OAMADDR = 0;
    ppu->OAMDATA = 0;
    ppu->PPUSCROLL = 0;
    ppu->PPUADDR = 0;
    ppu->PPUDATA = 0;

    ppu->v = 0;
    ppu->t = 0;
    ppu->x = 0;
    ppu->w = 0;

    memset(ppu->oam, 0, sizeof(ppu->oam));
    memset(ppu->vram, 0, sizeof(ppu->vram));
    memset(ppu->palette, 0, sizeof(ppu->palette));
    memset(ppu->framebuffer, 0, sizeof(ppu->framebuffer));
    
    ppu->cycle = 0;
    ppu->scanline = -1;
    ppu->frame = 0;
}

void ppu_write_register(PPU *ppu, uint16_t address, uint8_t data) {
    switch(address) {
        case 0x2000: 
            ppu->PPUCTRL = data;
            break;
        case 0x2001: 
            ppu->PPUMASK = data;
            break;
        case 0x2003: 
            ppu->OAMADDR = data;
            break;
        case 0x2004: 
            ppu->oam[ppu->OAMADDR] = data;
            ppu->OAMADDR+=1;
            break;
        case 0x2005: 
            if(ppu->w == 0) {
                ppu->t = (ppu->t & 0xFF00) | data; 
                ppu->w = 1;
            } else {
                ppu->t = (ppu->t & 0x00FF) | (data << 8);
                ppu->w = 0;  
            }
            break;
        case 0x2006: 
            if(ppu->w == 0) {
                ppu->t = (ppu->t & 0xFF00) | (data << 8); 
                ppu->w = 1;
            } else {
                ppu->t = (ppu->t & 0x00FF) | data;
                ppu->v = ppu->t;  
                ppu->w = 0;
            }
            break;
        case 0x2007:
            ppu->vram[ppu->v] = data;
            ppu->v += (ppu->PPUCTRL & 0x04) ? 32 : 1;
            break;
    }
}