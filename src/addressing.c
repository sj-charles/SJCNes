#include <stdint.h>
#include "addressing.h"
#include "cpu.h"
#include "bus.h"

uint8_t addr_immediate(CPU *cpu) {
    uint8_t val = read(cpu->pc);
    cpu->pc++;
    return val;
}

uint8_t addr_zeropage(CPU *cpu) {
        uint8_t val = read(cpu->pc);
        cpu->pc++;
        uint8_t final_val = read(val);
        return final_val;
}

uint8_t addr_absolute(CPU *cpu) {
    uint8_t val1 = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(cpu->pc);
    cpu->pc++;
    uint16_t address = (val2 << 8)  | val1;
    return read(address); 
}

uint8_t addr_zeropage_x(CPU *cpu) {
        uint8_t val = read(cpu->pc);
        cpu->pc++;
        val+=cpu->X;
        uint8_t final_val = read(val);
        return final_val;
}

uint8_t addr_zeropage_y(CPU *cpu) {
        uint8_t val = read(cpu->pc);
        cpu->pc++;
        val+=cpu->Y;
        uint8_t final_val = read(val);
        return final_val;
}

uint8_t addr_absolute_x(CPU *cpu) {
    uint8_t val1 = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(cpu->pc);
    cpu->pc++;
    uint16_t address = (val2 << 8)  | val1;
    address+=cpu->X;
    return read(address); 
}
uint8_t addr_absolute_y(CPU *cpu) {
    uint8_t val1 = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(cpu->pc);
    cpu->pc++;
    uint16_t address = (val2 << 8)  | val1;
    address+=cpu->Y;
    return read(address); 
}

uint8_t addr_indirect_x(CPU *cpu) {
    uint8_t val = read(cpu->pc);
    cpu->pc++;
    val+=cpu->X;
    uint8_t val2 = read(val);
    val++;
    uint8_t val3 = read(val);
    uint16_t address = (val3 << 8) | val2;    
    return read(address);

}

uint8_t addr_indirect_y(CPU *cpu) {
    uint8_t val = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(val);
    val++;
    uint8_t val3 = read(val);
    uint16_t address = (val3 << 8) | val2;  
    address += cpu->Y; 
    return read(address);

}

uint16_t addr_zeropage_addr(CPU *cpu) {
    uint8_t val = read(cpu->pc);
    cpu->pc++;
    return val;
}

uint16_t addr_absolute_addr(CPU *cpu) {
    uint8_t val1 = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(cpu->pc);
    cpu->pc++;
    uint16_t address = (val2 << 8)  | val1;
    return address;
}
uint16_t addr_zeropage_x_addr(CPU *cpu) {
    uint8_t val = read(cpu->pc);
        cpu->pc++;
        val+=cpu->X;
        return val;
        
}
uint16_t addr_zeropage_y_addr(CPU *cpu) {
    uint8_t val = read(cpu->pc);
        cpu->pc++;
        val+=cpu->Y;
        return val;       
}
uint16_t addr_absolute_x_addr(CPU *cpu) {
    uint8_t val1 = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(cpu->pc);
    cpu->pc++;
    uint16_t address = (val2 << 8)  | val1;
    address+=cpu->X;
    return address;
}
uint16_t addr_absolute_y_addr(CPU *cpu) {
    uint8_t val1 = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(cpu->pc);
    cpu->pc++;
    uint16_t address = (val2 << 8)  | val1;
    address+=cpu->Y;
    return address;
}
uint16_t addr_indirect_x_addr(CPU *cpu) {
    uint8_t val = read(cpu->pc);
    cpu->pc++;
    val+=cpu->X;
    uint8_t val2 = read(val);
    val++;
    uint8_t val3 = read(val);
    uint16_t address = (val3 << 8) | val2;    
    return address;
}
uint16_t addr_indirect_y_addr(CPU *cpu) {
        uint8_t val = read(cpu->pc);
    cpu->pc++;
    uint8_t val2 = read(val);
    val++;
    uint8_t val3 = read(val);
    uint16_t address = (val3 << 8) | val2;  
    address += cpu->Y; 
    return address;
}
uint16_t addr_indirect_addr(CPU *cpu) {
    uint8_t lo = read(cpu->pc);
    cpu->pc++;
    uint8_t hi = read(cpu->pc);
    cpu->pc++;
    uint16_t pointer = (hi << 8) | lo;
    uint8_t lo2 = read(pointer);
    uint8_t hi2 = read(pointer + 1);
    return (hi2 << 8) | lo2;
}

