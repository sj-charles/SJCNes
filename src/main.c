#include <stdio.h>
#include "cpu.h"
#include "bus.h"
#include "rom.h"

int main() {
    ROM rom;
    CPU cpu;

    if (!rom_load(&rom, "nestest.nes")) {
        printf("Failed to load ROM\n");
        return 1;
    }

    bus_connect_rom(&rom);
    cpu_init(&cpu);
    cpu.pc = 0xC000;

    // run 10000 instructions and log
    for (int i = 0; i < 100000; i++) {
        printf("%04X  A:%02X X:%02X Y:%02X P:%02X SP:%02X\n",
            cpu.pc, cpu.accumulator, cpu.X, cpu.Y, cpu.status, cpu.SP);
        cpu_step(&cpu);
    }

    return 0;
}