#include "include.h"
#include "wireless.h"

#if WIRELESS_EN && ADAPTER_SAVE_PARAM_EN

#if ADAPTER_EN
// Key ID
#define KEY_ID_NO               0x00
#define KEY_ID_PP               0x01
#define KEY_ID_K1               0x02
#define KEY_ID_K2               0x03
#define KEY_ID_K3               0x04
#define KEY_ID_K4               0x05

#define EVT_MIC_MUTE            0xf2
#define EVT_MIC_VOL_UP          0xf1
#define EVT_MIC_VOL_DOWN        0xf0
#define EVT_MIC_ECHO_UP         0xea
#define EVT_MIC_ECHO_DOWN       0xe9
#define EVT_MIC_MAGIC_CHANGE    0xe8
#define EVT_RM_VIOCE_CHANGE     0xe7
#define EVT_BT_MUSIC_PP         0xe6
#define EVT_BT_MUSIC_NEXT       0xe5
#define EVT_BT_MUSIC_PREV       0xe4
#define EVT_BT_MUSIC_VOL_UP     0xe3
#define EVT_BT_MUSIC_VOL_DOWN   0xe2
#define EVT_USER_UP             0xe1
#define EVT_USER_DOWN           0xe0
#define EVT_USER_MODE_CHANGE    0xdf
#define EVT_PWR_OFF_START       0xde
#define EVT_PWR_OFF_HOLD        0xdd
#define EVT_PWR_OFF_RELEASE     0xdc
#define EVT_PWR_OFF             0xdb

#else

const u8 key_msg_2_evt[20] = {KEY_ID_NO, EVT_MIC_MUTE, EVT_PWR_OFF, EVT_MIC_VOL_UP, EVT_MIC_VOL_DOWN, \
                              EVT_MIC_ECHO_UP, EVT_MIC_ECHO_DOWN, EVT_MIC_MAGIC_CHANGE, EVT_RM_VIOCE_CHANGE, EVT_BT_MUSIC_PP, \
                              EVT_BT_MUSIC_NEXT, EVT_BT_MUSIC_PREV, EVT_BT_MUSIC_VOL_UP, EVT_BT_MUSIC_VOL_DOWN, EVT_USER_UP, \
                              EVT_USER_DOWN, EVT_USER_MODE_CHANGE, EVT_PWR_OFF_START, EVT_PWR_OFF_HOLD, EVT_PWR_OFF_RELEASE};

void wl_key_tbl_param_set(wl_xcfg_str_t * wl_xcfg_ptr)
{
    u8 wl_temp_tbl[5][5];

    memcpy(wl_temp_tbl, key_msg_tbl_dft[sys_cb.key_tbl_sel&0x3], sizeof(wl_temp_tbl));
    if (sys_cb.key_tbl_sel&0x4) {
        ///上下曲相反
        uint8_t k2_long_save = wl_temp_tbl[KEY_ID_K2][1];
        wl_temp_tbl[KEY_ID_K2][1] = wl_temp_tbl[KEY_ID_K1][1];
        wl_temp_tbl[KEY_ID_K1][1] = k2_long_save;
    }
    if (wl_xcfg_ptr->wl_key1_msc_cfg_en) {
        wl_temp_tbl[1][0] = key_msg_2_evt[wl_xcfg_ptr->wl_key1_msc_ks_sel];
        wl_temp_tbl[1][1] = key_msg_2_evt[wl_xcfg_ptr->wl_key1_msc_kl_sel];
        wl_temp_tbl[1][2] = key_msg_2_evt[wl_xcfg_ptr->wl_key1_msc_kl_up_sel];
        wl_temp_tbl[1][3] = key_msg_2_evt[wl_xcfg_ptr->wl_key1_msc_kl_hold_sel];
        wl_temp_tbl[1][4] = key_msg_2_evt[wl_xcfg_ptr->wl_key1_msc_kd_sel];
    }
    if (wl_xcfg_ptr->wl_key2_msc_cfg_en) {
        wl_temp_tbl[2][0] = key_msg_2_evt[wl_xcfg_ptr->wl_key2_msc_ks_sel];
        wl_temp_tbl[2][1] = key_msg_2_evt[wl_xcfg_ptr->wl_key2_msc_kl_sel];
        wl_temp_tbl[2][2] = key_msg_2_evt[wl_xcfg_ptr->wl_key2_msc_kl_up_sel];
        wl_temp_tbl[2][3] = key_msg_2_evt[wl_xcfg_ptr->wl_key2_msc_kl_hold_sel];
        wl_temp_tbl[2][4] = key_msg_2_evt[wl_xcfg_ptr->wl_key2_msc_kd_sel];
    }
    if (wl_xcfg_ptr->wl_key3_msc_cfg_en) {
        wl_temp_tbl[3][0] = key_msg_2_evt[wl_xcfg_ptr->wl_key3_msc_ks_sel];
        wl_temp_tbl[3][1] = key_msg_2_evt[wl_xcfg_ptr->wl_key3_msc_kl_sel];
        wl_temp_tbl[3][2] = key_msg_2_evt[wl_xcfg_ptr->wl_key3_msc_kl_up_sel];
        wl_temp_tbl[3][3] = key_msg_2_evt[wl_xcfg_ptr->wl_key3_msc_kl_hold_sel];
        wl_temp_tbl[3][4] = key_msg_2_evt[wl_xcfg_ptr->wl_key3_msc_kd_sel];
    }
    if (wl_xcfg_ptr->wl_key4_msc_cfg_en) {
        wl_temp_tbl[4][0] = key_msg_2_evt[wl_xcfg_ptr->wl_key4_msc_ks_sel];
        wl_temp_tbl[4][1] = key_msg_2_evt[wl_xcfg_ptr->wl_key4_msc_kl_sel];
        wl_temp_tbl[4][2] = key_msg_2_evt[wl_xcfg_ptr->wl_key4_msc_kl_up_sel];
        wl_temp_tbl[4][3] = key_msg_2_evt[wl_xcfg_ptr->wl_key4_msc_kl_hold_sel];
        wl_temp_tbl[4][4] = key_msg_2_evt[wl_xcfg_ptr->wl_key4_msc_kd_sel];
    }

    wl_key_msg_tbl_set((u8 *)&wl_temp_tbl[0][0]);
}

#endif

#endif
