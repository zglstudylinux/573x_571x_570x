#ifndef _USB_DEVICE_AUDIO_OUT_H
#define _USB_DEVICE_AUDIO_OUT_H

typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

typedef struct {
    u8 mic_start;

    u8 mute;
    u8 sample_rate;
    u8 channel;
    u16 samples;
    audio_callback_t callback;
} usb_audio_out_cfg_t;

typedef struct {
    u8 vol;
    u8 cur_dev;
    u8 db_level;
    u8 db_level_l;
    u8 db_level_r;
    u8 dev_change;
    u8 rw_sta;
    u32 run_tick;
} spk_ude_t;

extern spk_ude_t spk_ude;

void usb_audio_out_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);

///外部变量及api
extern uint8_t cfg_wireless_full_duplex;

#define  wireless_mic_full_duplex()    cfg_wireless_full_duplex

#endif //_USB_DEVICE_USB_AUDIO_OUT_H


