#ifndef  __MIC_H_
#define  __MIC_H_


void mic_init(u8 sample_rate, u16 samples, u8 channel);
void mic_audio_output_callback_set(audio_callback_t callback);
void mic_start(void);
void mic_stop(void);
void wireless_mic_reset(void);
void mic_leave_out_rc_proc(void);
void mic_leave_out_rc_init(void);
void mic_bias_trim(void);

void sdadc_start_kick(u16 channel);
void wireless_mic_kick(void);
void local_mic_init(void);
bool karaok_local_mic_pcm_get(u8 *ptr, u32 samples);
void local_mic_effect_process(void);
void karaok_local_mic_mute_set(uint8_t mute);
#endif
