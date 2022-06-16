#include <stdint.h>
#include "pulse_detector.h"

static inline void reset_detector(PULSE_DETECTOR* pd)
{
    pd->bpm = 0;
    pd->previous_sample = 0;
    pd->sample_counter = 0;
    pd->state = STATE_IDLE;
}

static inline uint8_t pulse_sanity_check(uint32_t bpm)
{
    return (bpm < BPM_MIN || bpm > BPM_MAX) ? 1 : 0;
}

void measure_heart_rate(PULSE_DETECTOR* pd, uint16_t current_sample)
{
    uint8_t is_beat = (current_sample - pd->previous_sample > HR_BEAT_THRESHOLD) ? 1 : 0;

    switch(pd->state)
    {
        case STATE_IDLE:
            if(is_beat)
            {
                pd->sample_counter++;
                pd->state = STATE_IN_PROGRESS;
            }
            break;

        case STATE_IN_PROGRESS:
            pd->sample_counter++;
            if(is_beat)
            {
                float num_seconds_between_peaks = pd->sample_counter / 100.0; // TODO: replace 100
                float bpm_f = (SECONDS_PER_MINUTE / num_seconds_between_peaks);
                if(pulse_sanity_check((uint16_t)bpm_f))
                {
                    reset_detector(pd);
                    break;
                }
                pd->bpm = (uint16_t)bpm_f;
                pd->state = STATE_READY;
            }
            break;

        case STATE_READY:
            pd->sample_counter = 0;
            pd->state = STATE_IN_PROGRESS;
            break;
    }

    pd->previous_sample = current_sample;
}
