#ifndef __FUNC_DEVICE_H
#define __FUNC_DEVICE_H

void func_device(void);
void func_device_init(void);

///extern api
void func_device_message(u16 msg);

typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

///外部adc api
void mic_init(u8 sample_rate, u16 samples, u8 channel);
void mic_audio_output_callback_set(audio_callback_t callback);
void mic_start(void);
void mic_stop(void);

///外部howling api
/// 支持额外执行noise_gate 外部重定义即可 u8 ng_level_param = 5; u8 ng_level_param_enbale = 1;
void howling_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void howling_audio_output_callback_set(audio_callback_t callback);
void howling_init(u8 sample_rate, u16 samples);
void howling_mic_param_set(s32 magic_param);//防啸叫传参

///外部post gain mic api
///软件数字增益调节，调节直接修改标志位即可 post_gain_level
void post_gain_mic_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void post_gain_mic_audio_output_callback_set(audio_callback_t callback);
void post_gain_mic_init(u8 sample_rate, u16 samples);

///外部mic mute api
void mic_mute_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void mic_mute_audio_output_callback_set(audio_callback_t callback);
void mic_mute_init(u8 sample_rate, u16 samples);

///外部adpcm enc mic api
void adpcm_enc_mic_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void adpcm_enc_mic_init(u8 sample_rate, u16 samples);

///外部sbc enc mic api
void sbc_enc_mic_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void sbc_enc_mic_init(u8 sample_rate, u16 samples);

///外部opus enc mic api
void opus_enc_mic_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void opus_enc_mic_init(u8 sample_rate, u16 samples);


#if WIRELESS_MIC_FULL_DUPLEX_EN
///外部sbc dec audio api
void sbc_dec_audio_output_callback_set(audio_callback_t callback);
void sbc_dec_audio_init(u8 sample_rate, u16 samples);

///外部plc sbc api
void plc_sbc_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void plc_sbc_audio_output_callback_set(audio_callback_t callback);
void plc_sbc_audio_init(u8 sample_rate, u16 samples);
#endif

void func_device_interphone_set_con_cmp(void);
void func_device_set_create_con(void);

///外部dac out api
void dac_out_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void dac_out_audio_output_callback_set(audio_callback_t callback);
void dac_out_init(u8 sample_rate, u16 samples);



///外部dac api //此级音效处理不传到下一级，直接dac输出
void dac1_fast_out_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void dac1_fast_out_audio_output_callback_set(audio_callback_t callback);
void dac1_fast_out_init(u8 sample_rate, u16 samples);

void func_device_set_create_con(void);
#endif
