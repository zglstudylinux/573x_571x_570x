#ifndef _I2S_AUDIO_H
#define _I2S_AUDIO_H

#include "bsp_i2s.h"

#define I2S_INCACHE_SIZE            1024
#define I2S_OUTCACHE_SIZE           512

typedef struct {
#if I2S_AUDIO_IN_EN || I2S_AUDIO_OUT_EN
    uint8_t cache[I2S_INCACHE_SIZE];
    uint8_t frame[I2S_OUTCACHE_SIZE];

    ring_buf_t ring_buf;
#elif ADAPTER_I2S_IN_OUT_EN
    uint8_t rx_cache[I2S_INCACHE_SIZE];
    uint8_t rx_frame[I2S_OUTCACHE_SIZE];

    uint8_t tx_cache[I2S_INCACHE_SIZE];
    uint8_t tx_frame[I2S_OUTCACHE_SIZE];

    ring_buf_t rx_ring_buf;
    ring_buf_t tx_ring_buf;
#endif
} i2s_audio_buf_t;

typedef struct {
    audio_callback_t callback;
    i2s_cfg_t i2s_cfg;
} i2s_in_ctl_t;


void i2s_audio_in_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void i2s_audio_input_callback_set(audio_callback_t callback);
void i2s_audio_in_init(u8 sample_rate, u16 samples, u8 channel);

//I2S OUTPUT
void i2s_audio_out_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void i2s_audio_output_callback_set(audio_callback_t callback);
void i2s_audio_out_init(u8 sample_rate, u16 samples, u8 channel);

//双向I2S时,主机和从机共用以下接口
void i2s_audio_in_and_out_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void i2s_audio_in_and_output_callback_set(audio_callback_t callback);
void i2s_audio_in_and_out_init(u8 sample_rate, u16 samples, u8 channel);


#endif
