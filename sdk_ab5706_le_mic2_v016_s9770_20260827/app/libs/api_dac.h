#ifndef _API_DAC_H_
#define _API_DAC_H_

#define PCM_OUT_24BITS              BIT(0)
#define PCM_OUT_MONO                BIT(1)

void adpll_init(u8 out48k_spr);
void adpll_spr_set(u8 out48k_spr);
void adpll_disable(void);

//dac
void dac_cb_init(u32 dac_cfg, u8 unmute_delay);
void dac_power_on(void);
void dac_restart(void);
void dac_power_off(void);
bool dac_get_pwr_sta(void);
void dac_mono_init(bool dual_en, bool lr_sel);              //when dual_en=1, select select left and right mixed
                                                            //                lr_sel is invalid
                                                            //when dual_en=0, lr_sel=1 select left channel
                                                            //                lr_sel=0 select right channel
void dac_channel_enable(void);                              //enable dac output channel
void dac_channel_disable(void);                             //disable dac output channel, for power save
void dac_ang_gain_set(u8 gain);
void dac_clk_source_sel(u32 val);
uint8_t dac_fade_process(void);

void dac_dump_vol(void);                                    //dump all volume regs, for debug
void dac_src_vol_set(uint src_idx, u32 vol);
bool dac_src_fade_in(uint src_idx);
bool dac_src_fade_out(uint src_idx);
void dac_src_fade_wait(uint8_t src_idx);
void dac_src_w4_empty(int src_idx);

bool aubuf_dma_is_busy(u8 index);

void dac_set_dual_mix(void);                                //设置dac左右声道输出相同的声音(l+r)/2
void dac_set_dual_mix_inverse(void);                        //设置dac伪差分输出,左声道输出声音(l+r)/2,右声道输出声音-(l+r)/2

//dac0
#define dac_vol_set(vol)        dac_src_vol_set(0, vol)     //dac0 set volume(0~0x7fff)
#define dac_fade_out()          dac_src_fade_out(0)         //dac0 fade out
#define dac_fade_in()           dac_src_fade_in(0)          //dac0 fade in
#define dac_fade_wait()         dac_src_fade_wait(0)        //dac0 fade wait
#define dac_aubuf_w4_empty()    dac_src_w4_empty(0)         //dac0 wait for obuf empty
#define dac_phase_set(phase)    PHASECOMP0 = (phase&0x00ffffff)
bool dac_is_fade_in(void);                                  //dac0 get fade in state
void dac_vol_set_reduce(u16 percent);

void dac_spr_set(uint spr);                                 //dac0 set sample rate
void dac_aubuf_init(void);                                  //dac0 aubuf init
void dac_aubuf_clr(void);                                   //dac0 clear aubuf
void dac_put_zero(uint samples);                            //dac0 put some samples to aubuf, value is zero
void dac_put_sample_16bit(s16 left, s16 right);             //dac0 put one sample(16bit) to aubuf, value is left & right
void dac_put_sample_16bit_w(s16 left, s16 right);           //dac0 put one sample(16bit) to aubuf, wait if aubuf is full
void dac_balance_set(u16 l_vol, u16 r_vol);                 //dac0 left & right volume balance(0~0x7fff)
void aubuf0_dma_init(void);
void aubuf0_dma_exit(void);
void aubuf0_dma_kick(void *ptr, u32 samples, uint nch);
void aubuf0_dma_w4_done(void);
#define aubuf0_dma_is_busy()    aubuf_dma_is_busy(0)

//dac1(dac1混合到dac0输出，可独立控制音量)
#define dac1_vol_set(vol)       dac_src_vol_set(1, vol)     //dac1 set volume(0~0x7fff)
#define dac1_fade_out()         dac_src_fade_out(1)         //dac1 fade out
#define dac1_fade_in()          dac_src_fade_in(1)          //dac1 fade in
#define dac1_fade_wait()        dac_src_fade_wait(1)        //dac1 fade wait
#define dac1_aubuf_w4_empty()   dac_src_w4_empty(1)         //dac1 wait for obuf empty
#define dac1_phase_set(phase)   PHASECOMP1 = (phase & 0x00ffffff)
void dac1_spr_set(uint spr);                                //dac1 set sample rate
void dac1_aubuf_init(void);                                 //dac1 aubuf init
void dac1_aubuf_clr(void);                                  //dac1 clear aubuf
void dac1_put_zero(uint samples);                           //dac1 put some samples to aubuf, value is zero
void dac1_put_sample_16bit(s16 left, s16 right);            //dac1 put one sample(16bit) to aubuf, value is left & right
void aubuf1_dma_init(void);
void aubuf1_dma_exit(void);
void aubuf1_dma_kick(void *ptr, u32 samples, uint nch);
void aubuf1_dma_w4_done(void);
#define aubuf1_dma_is_busy()    aubuf_dma_is_busy(1)

//dac2(dac2混合到dac0输出，可独立控制音量)
#define dac2_vol_set(vol)       dac_src_vol_set(2, vol)     //dac2 set volume(0~0x7fff)
#define dac2_fade_out()         dac_src_fade_out(2)         //dac2 fade out
#define dac2_fade_in()          dac_src_fade_in(2)          //dac2 fade in
#define dac2_fade_wait()        dac_src_fade_wait(2)        //dac2 fade wait
#define dac2_aubuf_w4_empty()   dac_src_w4_empty(2)         //dac2 wait for obuf empty
void dac2_spr_set(uint spr);                                //dac2 set sample rate
void dac2_aubuf_init(void);                                 //dac2 aubuf init
void dac2_aubuf_clr(void);                                  //dac2 clear aubuf
void dac2_put_zero(uint samples);                           //dac2 put some samples to aubuf, value is zero
void aubuf2_dma_init(void);
void aubuf2_dma_exit(void);
void aubuf2_dma_kick(void *ptr, u32 samples, uint nch);
void aubuf2_dma_w4_done(void);
#define aubuf2_dma_is_busy()    aubuf_dma_is_busy(2)

//dnr
void dac_dnr_detect(void);
u16 dac_pcm_pow_calc(void);


#endif
