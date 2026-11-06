#ifndef __RECORD_H
#define __RECORD_H

bool is_mic_record_dir(char *sfn);
bool is_fm_record_dir(char *sfn);
bool is_aux_record_dir(char *sfn);
bool is_bt_record_dir(char *sfn);
bool is_record_dir(char *sfn);

void bsp_opus_rec_mic_start(void);
void bsp_opus_rec_mic_stop(void);
bool bsp_opus_rec_is_working(void);
u8 bsp_get_opus_param_packetsize(void);
u8 bsp_get_opus_param_nchannel(void);
u16 bsp_get_opus_data(u8* buf, u16 len);
void opus_sdadc_process(u8 *ptr, u32 samples, u32 pcm_mode);
#endif //__RECORD_H
