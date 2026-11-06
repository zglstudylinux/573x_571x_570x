#ifndef _API_EFFECTS_H
#define _API_EFFECTS_H

typedef struct {
    const u8* dbb_param;
    u32 param_len;
    s32* coef_l;
    s32* coef_r;
    u8 dac_band_cnt;
} dbb_param_cb_t;

typedef struct {
    u32 cutoff_frequency;
    u32 intensity_set;
    u8 vbass_high_frequency_set;
    u32 intensity;
    u32 pregain;
} vbass_param_cb_t;

typedef struct {
    const s32* dyeq_param;
    const s32* coef_param;
    u8 vbass_en;
} dyeq_param_cb_t;

typedef struct {
    u16 fade_step;
    volatile s8 fade_dir;
    u16 gain;
    volatile u16 target_gain;
} soft_vol_t;

typedef struct {
	s32 wave_type;
	s32 intensity;
	s32 plfsr_l;
	s32 plfsr_r;
} ABP_generation_init_cb_t;

u8 codecs_pcm_is_start(void);

///软件音量
void soft_vol_process_mono_24bits_one_sample(soft_vol_t* p, s32* input);
void soft_vol_process_stereo_24bits_one_sample(soft_vol_t* p, s32* inputl, s32* inputr);
void soft_vol_process_mono_one_sample(soft_vol_t* p, s16* input);
void soft_vol_process_stereo_one_sample(soft_vol_t* p, s16* inputl, s16* inputr);
void soft_vol_set_vol_param(soft_vol_t* p, u16 vol, u8 vol_direct_set);
void soft_vol_set_fade_step(soft_vol_t* p, u16 step);
void soft_vol_init(soft_vol_t* p);

///动态低音音效
int music_dbb_init(dbb_param_cb_t* p);
int music_dbb_update_param(u8 vol_level, u8 bass_level);
int music_dbb_stop(void);
u8 music_dbb_get_bass_level(void);
u8 music_dbb_get_level_cnt(void);

///空间音效
int v3d_get_data_ptr_l1(u32* ptr);
void v3d_frame_process(s32* inbuf_l, s32* inbuf_r, u32 samples, u32 in_nch);
void v3d_clear_cache(void);
void v3d_delay_buf_clear_cache(void);
void v3d_set_param(int mode, u16 wet, u16 dry);
bool v3d_delay_buf_init(void* ptr_l, u32 buf_l_size, u32 delay_l_samples, void* ptr_r, u32 buf_r_size, u32 delay_r_samples);
void v3d_init(void);

///虚拟低音
void vbass_mono_process(void *cb, void *buf, u32 samples, u32 in_24bits);
void vbass_stereo_process(void *cb_l, void *cb_r, void *buf, u32 samples, u32 in_24bits);
void vbass_init(void *cb, vbass_param_cb_t *p);
void vbass_set_param(void *cb, u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set, u32 pregain);

///舒适噪声（alpha波、beta波、pink）
void alpha_beta_pink_generation_init(ABP_generation_init_cb_t *p);
void alpha_beta_pink_generation(s16 *samples, s32 len, u8 idx);
void alpha_beta_pink_generation_stereo(s16 *samples, s32 len);

#endif // _API_EFFECTS_H
