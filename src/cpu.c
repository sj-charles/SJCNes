#include "cpu.h"
#include "bus.h"
#include "addressing.h"

#include <stdint.h>


void cpu_init(CPU *cpu) {
    cpu->accumulator = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->pc = 0;
    cpu->SP = 0xFD;
    cpu->status = 0x24;
}
void set_flags(CPU *cpu, uint8_t value) {
    if(value == 0x0) {
        cpu->status = cpu->status | 0x02; 
    } else {
        cpu->status = cpu->status & 0xFD; 
    }
     if((value >> 7) & 1) {
        cpu->status = cpu->status | 0x80;
    } else {
        cpu->status = cpu->status & 0x7F;
    }
}
void stack_push(CPU *cpu, uint8_t val) {
    write(0x0100 + cpu->SP, val);
    cpu->SP--;
}
uint8_t stack_pull(CPU *cpu) {
    cpu->SP++;
    return read(0x0100 + cpu->SP);
}
void cpu_step(CPU *cpu) {
    // fetch:
    uint8_t fetch = read(cpu->pc);
    cpu->pc = cpu->pc+1;

    //decode:
    switch(fetch) {

        case 0xA9: {
            uint8_t val = addr_immediate(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xA5: {
            uint8_t val = addr_zeropage(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xAD: {
            uint8_t val = addr_absolute(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xB5: {
            uint8_t val = addr_zeropage_x(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xBD: {
            uint8_t val = addr_absolute_x(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xB9: {
            uint8_t val = addr_absolute_y(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xA1: {
            uint8_t val = addr_indirect_x(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;

        }
        case 0xB1: {
            uint8_t val = addr_indirect_y(cpu);
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x85: {
            uint16_t val = addr_zeropage_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0x95: {
            uint16_t val = addr_zeropage_x_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0x8D: {
            uint16_t val = addr_absolute_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0x9D: {
            uint16_t val = addr_absolute_x_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0x99: {
            uint16_t val = addr_absolute_y_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0x81: {
            uint16_t val = addr_indirect_x_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0x91: {
            uint16_t val = addr_indirect_y_addr(cpu);
            write(val, cpu->accumulator);
            break;
        }
        case 0xA2: {
            uint8_t val = addr_immediate(cpu);
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xA6: {
            uint8_t val = addr_zeropage(cpu);
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xB6: {
            uint8_t val = addr_zeropage_y(cpu);
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xAE: {
            uint8_t val = addr_absolute(cpu);
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xBE: {
            uint8_t val = addr_absolute_y(cpu);
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xA0: {
            uint8_t val = addr_immediate(cpu);
            cpu->Y = val;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xA4: {
            uint8_t val = addr_zeropage(cpu);
            cpu->Y = val;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xB4: {
            uint8_t val = addr_zeropage_x(cpu);
            cpu->Y = val;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xAC: {
            uint8_t val = addr_absolute(cpu);
            cpu->Y = val;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xBC: {
            uint8_t val = addr_absolute_x(cpu);
            cpu->Y = val;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xAA: {
            uint8_t val = cpu->accumulator;
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xA8: {
            uint8_t val = cpu->accumulator;
            cpu->Y = val;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0x8A: {
            uint8_t val = cpu->X;
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x98: {
            uint8_t val = cpu->Y;
            cpu->accumulator = val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x9A: {
            uint8_t val = cpu->X;
            cpu->SP = val;
            break;
        }
        case 0xBA: {
            uint8_t val = cpu->SP;
            cpu->X = val;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xE8: {
            cpu->X++;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0xC8: {
            cpu->Y++;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xCA: {
            cpu->X--;
            set_flags(cpu, cpu->X);
            break;
        }
        case 0x88: {
            cpu->Y--;
            set_flags(cpu, cpu->Y);
            break;
        }
        case 0xE6: {
            uint16_t address = addr_zeropage_addr(cpu);
            uint8_t val = read(address);
            val++;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xF6: {
            uint16_t address = addr_zeropage_x_addr(cpu);
            uint8_t val = read(address);
            val++;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xEE: {
            uint16_t address = addr_absolute_addr(cpu);
            uint8_t val = read(address);
            val++;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xFE: {
            uint16_t address = addr_absolute_x_addr(cpu);
            uint8_t val = read(address);
            val++;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xC6: {
            uint16_t address = addr_zeropage_addr(cpu);
            uint8_t val = read(address);
            val--;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xD6: {
            uint16_t address = addr_zeropage_x_addr(cpu);
            uint8_t val = read(address);
            val--;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xCE: {
            uint16_t address = addr_absolute_addr(cpu);
            uint8_t val = read(address);
            val--;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0xDE: {
            uint16_t address = addr_absolute_x_addr(cpu);
            uint8_t val = read(address);
            val--;
            write(address, val);
            set_flags(cpu, val);
            break;
        }
        case 0x18: {
            cpu->status = cpu->status & 0xFE;
            break;
        }
        case 0x38: {
            cpu->status = cpu->status | 0x01;
            break;
        }
        case 0x58: {
            cpu->status = cpu->status & 0xFB;
            break;
        }
        case 0x78: {
            cpu->status = cpu->status | 0x04;
            break;
        }
        case 0xB8: {
            cpu->status = cpu->status & 0xBF;
            break;
        }
        case 0xD8: {
            cpu->status = cpu->status & 0xEF;
            break;
        }
        case 0xF8: {
            cpu->status = cpu->status | 0x08;
            break;
        }
        case 0x29: {
            uint8_t val = addr_immediate(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x25: {
            uint8_t val = addr_zeropage(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x35: {
            uint8_t val = addr_zeropage_x(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x2D: {
            uint8_t val = addr_absolute(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x3D: {
            uint8_t val = addr_absolute_x(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x39: {
            uint8_t val = addr_absolute_y(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x21: {
            uint8_t val = addr_indirect_x(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x31: {
            uint8_t val = addr_indirect_y(cpu);
            cpu->accumulator = cpu->accumulator & val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x09: {
            uint8_t val = addr_immediate(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x05: {
            uint8_t val = addr_zeropage(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x15: {
            uint8_t val = addr_zeropage_x(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x0D: {
            uint8_t val = addr_absolute(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x1D: {
            uint8_t val = addr_absolute_x(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x19: {
            uint8_t val = addr_absolute_y(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x01: {
            uint8_t val = addr_indirect_x(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x11: {
            uint8_t val = addr_indirect_y(cpu);
            cpu->accumulator = cpu->accumulator | val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x49: {
            uint8_t val = addr_immediate(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x45: {
            uint8_t val = addr_zeropage(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x55: {
            uint8_t val = addr_zeropage_x(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x4D: {
            uint8_t val = addr_absolute(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x5D: {
            uint8_t val = addr_absolute_x(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x59: {
            uint8_t val = addr_absolute_y(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x41: {
            uint8_t val = addr_indirect_x(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x51: {
            uint8_t val = addr_indirect_y(cpu);
            cpu->accumulator = cpu->accumulator ^ val;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xC9: {
            uint8_t val = addr_immediate(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xC5: {
            uint8_t val = addr_zeropage(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xD5: {
            uint8_t val = addr_zeropage_x(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xCD: {
            uint8_t val = addr_absolute(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xDD: {
            uint8_t val = addr_absolute_x(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xD9: {
            uint8_t val = addr_absolute_y(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xC1: {
            uint8_t val = addr_indirect_x(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xD1: {
            uint8_t val = addr_indirect_y(cpu);
            uint8_t result = cpu->accumulator - val;
            if(cpu->accumulator >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xE0: {
            uint8_t val = addr_immediate(cpu);
            uint8_t result = cpu->X - val;
            if(cpu->X >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xE4: {
            uint8_t val = addr_zeropage(cpu);
            uint8_t result = cpu->X - val;
            if(cpu->X >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xEC: {
            uint8_t val = addr_absolute(cpu);
            uint8_t result = cpu->X - val;
            if(cpu->X >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xC0: {
            uint8_t val = addr_immediate(cpu);
            uint8_t result = cpu->Y - val;
            if(cpu->Y >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xC4: {
            uint8_t val = addr_zeropage(cpu);
            uint8_t result = cpu->Y - val;
            if(cpu->Y >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0xCC: {
            uint8_t val = addr_absolute(cpu);
            uint8_t result = cpu->Y - val;
            if(cpu->Y >= val) {
                cpu->status = cpu->status | 0x01;
            } else {
                cpu->status = cpu->status & 0xFE;
            }
            set_flags(cpu, result);
            break;
        }
        case 0x90: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x01) == 0) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0xB0: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x01) == 1) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0xF0: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x02) == 2) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0xD0: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x02) == 0) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0x30: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x80) == 0x80) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0x10: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x80) == 0) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0x50: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x40) == 0) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0x70: {
            int8_t offset = (int8_t)read(cpu->pc);
            cpu->pc++;
            if((cpu->status & 0x40) == 0x40) {
                cpu->pc = cpu->pc + offset;
            }
            break;
        }
        case 0x4C: {
            uint16_t address = addr_absolute_addr(cpu);
            cpu->pc = address;
            break;
        }
        case 0x6C: {
            uint16_t address = addr_indirect_addr(cpu);
            cpu->pc = address;
            break;
        }
        case 0x20: {
            uint16_t address = addr_absolute_addr(cpu);
            uint16_t return_addr = cpu->pc - 1;
            stack_push(cpu, (return_addr >> 8));  
            stack_push(cpu, (return_addr & 0xFF)); 
            cpu->pc = address;
            break;
        }
        case 0x60: {
            uint8_t lo = stack_pull(cpu);
            uint8_t hi = stack_pull(cpu);
            cpu->pc = ((hi << 8) | lo) + 1;
            break;
        }
        case 0x48: {
            stack_push(cpu, cpu->accumulator);
            break;
        }
        case 0x68: {
            cpu->accumulator = stack_pull(cpu);   
            set_flags(cpu, cpu->accumulator);         
            break;
        }
        case 0x08: {
            stack_push(cpu, cpu->status);
            break;
        }
        case 0x28: {
            cpu->status = stack_pull(cpu);   
            break;
        }
        case 0xEA: {
            break;  
        }
        case 0x86: {
            uint16_t addr = addr_zeropage_addr(cpu);
            write(addr, cpu->X);
            break;  
        }
        case 0x96: {
            uint16_t addr = addr_zeropage_y_addr(cpu);
            write(addr, cpu->X);
            break;  
        }
        case 0x8E: {
            uint16_t addr = addr_absolute_addr(cpu);
            write(addr, cpu->X);
            break;  
        }
        case 0x84: {
            uint16_t addr = addr_zeropage_addr(cpu);
            write(addr, cpu->Y);
            break;  
        }
        case 0x94: {
            uint16_t addr = addr_zeropage_x_addr(cpu);
            write(addr, cpu->Y);
            break;  
        }
        case 0x8C: {
            uint16_t addr = addr_absolute_addr(cpu);
            write(addr, cpu->Y);
            break;  
        }
        case 0x0A: { 
            uint8_t old = cpu->accumulator;
            if (old & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            cpu->accumulator = old << 1;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x06: { 
            uint16_t addr = addr_zeropage_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val <<= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x16: { 
            uint16_t addr = addr_zeropage_x_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val <<= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x0E: { 
            uint16_t addr = addr_absolute_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val <<= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x1E: { 
            uint16_t addr = addr_absolute_x_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val <<= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x4A: { 
            uint8_t old = cpu->accumulator;

            if (old & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            cpu->accumulator = old >> 1;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x46: {
            uint16_t addr = addr_zeropage_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val >>= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x56: {
            uint16_t addr = addr_zeropage_x_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val >>= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x4E: {
            uint16_t addr = addr_absolute_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val >>= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x5E: {
            uint16_t addr = addr_absolute_x_addr(cpu);
            uint8_t val = read(addr);

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val >>= 1;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
         case 0x2A: {
            uint8_t oldCarry = cpu->status & 0x01;
            uint8_t old = cpu->accumulator;

            if (old & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            cpu->accumulator = (old << 1) | oldCarry;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x6A: {
            uint8_t oldCarry = (cpu->status & 0x01) ? 0x80 : 0x00;
            uint8_t old = cpu->accumulator;

            if (old & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            cpu->accumulator = (old >> 1) | oldCarry;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0x69: { 
            uint8_t val = addr_immediate(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) & (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }
         case 0xE9: {
            uint8_t val = addr_immediate(cpu);
            uint16_t diff = cpu->accumulator - val - ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) & (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }
          case 0x24: {
            uint8_t val = addr_zeropage(cpu);

            if ((cpu->accumulator & val) == 0) cpu->status |= 0x02;
            else cpu->status &= 0xFD;

            if (val & 0x80) cpu->status |= 0x80;
            else cpu->status &= 0x7F;

            if (val & 0x40) cpu->status |= 0x40;
            else cpu->status &= 0xBF;

            break;
        }
        case 0x2C: { 
            uint8_t val = addr_absolute(cpu);

            if ((cpu->accumulator & val) == 0) cpu->status |= 0x02;
            else cpu->status &= 0xFD;

            if (val & 0x80) cpu->status |= 0x80;
            else cpu->status &= 0x7F;

            if (val & 0x40) cpu->status |= 0x40;
            else cpu->status &= 0xBF;

            break;
        }
        case 0x00: {
            cpu->pc++;

            stack_push(cpu, (cpu->pc >> 8) & 0xFF);
            stack_push(cpu, cpu->pc & 0xFF);
            stack_push(cpu, cpu->status | 0x10);

            cpu->status |= 0x04;

            uint8_t lo = read(0xFFFE);
            uint8_t hi = read(0xFFFF);
            cpu->pc = (hi << 8) | lo;
            break;
        }
        case 0x40: {
            cpu->status = stack_pull(cpu);

            uint8_t lo = stack_pull(cpu);
            uint8_t hi = stack_pull(cpu);

            cpu->pc = (hi << 8) | lo;
            break;
        }
        case 0x26: {
            uint16_t addr = addr_zeropage_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = cpu->status & 0x01;

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val << 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x36: {
            uint16_t addr = addr_zeropage_x_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = cpu->status & 0x01;

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val << 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x2E: {
            uint16_t addr = addr_absolute_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = cpu->status & 0x01;

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val << 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x3E: {
            uint16_t addr = addr_absolute_x_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = cpu->status & 0x01;

            if (val & 0x80) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val << 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x66: {
            uint16_t addr = addr_zeropage_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = (cpu->status & 0x01) ? 0x80 : 0x00;

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val >> 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x76: {
            uint16_t addr = addr_zeropage_x_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = (cpu->status & 0x01) ? 0x80 : 0x00;

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val >> 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x6E: {
            uint16_t addr = addr_absolute_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = (cpu->status & 0x01) ? 0x80 : 0x00;

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val >> 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x7E: {
            uint16_t addr = addr_absolute_x_addr(cpu);
            uint8_t val = read(addr);
            uint8_t carryIn = (cpu->status & 0x01) ? 0x80 : 0x00;

            if (val & 0x01) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            val = (val >> 1) | carryIn;
            write(addr, val);
            set_flags(cpu, val);
            break;
        }
        case 0x65: {
            uint8_t val = addr_zeropage(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0x75: {
            uint8_t val = addr_zeropage_x(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0x6D: {
            uint8_t val = addr_absolute(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0x7D: {
            uint8_t val = addr_absolute_x(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0x79: {
            uint8_t val = addr_absolute_y(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0x61: {
            uint8_t val = addr_indirect_x(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0x71: {
            uint8_t val = addr_indirect_y(cpu);
            uint16_t sum = cpu->accumulator + val + (cpu->status & 0x01);

            if (sum > 0xFF) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)sum;

            if ((~(cpu->accumulator ^ val) &
                (cpu->accumulator ^ result) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        case 0xE5: {
            uint8_t val = addr_zeropage(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0xF5: {
            uint8_t val = addr_zeropage_x(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0xED: {
            uint8_t val = addr_absolute(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0xFD: {
            uint8_t val = addr_absolute_x(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0xF9: {
            uint8_t val = addr_absolute_y(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0xE1: {
            uint8_t val = addr_indirect_x(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }

        case 0xF1: {
            uint8_t val = addr_indirect_y(cpu);
            uint16_t diff = cpu->accumulator - val -
                ((cpu->status & 0x01) ? 0 : 1);

            if (diff < 0x100) cpu->status |= 0x01;
            else cpu->status &= 0xFE;

            uint8_t result = (uint8_t)diff;

            if (((cpu->accumulator ^ result) &
                (cpu->accumulator ^ val) & 0x80))
                cpu->status |= 0x40;
            else
                cpu->status &= 0xBF;

            cpu->accumulator = result;
            set_flags(cpu, cpu->accumulator);
            break;
        }
        default:
            break;
    }



}