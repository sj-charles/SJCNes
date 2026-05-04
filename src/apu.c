#include "apu.h"
#include <string.h>
#include <stdio.h>

static const uint8_t duty_table[4][8] = {
    {0, 1, 0, 0, 0, 0, 0, 0}, // 12.5%
    {0, 1, 1, 0, 0, 0, 0, 0}, // 25%
    {0, 1, 1, 1, 1, 0, 0, 0}, // 50%
    {1, 0, 0, 1, 1, 1, 1, 1}, // 75% (inverted 25%)
};

static const uint8_t length_table[32] = {
    10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

void apu_init(APU *apu)
{
    memset(apu, 0, sizeof(APU));
    apu->pulse[0].enabled = 1;
    apu->pulse[1].enabled = 1;
}

void apu_write(APU *apu, uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0x4000:
        apu->pulse[0].duty = (data >> 6) & 0x03;
        apu->pulse[0].envelope_loop = (data >> 5) & 1;
        apu->pulse[0].envelope_disable = (data >> 4) & 1;
        apu->pulse[0].envelope_volume = data & 0x0F;
        break;
    case 0x4002:
        apu->pulse[0].timer_period =
            (apu->pulse[0].timer_period & 0xFF00) | data;
        break;
    case 0x4003:
        apu->pulse[0].timer_period =
            (apu->pulse[0].timer_period & 0x00FF) | ((data & 0x07) << 8);
        apu->pulse[0].length_counter = length_table[data >> 3];
        apu->pulse[0].sequencer = 0;
        apu->pulse[0].envelope_decay = apu->pulse[0].envelope_volume;
        break;
    case 0x4004:
        apu->pulse[1].duty = (data >> 6) & 0x03;
        apu->pulse[1].envelope_loop = (data >> 5) & 1;
        apu->pulse[1].envelope_disable = (data >> 4) & 1;
        apu->pulse[1].envelope_volume = data & 0x0F;
        break;
    case 0x4006:
        apu->pulse[1].timer_period =
            (apu->pulse[1].timer_period & 0xFF00) | data;
        break;
    case 0x4007:
        apu->pulse[1].timer_period =
            (apu->pulse[1].timer_period & 0x00FF) | ((data & 0x07) << 8);
        apu->pulse[1].length_counter = length_table[data >> 3];
        apu->pulse[1].sequencer = 0;
        apu->pulse[1].envelope_decay = apu->pulse[1].envelope_volume;
        break;
    case 0x4015:
        apu->pulse[0].enabled = (data >> 0) & 1;
        apu->pulse[1].enabled = (data >> 1) & 1;
        break;
    }
}

void apu_step(APU *apu)
{
    for (int i = 0; i < 2; i++)
    {
        if (apu->pulse[i].timer == 0)
        {
            apu->pulse[i].timer = apu->pulse[i].timer_period;
            apu->pulse[i].sequencer = (apu->pulse[i].sequencer + 1) & 7;
        }
        else
        {
            apu->pulse[i].timer--;
        }
    }

    // clock envelope every 3728 APU cycles (~240Hz)
    apu->frame_counter++;
    if (apu->frame_counter >= 3728)
    {
        apu->frame_counter = 0;
        for (int i = 0; i < 2; i++)
        {
            if (!apu->pulse[i].envelope_disable)
            {
                if (apu->pulse[i].envelope_decay > 0)
                    apu->pulse[i].envelope_decay--;
                else if (apu->pulse[i].envelope_loop)
                    apu->pulse[i].envelope_decay = apu->pulse[i].envelope_volume;
            }
        }
    }
}

float apu_sample(APU *apu)
{
    float pulse1 = 0, pulse2 = 0;

    // Pulse 1
    if (apu->pulse[0].enabled &&
        apu->pulse[0].timer_period >= 8)
    {
        uint8_t out = duty_table[apu->pulse[0].duty][apu->pulse[0].sequencer];
        uint8_t vol = apu->pulse[0].envelope_disable ?
                      apu->pulse[0].envelope_volume :
                      apu->pulse[0].envelope_decay;
        pulse1 = out ? vol / 15.0f : 0;
    }

    // Pulse 2
    if (apu->pulse[1].enabled &&
        apu->pulse[1].timer_period >= 8)
    {
        uint8_t out = duty_table[apu->pulse[1].duty][apu->pulse[1].sequencer];
        uint8_t vol = apu->pulse[1].envelope_disable ?
                      apu->pulse[1].envelope_volume :
                      apu->pulse[1].envelope_decay;
        pulse2 = out ? vol / 15.0f : 0;
    }

    // Mix using NES mixer formula
    float pulse_out = 0;
    if (pulse1 + pulse2 > 0)
        pulse_out = 95.88f / (8128.0f / (pulse1 + pulse2) + 100.0f);

    return pulse_out;
}