#ifndef _API_EQ_H
#define _API_EQ_H

#define EQ_CHL          BIT(0)
#define EQ_CHR          BIT(1)

typedef struct {
    u8 band_cnt;
    u8 version;
    u8 sample_rate;
    u8 rfu[1];
    const u32 *param;
} eq_param_t;

typedef struct {
    u8  band_cnt;
    u8  version;
    u8  sample_rate;
    u8  rfu[1];
    u32 param[20];                                                  //硬件DRC输入参数
} drc_param_t;

//EQ
#define EQ_MAX_BANK                 20
void eq_var_init(void);
void eq_coef_cal(void *eq_coef, int gain);
void bass_treble_coef_cal(void *eq_coef, int gain, int mode);                //gain:-12dB~12dB, mode:0(bass), 1(treble)
void drc_param_format(drc_param_t *drc, u8 *ptr);

//dac eq/drc
bool music_eq_set_by_res(u32 addr, u32 len);
void music_eq_set_by_param(u8 ch, u8 band_cnt, const u32 *eq_param);        //ch: L:BIT(0) R:BIT(1) L+R:0/3
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
