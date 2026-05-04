#pragma once
#include <stdint.h>

typedef struct {
    uint8_t enabled;
    uint8_t duty;    
    uint8_t length_counter;
    uint8_t envelope_volume;
    uint8_t envelope_decay;
    uint8_t envelope_loop;
    uint8_t envelope_disable;
    uint16_t timer;
    uint16_t timer_period;
    uint8_t sequencer;
} PulseChannel;

typedef struct {
    PulseChannel pulse[2];
    uint8_t frame_counter;
    float sample_buffer[4096];
    int sample_count;
} APU;

void apu_init(APU *apu);
void apu_write(APU *apu, uint16_t address, uint8_t data);
void apu_step(APU *apu);
float apu_sample(APU *apu);