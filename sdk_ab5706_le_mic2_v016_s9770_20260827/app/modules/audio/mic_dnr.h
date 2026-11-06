#ifndef __MIC_DNR_H
#define __MIC_DNR_H


typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    u16 maxpow_mic;
    volatile u8 mic_dnr_sta;
    audio_callback_t callback;
} mic_dnrpwr_cfg_t;

void mic_dnr_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void mic_dnr_audio_output_callback_set(audio_callback_t callback);
void mic_dnr_audio_init(u8 sample_rate, u16 samples, u8 channel);
void mic_dnr_audio_mute_set(uint8_t mute);
void mic_dnr_audio_set_param(u16 room_decay_set, u16 room_dry, u16 room_wet);
void mic_dnr_process(void);
#endif
