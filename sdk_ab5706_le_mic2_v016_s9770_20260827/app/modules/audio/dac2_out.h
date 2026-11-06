#ifndef _DAC2_OUT_H
#define _DAC2_OUT_H

typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

typedef struct {
    u8  mute;
    u8  sample_rate;
    u16 samples;
    u8  low_thr;
    u8  high_thr;
    audio_callback_t callback;
} dac2_out_cfg_t;

typedef struct {
    u8  sample_rate;
    u8  low_thr;
    u8  high_thr;
} dac2_out_param_cfg_t;

void dac2_out_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void dac2_out_audio_output_callback_set(audio_callback_t callback);
void dac2_out_init(u8 sample_rate, u16 samples, u8 channel);
void dac2_out_audio_mute_set(uint8_t mute);


#endif
