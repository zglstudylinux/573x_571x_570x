#ifndef _API_SDADC_H
#define _API_SDADC_H

#define CHANNEL_L           0x00ff
#define CHANNEL_R           0xff00

#define ADC2DAC_EN          0x01        //ADC-->DAC
#define ADC2SRC_EN          0x02        //ADC-->SRC
#define ADC2IRQ_EN          0x03        //ADC-->IRQ（测试用）
#define ADC2DIR_EN          0x04        //ADC-->DAC（测试用）
#define ADC2SANC_EN         0x05        //ADC-->SANC
#define ADC2ASR_EN          0x06        //ADC-->ASR

typedef void (*pcm_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *params);

typedef struct {
    u16 channel;
    u8 sample_rate;
    u32 anl_gain;
    u32 dig_gain;
    u8 bits_mode;                       //ADC BITS选择；0: 24bits, 1: 16bits, 0xff: 跟随DAC的BIT MODE
    u8 out_ctrl;
    u16 samples;
    pcm_callback_t callback;
} sdadc_cfg_t;

enum {
    SPR_48000,
    SPR_44100,
    SPR_38000,
    SPR_32000,
    SPR_24000,
    SPR_22050,
    SPR_16000,
    SPR_12000,
    SPR_11025,
    SPR_8000,
    SPR_6000,
    SPR_4000,

    SPR_96000,
    SPR_88200,
    SPR_76000,
    SPR_64000,
    SPR_384000,
    SPR_352800,
    SPR_192000,
    SPR_176400,
};

void set_mic_analog_gain(u16 level, u16 str_ch);        //0~23(共24级), step 3DB (-6db ~ +63db)
void sdadc_set_digital_gain(u16 ch, u16 gain);
void sdadc_set_soft_gain(u16 ch, u32 gain);
void set_aux_analog_vol(u8 level, u8 auxlr_sel);
void sdadc_dummy(u8 *ptr, u32 samples, int ch_mode);
void sdadc_var_init(void);

int sdadc_init(const sdadc_cfg_t *p_cfg, u8 *sdadc_buf);
int sdadc_start(u16 channel);
int sdadc_exit(u16 channel);

//mic
void mic_mute(void);
void mic_unmute(void);


#define ANC_FIX_BIT         23
#define EQ_GAIN_MAX         0x07eca9cd      //24dB
#define EQ_GAIN_MIN         0x00081385      //-24dB

void ttp_init(u32 param);
void ttp_exit(void);

void sdadc_pdm_mic_init(u8 mapping);
void sdadc_pdm_mic_exit(u8 mapping);
void lefmic_zero_detect_do(u8 *ptr, u32 samples);

bool sdadcl_set_soft_eq_by_res(u32 addr, u32 len);
bool sdadcr_set_soft_eq_by_res(u32 addr, u32 len);

#endif //_API_SDADC_H
