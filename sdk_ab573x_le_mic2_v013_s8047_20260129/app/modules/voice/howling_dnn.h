#ifndef __HOWLING_DNN_H
#define __HOWLING_DNN_H

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    u32 params;
    audio_callback_t callback;
} howling_dnn_mic_cfg_t;

///库接口外的模块接口声明
void howling_dnn_mic_init(u8 sample_rate, u16 samples, u8 channel);
void howling_dnn_mic_audio_input(u8 *ptr, u32 samples, u32 params);
void howling_dnn_mic_output_callback_set(audio_callback_t callback);
void howling_dnn_mic_exit(void);
void howling_dnn_mic_proc_cb(void);

void howling_dnn_mic_param_set(int8_t howling_dnn_nt);
u8 howling_dnn_mic_mute_get(void);
void howling_dnn_mic_mute_set(uint8_t mute);

#endif
