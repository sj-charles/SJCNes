#pragma once
#include <stdint.h>
typedef struct  {
    uint8_t accumulator;
    uint8_t X;
    uint8_t Y;
    uint8_t SP;
    uint8_t status;
    uint16_t pc;
} CPU;

void cpu_init(CPU *cpu);
void cpu_step(CPU *cpu);