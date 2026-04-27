#include "ppu.h"
#include "bus.h"
#include "addressing.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

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
    ppu->nmi_pending = 0;
}

static uint32_t nes_palette[64] = {
    0xFF545454, 0xFF001E74, 0xFF081090, 0xFF300088,
    0xFF440064, 0xFF5C0030, 0xFF540400, 0xFF3C1800,
    0xFF202A00, 0xFF083A00, 0xFF004000, 0xFF003C00,
    0xFF00323C, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFF989698, 0xFF084CC4, 0xFF3032EC, 0xFF5C1EE4,
    0xFF8814B0, 0xFFA01464, 0xFF982220, 0xFF783C00,
    0xFF545A00, 0xFF287200, 0xFF087C00, 0xFF007628,
    0xFF006678, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFECEEEC, 0xFF4C9AEC, 0xFF787CEC, 0xFFB062EC,
    0xFFE454EC, 0xFFEC58B4, 0xFFEC6A64, 0xFFD48820,
    0xFFA0AA00, 0xFF74C400, 0xFF4CD020, 0xFF38CC6C,
    0xFF38B4CC, 0xFF3C3C3C, 0xFF000000, 0xFF000000,
    0xFFECEEEC, 0xFFA8CCEC, 0xFFBCBCEC, 0xFFD4B2EC,
    0xFFECAEEC, 0xFFECAED4, 0xFFECB4B0, 0xFFE4C490,
    0xFFCCD278, 0xFFB4DE78, 0xFFA8E290, 0xFF98E2B4,
    0xFFA0D6E4, 0xFFA0A2A0, 0xFF000000, 0xFF000000,
};

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
                ppu->t = (ppu->t & 0x00FF) | (data << 8); 
                ppu->w = 1;
            } else {
                ppu->t = (ppu->t & 0xFF00) | data;
                ppu->v = ppu->t;  
                ppu->w = 0;
            }
            break;
        case 0x2007: {
           uint16_t addr = ppu->v & 0x3FFF;
        if (addr >= 0x3F00) {
        ppu->palette[addr & 0x1F] = data;
    } else {
        ppu->vram[addr & 0x07FF] = data;
    }
    ppu->v += (ppu->PPUCTRL & 0x04) ? 32 : 1;
        break;
        }
        default:
            break;
    }
}

uint8_t ppu_read_register(PPU *ppu, uint16_t address) {
    switch (address) {
        case 0x2002: {
            uint8_t status = ppu->PPUSTATUS;
            ppu->PPUSTATUS &= 0x7F;
            ppu->w = 0;
            return status;
        
            }
        case 0x2004: 
            return ppu->oam[ppu->OAMADDR];
        case 0x2007: {
            uint16_t addr = ppu->v & 0x3FFF;
            uint8_t data;
            if (addr >= 0x3F00) {
                data = ppu->palette[addr & 0x1F];
            } else {
                data = ppu->vram[addr & 0x07FF];
            }
            ppu->v += (ppu->PPUCTRL & 0x04) ? 32 : 1;
            return data;
        }
        default:
            return 0;
    }
}

void ppu_step(PPU *ppu) {
    ppu->cycle+=1;
    if(ppu->cycle == 341) {
        ppu->cycle = 0;
        ppu->scanline+=1;
    }
    if(ppu->scanline == 262) {
        ppu->scanline = -1;
        ppu->frame+=1;
    }
    if (ppu->scanline == 241 && ppu->cycle == 1) {
        ppu->PPUSTATUS |= 0x80;  
        if (ppu->PPUCTRL & 0x80) {
            ppu->nmi_pending = 1;
        }
    }
    if (ppu->scanline == -1 && ppu->cycle == 1) {
        ppu->PPUSTATUS &= 0x7F; 
        ppu->nmi_pending = 0;
    }

    if (ppu->scanline == 240 && ppu->cycle == 1) {
        ppu_render_sprites(ppu);
    }

    if (ppu->scanline >= 0 && ppu->scanline < 240 && 
        ppu->cycle >= 1 && ppu->cycle <= 256) {
        ppu_render_pixel(ppu);
    }
}

void ppu_connect_chr(PPU *ppu, uint8_t *chr) {
    ppu->chr_rom = chr;
}

void ppu_render_pixel(PPU *ppu) {
    int x = ppu->cycle - 1;
    int y = ppu->scanline;

    if(x < 0 || x >= 256 || y < 0 || y >= 240) {
        return;
    }

    int tile_x = x/8;
    int tile_y = y/8;

    uint16_t nametable_addr = 0x2000 + tile_y * 32 + tile_x;
    uint8_t tile_index = ppu->vram[nametable_addr & 0x0FFF];

    int fine_x = x % 8;
    int fine_y = y % 8;

    uint16_t pattern_addr = tile_index * 16 + fine_y;
    uint8_t lo = ppu->chr_rom[pattern_addr];
    uint8_t hi = ppu->chr_rom[pattern_addr + 8];

    int pixel = ((hi >> (7 - fine_x)) & 1) << 1 |
                ((lo >> (7 - fine_x)) & 1);
    
  uint8_t palette_index = ppu->palette[pixel & 3];
  ppu->framebuffer[y * 256 + x] = nes_palette[palette_index & 0x3F];
}

void ppu_render_sprites(PPU *ppu) {
for(int i = 0; i < 64; i++) {
    uint8_t y          = ppu->oam[i * 4 + 0];
    uint8_t tile_index = ppu->oam[i * 4 + 1];
    uint8_t attr       = ppu->oam[i * 4 + 2];
    uint8_t x          = ppu->oam[i * 4 + 3];

    int flip_h = (attr & 0x40) ? 1 : 0;
    int flip_v = (attr & 0x80) ? 1 : 0;

    uint16_t pattern_base = (ppu->PPUCTRL & 0x08) ? 0x1000 : 0x0000;

    for(int row = 0; row < 8; row++) {
        int effective_row = flip_v ? (7 - row) : row;
        uint16_t pattern_addr = pattern_base + tile_index * 16 + effective_row;
        uint8_t lo = ppu->chr_rom[pattern_addr];
        uint8_t hi = ppu->chr_rom[pattern_addr + 8];

        for(int col = 0; col < 8; col++) {
            int effective_col = flip_h ? (7 - col) : col;
            int pixel = ((hi >> (7 - effective_col)) & 1) << 1 |
                        ((lo >> (7 - effective_col)) & 1);
            if(pixel == 0) continue;

            int px = x + col;
            int py = y + row + 1;
            if(px >= 256 || py >= 240) continue;

            uint8_t palette_index = ppu->palette[0x10 + (attr & 0x03) * 4 + pixel];
            ppu->framebuffer[py * 256 + px] = nes_palette[palette_index & 0x3F];
        }
    }
  }
}
