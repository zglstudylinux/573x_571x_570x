#ifndef __FUNC_ADAPTER_H
#define __FUNC_ADAPTER_H


void func_adapter(void);

void func_adapter_message(u16 msg);
u8 adapter_usb_init_en(void);

void usb_audio_out_init(u8 sample_rate, u16 samples, u8 channel);
void usb_audio_out_audio_output_callback_set(audio_callback_t callback);

void ude_set_spk_vol_cb(void);
void ude_set_spk_volume_db_l_r(void);

#endif
