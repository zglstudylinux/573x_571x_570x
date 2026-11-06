#ifndef __DNN_L2_H
#define __DNN_L2_H

///公共
typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    audio_callback_t callback;
} dnn_L2_mic_cfg_t;

///库接口外的模块接口声明
void dnn_L2_mic_init(u8 sample_rate, u16 samples, u8 channel);
void dnn_L2_mic_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void dnn_L2_mic_output_callback_set(audio_callback_t callback);
void dnn_L2_mic_param_set(s16 dnn_L2_nt);
uint8_t dnn_L2_mic_mute_get(void);
void dnn_L2_mic_mute_set(uint8_t mute);
void dnn_L2_mic_exit(void);
void dnn_L2_mic_proc_cb(void);

#endif
