/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

uint32_t cfg_sdk_version = SDK_VERSION;

//#if !FUNC_USBDEV_EN
#if (!(ADAPTER_USB_MIC_RX_EN || ADAPTER_USB_SPK_EN))
void usb_dev_isr(void){}
void ude_ep_reset(void){}
void ude_control_flow(void){}
void ude_isoc_tx_process(void){}
void ude_isoc_rx_process(void){}
void uda_set_balance_vol(u8 vol_l, u8 vol_r){}
void uds_process_cb(u8 type){}
void ude_set_sys_mute(u8 sys_mute){}
AT(.usbdev.com)
void ude_set_sys_volume(u8 vol){}
#endif

#if !FUNC_REC_EN
bool puts_rec_encbuf(u8 *buf, u16 len) {return false;}
bool gets_rec_obuf(u8 *buf, u16 len) {return false;}
#endif
#if (REC_TYPE_SEL != REC_MP3)
int mpa_encode_frame(void) {return 0;}
#endif //(REC_TYPE_SEL != REC_MP3)


#if (REC_TYPE_SEL != REC_ADPCM && !BT_HFP_REC_EN)
void adpcm_encode_process(void){}
#endif //(REC_TYPE_SEL != REC_ADPCM)

#if !MUSIC_WAV_SUPPORT
int wav_dec_init(void){return 0;}
bool wav_dec_frame(void){return false;}
void load_code_wavdec(void){}
int wav_decode_init(void){return 0;}
#endif // MUSIC_WAV_SUPPORT

#if !MUSIC_WMA_SUPPORT
int wma_dec_init(void){return 0;}
bool wma_dec_frame(void){return false;}
void load_code_wmadec(void){}
int wma_decode_init(void){return 0;}
#endif // MUSIC_WMA_SUPPORT

#if !MUSIC_APE_SUPPORT
int ape_dec_init(void){return 0;}
bool ape_dec_frame(void){return false;}
void load_code_apedec(void){}
int ape_decode_init(void){return 0;}
#else
AT(.com_text.weak.dac)
void obuf_put_one_sample(s32 left, s32 right)
{
    dac_put_sample_16bit(left, right);
}
#endif // MUSIC_APE_SUPPORT

#if !MUSIC_FLAC_SUPPORT
int flac_dec_init(void){return 0;}
bool flac_dec_frame(void){return false;}
void load_code_flacdec(void){}
int flac_decode_init(void){return 0;}
#endif // MUSIC_FLAC_SUPPORT

#if !MUSIC_SBC_SUPPORT
int sbcio_dec_init(void){return 0;}
bool sbcio_dec_frame(void){return false;}
int sbcio_decode_init(void){return 0;}

AT(.sbcdec.code)
void codec_sbcdec_update(void){}
AT(.sbcdec.code)
void codec_sbcdec_init(void *param){}
#endif // MUSIC_SBC_SUPPORT

#if !MUSIC_AAC_SUPPORT
int aacio_dec_init(void) {return 0;}
int aacio_decode_init(void) {return 0;}
#endif

#if !MUSIC_M4A_SUPPORT
int m4a_decode_init(void){return 0;}
int m4a_dec_init(void){return 0;}
bool aacio_sub_process(void){return true;}
#endif

#if !MUSIC_AAC_SUPPORT && !MUSIC_M4A_SUPPORT
bool aacio_dec_frame(void) {return false;}
#endif

#if (!(MUSIC_WMA_SUPPORT | MUSIC_APE_SUPPORT | MUSIC_FLAC_SUPPORT))
void msc_stream_start(u8 *ptr) {}
void msc_stream_end(void) {}
int msc_stream_read(void *buf, unsigned int size) {return 0;}
bool msc_stream_seek(unsigned int ofs, int whence) {return false;}
void os_stream_fill(void) {}
void os_stream_read(void) {}
void os_stream_seek(void) {}
void os_stream_end(void) {}
AT(.com_text.weak.stream)
void msc_stream_fill(void) {}
#endif

void auxr_analog_init(u8 str_ch, u8 gain) {}
void auxl_analog_init(u8 str_ch, u8 gain) {}
void auxr_analog_exit(u8 str_ch) {}
void auxl_analog_exit(u8 str_ch) {}
void aux_analog_init(u8 adc_ch, u8 str_ch, u8 gain_level) {}
void aux_analog_exit(u8 adc_ch, u8 str_ch) {}

int mp3_dec_init(void) {return 0;}
int mp3_decode_init(void){return 0;}
int music_decode_init(void){return 0;}
void mp3_get_total_time(void){}
void update_codec_playtime_callback(void *s){}
int mad_layer_I(void) { return 0; }
int mad_layer_II(void) { return 0; }
bool mp3_save_point(void *frame) {return false;}
void mp3_qskip_start(void *frame) {}
bool mp3_qskip_process(void *frame) {return false;}
AT(.text.qskip)
void codec_qskip_init(u32 block_start, u16 block_align) {}
AT(.text.weak.qskip)
void music_qskip(bool direct, u8 second) {}
AT(.text.weak.qskip)
void music_qskip_keep(void) {}
AT(.text.weak.qskip)
void calc_qskip_target_offset(void) {}
AT(.text.weak.qskip)
void music_qskip_end(void) {}
AT(.text.weak.qskip)
void music_set_jump(void *brkpt) {}
AT(.text.weak.qskip)
void music_get_breakpiont(void *brkpt) {}

#if 1
void msc_pcm_out_var_init(void) {}
void msc_pcm_out_start(void) {}
void msc_pcm_out_stop(void) {}
u8 codecs_pcm_is_start(void) {return 0;}
AT(.com_text.weak.codecs.pcm)
void msc_pcm_effect_process(u8 *buf, u32 samples, u32 nch, u32 is_24bit, u32 pcm_info) {}
AT(.com_text.weak.codecs.pcm)
void msc_pcm_out_24bit_process(s32 left, s32 right) {}
AT(.com_text.weak.codecs.pcm)
void msc_pcm_out_16bit_process(s32 left, s32 right) {}

void mp3_pcm_out_start(void) {}
AT(.mp3dec.pcm)
void mp3_pcm_out_process(u32 samples) {}
AT(.sbcdec.pcm)
void sbc_pcm_out_start(void) {}

AT(.aacdec.pcm)
void aac_pcm_out_start(void) {}
AT(.aacdec.pcm)
void aac_pcm_out_process(u32 samples, bool is_tws) {}
AT(.aacdec.pcm.clear)
void aac_tws_obuf_clear(void) {}

AT(.wavdec.code)
void obuf_put_16bit_sample_wav(s32 left, s32 right)
{
    dac_put_sample_16bit_w(left, right);
}
AT(.wavdec.code)
void obuf_put_24bit_sample_wav(s32 left, s32 right)
{
    dac_put_sample_16bit_w(left>>8, right>>8);
}
AT(.apedec.code)
void obuf_put_one_sample_wma(s32 left, s32 right)
{
    dac_put_sample_16bit_w(left, right);
}
AT(.flacdec.code)
void obuf_put_one_sample_flac(s32 left, s32 right)
{
    dac_put_sample_16bit_w(left, right);
}
AT(.apedec.code)
void obuf_put_16bit_sample_ape(s32 left, s32 right)
{
    dac_put_sample_16bit_w(left, right);
}
#endif

#if !BT_MUSIC_EFFECT_DBB_EN
u8 music_dbb_get_level_cnt(void) {return 0;}
u8 music_dbb_get_bass_level(void) {return 0;}
int music_dbb_stop(void) {return 0;}
int music_dbb_update_param(u8 vol_level, u8 bass_level) {return 0;}
int music_dbb_init(dbb_param_cb_t* p) {return 0;}
#endif

#if !BT_MUSIC_EFFECT_SPATIAL_AU_EN
void v3d_frame_process(u32* buf, u32 samples, u32 in_nch, u32 in_24bits) {}
void v3d_clear_cache(void) {}
void v3d_set_param(int mode, u16 wet, u16 dry) {}
void v3d_init(void) {}
#endif

#if !BT_MUSIC_EFFECT_VBASS_EN && !BT_MUSIC_EFFECT_DYEQ_VBASS_EN
int vbass_process(void *cb, s16 *data) {return 0;}
void vbass_init(void *cb, vbass_param_cb_t *p) {}
void vbass_set_param(void *cb, u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set) {}
#endif

#if !BT_MUSIC_EFFECT_DYEQ_EN
void dynamic_eq_process(void *cb, s32 *samples) {}
void dyeq_init(void *cb, dyeq_param_cb_t *p) {}
void dyeq_coef_update(void *cb, u8 *buf) {}
#endif

#if !BT_MUSIC_EFFECT_XDRC_EN
s32 xdrc_softeq_proc(void *cb, s32 input) {return 0;}
u32 xdrc_softeq_cb_init(void *cb, u32 cb_len, u32 res_addr, u32 res_len) {return 0;}
void xdrc_softeq_coef_update(void *cb, u32 cb_len, u32 *coef, u32 band, bool pregain) {}
s32 xdrc_drc_process_gain(void *cb, s32 data_in) {return 0;}
s32 xdrc_drc_get_pcm32_s(s32 data, int g) {return 0;}
s16 xdrc_drc_process_s16(void *cb, s32 data_in) {return 0;}
void xdrc_drcv1_cb_init(void *cb, u32 cb_len, u32 res_addr, u32 res_len) {}
void xdrc_drcv1_coef_update(void* cb, u32 *tbl) {}
s32 pcmdelay_mono_s32(void *cb, s32 pcm) {return 0;}
bool pcmdelay_init(void *cb,void *delay_buf, u16 delay_buf_len, u16 samples_size, u16 delay_samples) {return false;}
bool pcmdelay_coef_update(void *cb, u16 delay_samples) {return false;}
#endif


#if !BT_A2DP_LHDC_AUDIO_EN
void bt_lhdc_dec_init(void) {}
void lhdc_decode_init(u16 codec_id, u8 *value){}
bool lhdc_dec_init(u16 codec_id, u8 *value) {return false;}
bool lhdc_dec_frame(void) {return false;}
int lhdc_decode_frame(bool is_tws){return 0;}
void lhdc_dec_destory(void){}

void au0_dmain_start(void){}
void au0_dmain_stop(void){}
void au0_dmain_kick(s32 *ptr, u32 samples){}
AT(.com_text.weak.au0dma)
void au0_dmain_isr(void){}
void au0_dmain_sem_init(void){}

void spiflash_security_uid_read(void){}

void lhdc_obuf_tws_cpy(void) {}
void lhdc_fill_tws_obuf(void) {}
void lhdc_cpy_tws_obuf(void) {}
AT(.text.lhdc.weak.sbc.play)
void lhdc_dec_tick_proc(uint32_t ticks){}
u8 avdtp_tws_get_lhdc_spr(uint16_t codec_id, uint8_t spec_val){return 0x01;}
bool lhdc_nor_dec_frame(void){return false;}
bool lhdc_tws_dec_frame(void){return false;}

void lhdc_pcm_out_start(void);
void lhdc_pcm_out_process(u8 *buf, u32 samples, u32 nch, u32 in_24bits, bool is_tws) {}
void gpdma_lhdc_kick(u32 *ptr, u32 samples, u32 in_24bits) {}
void lhdc_kick_copy_tws_obuf(void){}
void lhdc_gpdma_done(void) {}
bool bt_decode_is_lhdc(void) {return false;}
#else
size_t lhdc_cache_read_do(uint8_t *buf, uint max_size);
void lhdc_cache_free_do(void);

AT(.text.lhdc.dec.cache)
void lhdc_cache_free(void) {
}
AT(.text.lhdc.dec.cache)
size_t lhdc_cache_read(uint8_t *buf, uint max_size) {
    return 0;
}
#endif

#if !FMRX_REC_EN
void fmrx_rec_start(void){}
void fmrx_rec_stop(void){}
#endif // FMRX_REC_EN

#if !USB_SUPPORT_EN
void usb_isr(void){}
void usb_init(void){}
#endif

#if ((!SD_SUPPORT_EN) && (!FUNC_USBDEV_EN))
void sd_disk_init(void){}
void sdctl_isr(void){}
void sd_disk_switch(u8 index){}
bool sd0_stop(bool type){return false;}

bool sd0_init(void){return false;}
bool sd0_read(void *buf, u32 lba){return false;}
bool sd0_write(void* buf, u32 lba){return false;}

#endif

#if !MUSIC_UDISK_EN && !MUSIC_SDCARD_EN
u32 fs_get_file_size(void){return 0;}
void fs_save_file_info(unsigned char *buf){}
void fs_load_file_info(unsigned char *buf){}
#endif

void karaok_process(void){}
AT(.com_text.weak.karaok)
void karaok_sdadc_process(u8 *ptr, u32 samples, int ch_mode){}

s16 *karaok_echo_get_buf(u32 buf_num) {return NULL;}
AT(.com_text.weak.karaok)
void karaok_sdadc_echo_process(s16 *rptr){}
void echo_reset_buf(void *cfg){}
void echo_set_delay(u16 delay){}
void echo_set_level(const u16 *vol){}

#if !SYS_MAGIC_VOICE_EN
AT(.com_text.weak.karaok)
void karaok_sdadc_magic_process(s16 *rptr){}
void magic_voice_process(void) {}
void mav_kick_start(void) {}
#endif


#if !FUNC_SPDIF_EN
void spdif_pcm_process(void){}
bool spdif_smprate_detect(void) {    return false;}
AT(.com_text.weak.isr.spdif)
void spdif_isr(u32 spfrx_pnd){}
#endif

#if !FUNC_SPDIF_TX_EN
AT(.com_text.weak.isr.spdif)
void spdif_tx_isr(u32 spftx_pnd) {}
#endif


FRESULT fs_open(const char *path, u8 mode){return 0;}
FRESULT fs_read (void* buff, UINT btr, UINT* br){return 0;}
FRESULT fs_lseek (DWORD ofs, u8 whence){return 0;}

#if !KARAOK_REC_EN
AT(.com_text.weak.karaok.rec)
bool karaok_rec_fill_buf(u8 *buf, u16 len) {return false;}
#endif

#if !I2S_DMA_EN
void i2s_isr(void) {}
void i2s_process(void) {}
#endif

void voice_assistant_enc_process(void);
AT(.com_text.weak.opus)
void opus_enc_process(void) {}

void qtest_var_init(void) {}
AT(.com_text.weak.qtest)
void qtest_only_pair(void) {}
AT(.com_text.weak.qtest)
u8 qtest_get_mode(void) {return 0;}
bool qtest_is_send_btmsg(void) {return false;}
AT(.com_text.weak.bsp.uart.vusb)
void qtest_packet_huart_recv(u8 *rx_buf){};
AT(.com_text.weak.bsp.uart.vusb)
void qtest_packet_uart1_recv(u8 data);
void qcheck_save_checkcode(u32 checkcode) {}

#if (UART0_PRINTF_SEL == PRINTF_NONE)
void wdt_irq_init(void) {}
#endif

#if SYS_SLEEP_LEVEL > 2
void sys_sleep_restore(void);
AT(.com_text.weak.ret.bb)
void nanos_sleep_restore(void)
{
    sys_sleep_restore();
}

AT(.sleep_text.sleep.cb)
void sys_sleep_proc_cb(u8 lpclk_type)
{
    sys_sleep_proc_lv(lpclk_type, 0x84);
}
#endif

#if !TKEY_EN
AT(.com_text.weak.tkey.isr)
void tkey_isr(void) {}
#endif

#if (USB_DET_VER_SEL == 1) //usb检测新拔出方式不用这种操作
AT(.com_text.weak.usb.dectect)
void usbchk_switch_otg_device(void) {}
void usbchk_switch_otg_host(void) {}
void usbchk_only_host(void) {}
void usbchk_only_device(void) {}
u8 usbchk_connect(u8 mode) {return 0;}
#else
AT(.com_text.weak.usb.dectect)
u8 usb_connect(void) {return 0;}
#endif

//不够flash空间时可去掉差分或VCMBUF模式
#if DAC_DIFF_DIS
void dac_diff_ang_power_on(u32 restart) { printk("diff error\n");}
void dac_diff_power_off(void) {}
#endif
#if DAC_VCMBUF_DIS
void dac_vcmbuf_ang_power_on(u32 restart) { printk("vcmbuf error\n");}
void dac_vcmbuf_power_off(void) {}
#endif

#if !FOT_EN && !AB_MATE_APP_EN
u8 fot_checksum_cal(u8 *buf) {return 0;}
#endif

#if !EFFECT_DBG_ADJUST_EN
void music_effect_set_params(u8 sample_rate, u16 samples){}
#endif

#if !ADAPTER_MIC_AINS4_EN && !WIRELESS_MIC_AINS4_EN
void ains4_mic_proc_cb(void){}
#endif


#if WARNING_TONE_EN
WEAK void *sbcdec_init(u8 sbc_type) {return NULL;}
WEAK int sbc_decode(void *sbc, const void *input, size_t input_len) {return 0;}
WEAK bool msbc_encode_init(void) {return false;}
WEAK u8 *msbc_enc_get_obuf(void) {return NULL;}
WEAK void sbc_unpack_reset(void *sbc) {}
WEAK void msbc_enc_frame(s16 *inbuf) {}
WEAK bool wsbc_play_init(u16 *sample_rate, u8 *frame_size) {return 0;}
WEAK uint wsbc_play_proc(u8 *input, s16 *obuf, uint frame_size) {return 0;}

void func_bt_set_dac(u8 enable){}
uint bsp_bt_get_hfp_vol(uint hfp_vol){ return 0;}
uint32_t tws_time_tickn_get(void){ return 0;}
bool tws_time_tickn_expire(uint32_t tickn){return true;}
bool sco_is_bypass(void){return false;}
#endif

#if !WARNING_TONE_EN
int spi_stream_read(void *buf, unsigned int size){return 0;}
bool spi_stream_seek(unsigned int ofs, int whence){return false;}
void spi_save_file_info(unsigned char *buf){}
void spi_load_file_info(unsigned char *buf){}
bool sco_is_bypass(void){return false;}
#endif

#if !SPI_HW_EN
void set_spi1_baud(u32 baud){}
void update_spi1baud(void){}
#endif

#if FUNC_LE_DUT_EN || FUNC_LE_FCC_EN
uint8_t vs_ble_test_do(void const *cmd);
uint8_t vs_ble_test(void const *cmd) {
    return vs_ble_test_do(cmd);
}
#else
uint8_t vs_fcc_test_cmd(void const *param) {
    return 0x11;
}
#endif

u8 music_effect_get_process_flag(void){return 0;}
