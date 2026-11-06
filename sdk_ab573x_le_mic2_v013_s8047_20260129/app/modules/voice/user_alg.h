#ifndef __USER_ALG_H
#define __USER_ALG_H

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    audio_callback_t callback;
} user_mic_cfg_t;

void user_mic_proc_cb(void);

void user_mic_init(u8 sample_rate, u16 samples, u8 channel);
void user_mic_audio_input(u8 *ptr, u32 samples, u32 params);
void user_mic_output_callback_set(audio_callback_t callback);
#endif
