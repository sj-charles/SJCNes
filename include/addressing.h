#pragma once
#include <stdint.h>
#include "cpu.h"

uint8_t addr_immediate(CPU *cpu);
uint8_t addr_zeropage(CPU *cpu);
uint8_t addr_absolute(CPU *cpu);
uint8_t addr_zeropage_x(CPU *cpu);
uint8_t addr_zeropage_y(CPU *cpu);
uint8_t addr_absolute_x(CPU *cpu);
uint8_t addr_absolute_y(CPU *cpu);
uint8_t addr_indirect_x(CPU *cpu);
uint8_t addr_indirect_y(CPU *cpu);


uint16_t addr_zeropage_addr(CPU *cpu);
uint16_t addr_absolute_addr(CPU *cpu);
uint16_t addr_zeropage_x_addr(CPU *cpu);
uint16_t addr_zeropage_y_addr(CPU *cpu);
uint16_t addr_absolute_x_addr(CPU *cpu);
uint16_t addr_absolute_y_addr(CPU *cpu);
uint16_t addr_indirect_x_addr(CPU *cpu);
uint16_t addr_indirect_y_addr(CPU *cpu);
uint16_t addr_indirect_addr(CPU *cpu);