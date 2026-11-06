#ifndef __DNN_L1_H
#define __DNN_L1_H

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    u32 params;
    audio_callback_t callback;
    ring_buf_t dnn_L1_ring_buf;
} dnn_L1_mic_cfg_t;

///库接口外的模块接口声明
void dnn_L1_mic_init(u8 sample_rate, u16 samples, u8 channel);
void dnn_L1_mic_audio_input(u8 *ptr, u32 samples, u32 params);
void dnn_L1_mic_output_callback_set(audio_callback_t callback);
void dnn_L1_mic_exit(void);
void dnn_L1_mic_proc_cb(void);

void dnn_L1_mic_param_set(int8_t dnn_L1_nt);
uint8_t dnn_L1_mic_mute_get(void);
void dnn_L1_mic_mute_set(uint8_t mute);

#endif
