#ifndef _WIRELESS_H
#define _WIRELESS_H

#include "wireless_link_list.h"
#include "wireless_txrx.h"
#include "wireless_proc.h"
#include "wireless_cmd.h"
#include "wireless_pwr_ctr.h"
#include "wl_xcfg_param_proc.h"
#include "adapter_save_param.h"
#include "eq_drc_param_proc.h"
#include "voice_param_proc.h"
#include "key_table_param_proc.h"
#include "patch_param_proc.h"
#include "wireless_con_interact_proc.h"
//#include "../../os/os_thread.h"


#if WIRELESS_CON_COMB_BUF_EN
    #define WIRELESS_MIC_RX_BUFFER_SIZE     (WIRELESS_MIC_FRAME_SIZE*WIRELESS_MIC_COMB_NB)*WIRELESS_MIC_RETRY_NB
    #define WIRELESS_MIC_TX_BUFFER_SIZE     (WIRELESS_MIC_FRAME_SIZE*WIRELESS_MIC_COMB_NB)*WIRELESS_MIC_RETRY_NB
#else
    #define WIRELESS_MIC_RX_BUFFER_SIZE     (WIRELESS_MIC_FRAME_SIZE*WIRELESS_MIC_COMB_NB)
    #define WIRELESS_MIC_TX_BUFFER_SIZE     (WIRELESS_MIC_FRAME_SIZE*WIRELESS_MIC_COMB_NB)
#endif

#define wireless_is_sleep()             (ble_con_get_status() == 0)

void mic_enc_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void mic_enc_audio_output_callback_set(audio_callback_t callback);
void mic_enc_init(u8 sample_rate, u16 samples, u8 channel);
void mic_enc_reset(void);

void mic_dec_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params);
void mic_dec_audio_output_callback_set(audio_callback_t callback);
void mic_dec_init(u8 sample_rate, u16 samples, u8 channel);
void mic_dec_reset(u8 idx);

#if DEVICE_INTERPHONE_EN
void wireless_interphone_init(void);
void wireless_interphone_exit(u8 idx);
#endif

void wireless_init(void);
void wireless_adapter_init(void);
void wireless_device_init(void);
void wireless_device_exit(void);
void wireless_adapter_exit(u8 idx, u8 con_sta);
void wireless_channel_status(u8 idx, u8 chidx, s8 rssi, u8 bfi, u8 chstatus);

//lc3b, 1.25ms
void lc3b_dec(u8 *ibuf, s16 *obuf, uint samples, uint index);
void lc3b_dec_init(u8 sample_rate, u16 samples);
void lc3b_dec_exit(uint index);
void lc3b_enc_init(u8 sample_rate, u16 samples);
void lc3b_enc(void *ibuf, u8 *obuf, uint samples);
void lc3b_enc_exit(void);


//lc3s
void lc3s_enc_init(u8 sample_rate, u16 samples);
void lc3s_enc(s16 *ibuf, u8 *obuf, uint samples);
void lc3s_enc_exit(void);

void m_lc3s_dec_init(u8 sample_rate, u16 samples);
void m_lc3s_dec(u8 *ibuf, s16 *obuf, uint samples, uint index);
void m_lc3s_dec_exit(uint index);

void dac2_get_fifocnt(u32 tick_cnt);
void dac2_play_sync_fifocnt(u16 high_thr, u16 low_thr);

bool wireless_mic_is_active(void);
void wireless_status_change(void);
bool wireless_discon_all(void);
void ble_delete_link_info(void);

u8 *bongding_addr_get(void);
void wireless_get_local_bd_addr(u8 *addr);
void wireless_mic_unbonding(void);             //¶Ï¿ªÁ¬½ÓºÍÇå³ý×é¶Ó°ó¶¨½Ó¿Ú

bool wireless_get_link_info_addr(uint8_t *wl_addr);
enum{
    BONDING_NUM_0= 0,
    BONDING_NUM_1,
    BONDING_NUM_2,
    BONDING_NUM_RX_3,
};
u8 wireless_get_link_info_state(void);
void wireless_bond_nb_updatae(void);
void wireless_bond_init(void);
void wireless_bond_clr(void);

#if WIRELESS_DUMP_EN
void wireless_dump_init(void);
void wireless_dump_proc(void);
void wireless_dump_reset(u8 idx);
void wireless_dump_set_rx_status(u8 idx, u8 chidx, s8 rssi, u8 bfi);
void wireless_dump_set_chmap_cb(u8 idx, const u8 *chmap);
#else
    #define wireless_dump_init()
    #define wireless_dump_proc()
    #define wireless_dump_reset(idx)
    #define wireless_dump_set_rx_status(idx, chidx, rssi, bfi)
    #define wireless_dump_set_chmap_cb(idx, chmap)
#endif
bool wireless_discon_all(void);
void wireless_disconnect_req(uint index);
void wireless_con_ch_map_set(u8 idx);

#endif //_WIRELESS_H
