#ifndef __SRC_H_
#define __SRC_H_

typedef struct {
    u8  mute;
    u8  sample_rate;
    u16 samples;
    u8  low_thr;
    u8  high_thr;
    audio_callback_t callback;
} src_cfg_t;


void src0_init(u8 sample_rate, u16 samples, u8 channel);
void src0_audio_input(u8 *ptr, u32 samples, u32 params);
void src0_audio_output_callback_set(audio_callback_t callback);
void src0_audio_mute(u8 sta);

void src0_audio_input_dump(u8 *ptr, u32 samples, u32 params);
#endif
