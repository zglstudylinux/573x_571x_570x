#ifndef __API_ALG_H
#define __API_ALG_H
///------------------------------------------------------------------------------------------
///DNN_L1 算法结构体以及相关API声明
typedef struct {
	s16 nt;
	u8  nt_post;
	s16 exp_range_H;
	s16 exp_range_L;
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	u8  wind_thr;
	u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //Ê¹ÄÜ²ÎÊý´òÓ¡
    u8  wind_level;
	u16 wind_range;
	u16 low_fre_range;
	u16 low_fre_range0;
	u8  pitch_filter_en;
	u16 mask_floor;
	u8  mask_floor_r;
	u8  music_lev;
	u8  comforN_level;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	//s32 noise_db2;
	//s32 noise_db3;
	u8  smooth_en;
	u16 gain_assign;
	u8  enlarge_v;
    s32 noise_db;
	s32 noise_db2;
	s32 noise_db3;
    s32 noise_db4;
	u8  vad_floor_en;
	u32 prior_sum_v;
	//u16 pitch_filter_range;
} dnn_cb_t;

void dnn_init(dnn_cb_t *dnn_cb);
int dnn_process(s16 *data, s32 *data_32bits, u8 in_24bits_en);
void dnn_process_ext_window(s16 *data, s32 *data_32bits, u8 in_24bits_en,s32* outbuf);
void dnn_fre_process(s32 *f, u8 data_shift, u8 in_24bits_en);
void dnn_process_ext_next(s32 *data, s32 *data_32bits, u8 in_24bits_en,s16* outbuf);
///------------------------------------------------------------------------------------------
///AINS3 算法结构体以及相关API声明
typedef struct {
    s16 nt;
    u8  prior_opt_idx;
	u8  prior_opt_ada_en;
	u8	music_lev;
	u16 music_lev_hi_range;
	u16 music_lev_hi;
	u16 ns_ps_rate;
	u8	low_fre_lev;
	u16 low_fre_range;
	u16	ns_range_l;
	u16	ns_range_h;
	s32 noise_db;
	s32 noise_db2;
	s32 noise_db3;
	u8 smooth_en;
	//u8  nr_mode;
} ains3_cb_t;

void ains3_init(ains3_cb_t *ains3_cb);
void ains3_process(s16 *data);
void ains3_mic_proc_kick_start(void);

s32 * get_fft_buf_addr(void);
void ains3_process_ext_window(s16 *data,s32* out_buf);
void nr_process_do(s32 *f);
void ains3_process_ext_output(s16 *data);
///------------------------------------------------------------------------------------------
///NR_FAR 算法结构体以及相关API声明
void nr_far_process(s16 *data);
void nr_far_init(u16 noise_thr, u16 nr_level);
///------------------------------------------------------------------------------------------
///ROOM_REVERB 算法结构体以及相关API声明
typedef struct
{
	u16 damping_set;        /*高频阻尼，去除高频部分的齿音，擦类的刺耳声*/
	u16 decay_set;          /*衰减率*/
	u16 wet;
	u16 dry;
	u32 samples;
	u8 hpl;                 /*高通滤波器等级0表示关闭,1-10:50hz-500hz,步进50hz*/
}rvb_init_sb;

void room_reverb_process(s16 *sample, u8 idx);
void reverb_buf_init(rvb_init_sb *p, u8 idx);
///------------------------------------------------------------------------------------------
///ECHO 算法结构体以及相关API声明
typedef struct {
	u16 wet;
	u16 dry;
	u16 attenuation;
	s32 delay_length;
	s32 index;
	s32 inter_val0;
	s32 inter_val1;
	s32 inter_val2;
	u8  flag;
	u8  filter_en;
	//s16 delay_rbuf[DELAY_BUF_LENGTH];
} echo_cb_t;

void iir2_filter_init(s32 cutoffFreq_val);
void echo_init_mode(u8 sample_rate, s16 *delay_buf, u16 delay_buf_len, u16 max_delay_len);
void echo_process(s16 *ldata);
void echo_set_param(echo_cb_t *set_param);
void echo_clear_buf(void);

///------------------------------------------------------------------------------------------
///REVERB 算法结构体以及相关API声明
/*plate reverb 初始化结构体*/
typedef struct
{
	u16 predelay_set;       /*预延迟*/
	u16 damping_set;        /*高频阻尼，去除高频部分的齿音，擦类的刺耳声*/
	u16 mix_set;            /*初始化干湿比*/
	u16 ALLPASS_G2_set;     /*衰减扩散，控制尾部密度*/
	u16 decay_set;          /*衰减率*/
	u16 vocals_set;         /*人声部分 值越大 人声越突出  0-65535 */
}prvb_init_sb;

void plate_buf_init(prvb_init_sb *p);
void plate_reverb_process(s16 *ldata, s16 *rdata);
///------------------------------------------------------------------------------------------
///magic 算法结构体以及相关API声明
#define CircleBufo_Len		1536//0.032*Fs

typedef struct {
//Common use
	s32 fre_shift;
	u32 Fs;
	u32 pitch_levels;
	s32 ph1;
	s32 ph2;
	s16 pWtite1;
	s16 pRead1;
	s16 pRead2;
	s16 CircleBuf[CircleBufo_Len];
} pitch_shift2_cb_t;


void pitch_shift2_init(s32 SampleRate, s32 shift, pitch_shift2_cb_t *st);
void pitch_shift2_process(s16 *data, pitch_shift2_cb_t *st);



///------------------------------------------------------------------------------------------
//plc
void plc_soft_init(u8 idx, uint pkt_len);
void plc_soft_process(s16 *int_data, u32 samples, u8 bfi, u8 idx);
void plc_soft_exit(u8 idx);

///------------------------------------------------------------------------------------------
//plc_60  16bit 60samples
void plc_soft_60_init(u8 idx, uint pkt_len);
void plc_soft_60_process(s16 *int_data, u32 samples, u8 bfi, u8 idx);
void plc_soft_60_exit(u8 idx);

///------------------------------------------------------------------------------------------
//lc3s
void lc3s_enc_init(u8 sample_rate, u16 samples);
void lc3s_dec_init(u8 sample_rate, u16 samples);
void lc3s_dec(u8 *ibuf, s16 *obuf, uint samples, uint index);
void lc3s_enc(s16 *ibuf, u8 *obuf, uint samples);
uint lc3s_get_frame_size(uint fs_in, u16 samples, u32 bitrate);

///------------------------------------------------------------------------------------------
///AINS4降噪算法结构体以及相关API声明
typedef struct {

	//s32 noise_db2;
	//s32 noise_db3;
	s32 denoiseBound;
	s32 snr_thr;
	u8  overdrive_adapt_en;
	s32 overdrive;
	u8  smooth_en;
	u8  modelUpdatePars0;
	//u8  enlarge_v;
	s16 gainHB_rd;
	u8 delta_k_up;
	//s16 denoiseBound_fix;
	u8 	yuan_en;
	s16 enr_thres;
	s16  low_fre_range;
	s16 prior_opt_idx;
	s16 prior_opt_freh;
	u8  prior_opt_ada_en;
	s16 lquantile_sm;
	s16 hi_gain_len;
	s32 factor;
	s32 quan_gap;
	s16 quan_gap_low_len;
	s32 quan_gap_low;
	s16 speech_update;
	//s16 smooth_v;
	//u8  enr_mean_max_en;
	//s32 enr_nr_thr;
	u8	spp_en;
	s16 ymin_floor;
	s16 ymin_idx;
	s16 gain_ceil;
	s32 gain_ceil_ratio;
	s16 noise_min_floor;
	//s16 smooth_logLrt;
	s16 music_lev;
	u8  noise_min_en;
	s32 ai_vad_hard_thres;
	u16 spp_vad_len;
	s32 qhat_assign;
	s32 qhat_assign_low;
	u8  spp_max_en;
	u8  low_floor_add_l;
	s16 low_mask_floor_add;
	u8  dd_od_ctl_en;
	u8  ai_vad_spp_hard_en;
	s32 spp_thr_noiseprev;
	s16 spp_thr;
	s16 prev_noise_len;
	u8  vad_low_noise_en;
	s16 vad_low_noise_p;
	u8  vad_low_max_en;
	u8  sin_keep_en;
	s32 sin_keep_thr;
	s16 sin_keep_gap;
	u8  sin_coswin_en;
	u8  sin_keep_idx_lim;
	s32 priorModelPars0;
	s16 alp1_dd;

	//u16 pitch_filter_range;
} ains4_cb_t;
void ains4_mic_proc_kick_start(void);
void ains4_init(ains4_cb_t *ains4_cb);
int ains4_process(s16 *data);

///---------------------------------------------------------------------------------------------
///AGC 算法结构体以及相关API声明
typedef struct {
	u32 sampleHz;           //语音采样率
	u8  frameMs;             //一帧语音长度，以毫秒为单位
	u16 frameSh;            //一帧short的个数， 默认一帧10ms
	u16 frameSh_true;
	s16 CompressiondB;
	s16 TargetdBfs;
	u8  limiterEnable;		//是否打开限压器
	u8  frame_2p5ms_en;
	u8	inbits24_en;
	u8 	smooth_en;
	u8  agcMode;			//AGC模式
} agc_cb_t;
void agcInit_do(agc_cb_t* agc_cb);
void AgcProcess_do(s16 *input, s32 pcmLen);
void agcInit(agc_cb_t* agc_cb);
void AgcProcess(s16 *input, s32 pcmLen);

///---------------------------------------------------------------------------------------------
///DNN_L3 算法结构体以及相关API声明
typedef struct {
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	//u8  wind_thr;
	//u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印

	u16 low_fre_range;

	u8  music_lev;

	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 denoiseBound;
	u8  smooth_en;
	u8  ap_test_en;
	u16 hi_gain_len;
	s16 sp_thres;

	//u16 pitch_filter_range;
} dnn_l3_cb_t;

void dnn_l3_init(dnn_l3_cb_t *dnn_l3_cb);
int dnn_l3_process(s16 *data);

///---------------------------------------------------------------------------------------------
///DNN_L2 算法结构体以及相关API声明
typedef struct {
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	//u8  wind_thr;
	//u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印

	u16 low_fre_range;

	u8  music_lev;

	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 denoiseBound;
	u8  smooth_en;
	u8  ap_test_en;
	u16 hi_gain_len;
	s16 sp_thres;

	//u16 pitch_filter_range;
} dnn_l2_cb_t;

void dnn_l2_init(dnn_l2_cb_t *dnn_l2_cb);
int dnn_l2_process(s16 *data);


///---------------------------------------------------------------------------------------------
///howling 算法结构体以及相关API声明
void hl_init();
int howling_process(s16* data);
u16 notch_index_ret(void);//eq 参数索引
int howling_process_ext_window(s16* data,s32* outbuf);
int howling_process_ext(s32* fft_in);


typedef struct {
	//s16 nt;
	s16 mask_floor;
    u8  param_printf;                           //使能参数打印
	s32 howling_f_thres;
	s32 howling_f_thres_hi;
	u16 howling_fre_p;
	s32 hiwling_div_thres;
	u32 std_thres;
	s16  howling_exp_papr;
	s16  howling_exp_papr_hi;
	s16  howling_exp_papr2;
	//u8  sin_post_en;
	u8  smooth_en;

	//u16 pitch_filter_range;
} howling_dnn_cb_t;
void howling_dnn_init(howling_dnn_cb_t *dnn_cb);
int howling_dnn_process(s16 *data, s32 *data_32bits, u8 in_24bits_en);
void howling_dnn_fre_process(s32 *f, u8 data_shift, u8 in_24bits_en);
void howling_dnn_process_ext_window(s16 *data, s32 *data_32bits, u8 in_24bits_en,s32* outbuf);
void howling_dnn_process_ext_next(s32 *data, s32 *data_32bits, u8 in_24bits_en,s16* outbuf);

///---------------------------------------------------------------------------------------------
///dnr fre 算法结构体以及相关API声明
typedef struct {

	//s32 noise_db2;
	//s32 noise_db3;
	s16 denoiseBound;
	s32 overdrive;
	u8  smooth_en;
	s64 enr_thres;
	s16 smooth_v;
	u8  enr_mean_max_en;
	s16 music_lev;
	s32 enr_nr_thr;
	s16 low_fre_range;
	s16 prior_opt_idx;
	s32 in_attack;
	s32 in_release;
	s32 fs;
	s32 noise_init;
	u8  noise_init_en;
	//u16 pitch_filter_range;
} dnr_fre_cb_t;

void dnr_fre_init(dnr_fre_cb_t *dnr_fre_cb);
#if 1
int dnr_fre_process(s16 *data);
void dnr_fre_fre_process(s32 *f, s16* data);
#else
int dnr_fre_process(s32 *data);
void dnr_fre_fre_process(s32 *f, s32* data);
#endif


///---------------------------------------------------------------------------------------------
///循环buffer api
typedef struct {
    volatile u16 len;
    u8 *obuf;
    u8 *wptr;
    u8 *rptr;
    u8 *init_buf;
    u8 enable;
    u16 buf_len;
} cbuf_cfg_t;

extern uint16_t cfg_wireless_d2a_dec_us;

void cbuf_init(cbuf_cfg_t *cbuf, u8 *buf, u16 buf_size);
void cbuf_input(u8 *ptr, u16 len, cbuf_cfg_t *emt);
bool cbuf_output(u8 *buf, u16 len, cbuf_cfg_t *emt);
u16  cbuf_total_len_get(cbuf_cfg_t *emt);

void cbuf_input_audio(u8 *ptr, u16 samples, cbuf_cfg_t *emt);
bool cbuf_output_audio(u8 *buf, u16 samples, cbuf_cfg_t *emt);
u16  cbuf_total_samples_get(cbuf_cfg_t *emt);

///------------------------------------------------------------------------------------------
///软件src算法结构体以及相关API声明
void src_init(u32 ch_index, u32 spr_in, u32 spr_out);
int src_frame_resample(u32 ch_index, short *src_in, short *src_out, int in_cnt);

#endif // __API_CODEC_H

