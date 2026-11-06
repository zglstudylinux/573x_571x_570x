#ifndef _I2S_AUDIO_H
#define _I2S_AUDIO_H

#include "bsp_i2s.h"

typedef struct {
    audio_callback_t callback;
    i2s_cfg_t i2s_cfg;
} i2s_ctl_t;

void i2s_audio_in_input(u8 *ptr, u32 samples, u32 params);
void i2s_audio_input_callback_set(audio_callback_t callback);
void i2s_audio_in_init(u8 sample_rate, u16 samples, u8 channel);
void i2s_audio_in_exit(void);

void i2s_audio_in_out_input(u8 *ptr, u32 samples, u32 params);
void i2s_audio_in_out_input_callback_set(audio_callback_t callback);
void i2s_audio_in_out_init(u8 sample_rate, u16 samples, u8 channel);
void i2s_audio_in_out_exit(void);
void i2s_audio_in_out_dma_kick(uint tick_cnt);

//外部api,用于调速
void wl_get_tick1_time(uint tick_cnt);
void wl_play_sync_tick1(bool from_dac, uint8_t interval);
#endif
