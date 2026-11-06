#ifndef __ECHO_API_H
#define __ECHO_API_H
///------------------------------------------------------------------------------------------
///ECHO 算法结构体以及相关API声明
//typedef struct {
//	u16 wet;
//	u16 dry;
//	u16 attenuation;
//	s32 delay_length;
//	s32 index;
//	s32 inter_val0;
//	s32 inter_val1;
//	s32 inter_val2;
//	u8  flag;
//	u8  filter_en;
//    u8  skip_flag;
//	//s16 delay_rbuf[DELAY_BUF_LENGTH];
//} echo_cb_t;
//
//void iir2_filter_init(s32 cutoffFreq_val);
//void echo_init_mode(u8 sample_rate, s16 *delay_buf, u16 delay_buf_len, u16 max_delay_len, u8 skip_sta);
//void echo_process(s16 *ldata);
//void echo_set_param(echo_cb_t *set_param);
//void echo_clear_buf(void);

/*初始化参数结构体*/
typedef struct {
    u8   skip_flag;			/*开启后相同buf最大delay可多一倍*/
	u8   lp_filter_en;       /*滤波器使能*/
	u8   attenuation_set;    /*1-9 ->10percent-90percent*/
	u16  delay_set;          /*1-500ms*/
    u16  dry;
    u16  wet;
    s16 *delay_lbuf_set;
    u32  cutoffFreq_set;     /*截至频率选择*/
} echo_init_t;

void echo_init_16bit(echo_init_t *p);
void echo_update_param_16bit(echo_init_t *p, u8 fade_en);
void echo_process_16bit(s16 *ldata);

///------------------------------------------------------------------------------------------

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    u16 max_delay_len;
    u16 delay_len_step;
    u16 delay_level;
    u16 attenuation_level;
    audio_callback_t callback;
} echo_cfg_t;

void echo_audio_input(u8 *ptr, u32 samples, int ch_mode, void *param);
void echo_audio_output_callback_set(audio_callback_t callback);
void echo_audio_process(s16 *ptr, u32 samples);
void echo_audio_mute_set(uint8_t mute);
void echo_audio_init(u8 sample_rate, u16 samples, u8 channel);

/// attenuation:    混响次数  0~90
/// delay_length:   每一声混响之间的间隔  0~400
/// cutoffFreq_set: 低通滤波器的截止频率  1~4000
/// lp_filter_en:   低通滤波器的开关      0：关 1：开
/// dry_set:        原始声音               0~49152（Q15即0~1.5）
/// wet_set:        回声湿度，越大所占比例越大  0~32768（Q15 即0~1.0）
/// 默认配置 echo_audio_param_set(55,250,1000,1,32768,15000)
void echo_audio_param_set(u16 attenuation ,s32 delay_length ,u32 cutoffFreq_set ,u32 lp_filter_en , u16 dry_set ,u16 wet_set);

void echo_delay_level_change(void);
void echo_delay_level_up(void);
void echo_delay_level_down(void);
void echo_delay_level_set(u8 delay_len_level);
u8 echo_delay_level_get(void);
bool echo_delay_level_is_max_min(void);

void echo_attenuation_set(u16 attenuation);
void echo_attenuation_level_set(u8 attenuation_level);
void echo_attenuation_level_change(void);
#endif
