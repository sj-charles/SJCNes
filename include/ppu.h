#pragma once
#include <stdint.h>

typedef struct {
    uint8_t PPUCTRL;
    uint8_t PPUMASK;
    uint8_t PPUSTATUS;
    uint8_t OAMADDR;
    uint8_t OAMDATA;
    uint8_t PPUSCROLL;
    uint8_t PPUADDR;
    uint8_t PPUDATA;

    uint16_t v;  
    uint16_t t;  
    uint8_t x;   
    uint8_t w;   

    uint8_t *chr_rom;

    uint8_t oam[256];       
    uint8_t vram[2048];     
    uint8_t palette[32];     

    uint32_t framebuffer[256 * 240];

    int cycle;
    int scanline;
    int frame;
    int nmi_pending;

} PPU;

void ppu_init(PPU *ppu);
uint8_t ppu_read_register(PPU *ppu, uint16_t address);
void ppu_write_register(PPU *ppu, uint16_t address, uint8_t data);
void ppu_step(PPU *ppu);
void ppu_connect_chr(PPU *ppu, uint8_t *chr);
void ppu_render_pixel(PPU *ppu);