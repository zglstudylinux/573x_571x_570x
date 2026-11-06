#ifndef _API_EQ_H
#define _API_EQ_H


//EQ
void eq_var_init(void);
void eq_coef_cal(void *eq_coef, int gain);
void bass_treble_coef_cal(void *eq_coef, int gain, int mode);                //gain:-12dB~12dB, mode:0(bass), 1(treble)
void music_bass_treble_enable(uint8_t enable);    // 0:disable 1:enable
//dac eq/drc
bool music_eq_set_by_res(u32 addr, u32 len);
void music_eq_set_by_param(u8 band_cnt, const u32 *eq_param);
void music_eq_set_by_num(u8 num);
void music_eq_set_gain(u32 gain);
bool music_eq_is_done(void);        //判断上一次设置EQ是否完成，1：已完成
bool music_eq_divband_init(u8 ch, u8 div_band, u32 *param);
void music_eq_divband_exit(void);
void music_eq_off(void);
bool music_drc_set_by_res(u32 addr, u32 len);
void music_drc_set_by_param(u8 band_cnt, const u32 *drc_param);
void music_drc_on(void);            //使能music drc
void music_drc_off(void);           //关闭music drc

#endif
