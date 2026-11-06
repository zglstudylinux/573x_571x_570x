#ifndef _MIC_MIX_H
#define _MIC_MIX_H

typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

typedef struct {
    volatile u8 done_sem;
} mic_mix_t;

void mic_mix_init(void);
void mic_mix_audio_proc(s16 *pcm0, s16 *pcm1, uint8_t samples, s16 *pcm_out);
void mic_mix_exit(void);
u8 mic_mix_done_get(void);
void alg_mic_mix_audio_proc_input(s16 *ptr, u32 samples, s16 *pcm_out);
#endif
