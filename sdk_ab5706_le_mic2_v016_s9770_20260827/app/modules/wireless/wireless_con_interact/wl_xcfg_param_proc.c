#include "include.h"
#include "wireless.h"

#if WIRELESS_EN && ADAPTER_SAVE_PARAM_EN
static wl_xcfg_str_t wl_xcfg_str;

#if ADAPTER_EN

void wl_xcfg_param_tx_proc_init(uint8_t index)
{
    memcpy((u8 *)&wl_xcfg_str, (u8 *)&xcfg_cb.wl_save_version, sizeof(wl_xcfg_str_t));
    if (wl_xcfg_str.save_voice_param_en) {
        u8 con_addr[6];
        wl_pscan_get_addr_for_index(con_addr, index);

        wl_adapter_save_str *wl_adapter_save_ptr = wl_device_db_get_for_addr(con_addr);
        wl_xcfg_str.soft_gain_save_level = wl_adapter_save_ptr->wl_voice_param.mic_vol_level;
        wl_xcfg_str.echo_save_level = wl_adapter_save_ptr->wl_voice_param.echo_level;
        wl_xcfg_str.magic_save_level = wl_adapter_save_ptr->wl_voice_param.magic_level;
        wl_xcfg_str.mic_mute_save_level = wl_adapter_save_ptr->wl_voice_param.mute_level;
    }
    if (wl_xcfg_str.soft_gain_replace_en) {
        wl_xcfg_str.soft_gain_level_0 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_0];
        wl_xcfg_str.soft_gain_level_1 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_1];
        wl_xcfg_str.soft_gain_level_2 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_2];
        wl_xcfg_str.soft_gain_level_3 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_3];
        wl_xcfg_str.soft_gain_level_4 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_4];
        wl_xcfg_str.soft_gain_level_5 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_5];
        wl_xcfg_str.soft_gain_level_6 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_6];
        wl_xcfg_str.soft_gain_level_7 = soft_gain_tbl_64[wl_xcfg_str.soft_gain_level_7];
    }
}

uint8_t *wl_xcfg_param_get(uint8_t index, uint16_t *buf_len)
{
    wl_xcfg_param_tx_proc_init(index);
    *buf_len = sizeof(wl_xcfg_str_t);

    return (u8 *)&wl_xcfg_str;
}

#else

void wl_xcfg_param_set(void)
{
    if (wl_xcfg_str.wl_save_version == WL_SAVE_VERSION) {
        wl_voice_param_set(&wl_xcfg_str);
        wl_key_tbl_param_set(&wl_xcfg_str);
        wl_eq_drc_replace_set(&wl_xcfg_str);

        if (xcfg_cb.mic_dnr_en && wl_xcfg_str.dnr_replace_en) {
            mic_audio_dnr_max_pow_set(wl_xcfg_str.dnr_max_pow);
        }
    }
}

uint wl_xcfg_prase(u8 *buf, u8 len, inter_ctl_t *ctl)
{
    u16 data_len = GET_LE16(buf) & 0xfff;

    if(data_len > sizeof(wl_xcfg_str_t)) {
        return CMD_STA_ERR;
    }

    ctl->ptr      = (u8 *)&wl_xcfg_str;
    ctl->remain   = data_len;
    ctl->offset   = 0;
    return CMD_STA_RX;
}

void wl_xcfg_rxdone(inter_ctl_t *ctl)
{
    msg_enqueue(EVT_WL_XCFG_PARAM_SET);
}

#endif

#endif
