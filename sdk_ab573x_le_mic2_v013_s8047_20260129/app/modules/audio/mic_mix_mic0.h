#ifndef  __MIC_MIX_MIC0_H_
#define  __MIC_MIX_MIC0_H_

void mic_stereo_init(u8 sample_rate, u16 samples, u8 channel);
void mic_stereo_audio_input(u8 *ptr, u32 samples, u32 params);
void mic_stereo_audio_output_callback_set(audio_callback_t callback);

int s_clip16(int x);
#endif
