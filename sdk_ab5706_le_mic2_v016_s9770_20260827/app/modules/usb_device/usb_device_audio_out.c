#include "include.h"
#include "usb_device_audio_out.h"

#if UDE_STORAGE_EN || UDE_SPEAKER_EN || UDE_HID_EN || UDE_MIC_EN

static usb_audio_out_cfg_t usb_audio_out_cfg;
static volatile uint8_t usb_audio_ou_init_flag = 0;

AT(.buf.ude.cb)
spk_ude_t spk_ude;

/*=====================================================================================*/
AT(.text.bsp.dac)
void bsp_dac2_change_vol(u8 level)
{
    s32 db_num;
    db_num = 60 - level;
    if (db_num > 60) {
        db_num = 60;
    }
    if (db_num < 0) {
        db_num = 0;
    }

    dac2_vol_set(dac_dvol_tbl_db[db_num]);
}

AT(.text.bsp.usb)
void ude_set_spk_vol_cb(void)
{
    my_printf("spk_ude.db_level = %d\n",spk_ude.db_level);
    bsp_dac2_change_vol(spk_ude.db_level);
    wireless_set_usbspk_vol(spk_ude.db_level);
}

AT(.text.bsp.usb)
void ude_set_spk_volume_db_l_r(void)
{
//    my_printf("ude_set_spk_volume_db_l_r\n");
    bsp_change_volume_db_l_r(spk_ude.db_level_l, spk_ude.db_level_r);
}

AT(.usbdev.com)
void ude_set_sys_volume(u8 vol)
{
    if (spk_ude.db_level != vol) {
        spk_ude.db_level = vol;
        msg_enqueue(EVT_UDE_SET_VOL);
    }
}

//系统音量mute与usb mute同步
AT(.usbdev.com)
void ude_set_sys_mute(u8 sys_mute)
{
    if (sys_cb.mute != sys_mute) {
        msg_enqueue(EVT_UDE_SET_MUTE);
    }
}

u8 ude_get_sys_volume(void)
{
    return spk_ude.db_level;
}

//usb左右声道均衡
AT(.usbdev.com)
void uda_set_balance_vol(u8 vol_l, u8 vol_r)
{
    if (spk_ude.db_level_l != vol_l || spk_ude.db_level_r != vol_r) {
        spk_ude.db_level_l = vol_l;
        spk_ude.db_level_r = vol_r;
        msg_enqueue(EVT_UDE_SET_VOL_L_R);
    }
}
/*=====================================================================================*/

///USB AUDIO iso out process(speaker) callback
AT(.usbdev.com)
void ude_isoc_rx_process_callback(u8 *ptr, u8 len)
{
    usb_audio_out_audio_input(ptr, len, 2, NULL);
}

AT(.com_text.usb_audio_out)
void usb_audio_out_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    if (usb_audio_ou_init_flag && usb_audio_out_cfg.callback) {
        if (usb_audio_out_cfg.channel == 1) {
            s16 *mono_ptr = (s16 *)ptr;
            for (uint16_t i = 0; i < samples; i++) {
                mono_ptr[i] = mono_ptr[2 * i] + mono_ptr[2 * i + 1];
            }
        }

        usb_audio_out_cfg.callback(ptr, samples, usb_audio_out_cfg.channel, params);
    }
}

AT(.text.usb_audio_out)
void usb_audio_out_audio_output_callback_set(audio_callback_t callback)
{
    usb_audio_out_cfg.callback = callback;
}

AT(.text.usb_audio_out)
void usb_audio_out_audio_mute_set(u8 mute)
{
    usb_audio_out_cfg.mute = mute;
}

AT(.text.usb_audio_out)
void usb_audio_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&usb_audio_out_cfg, 0, sizeof(usb_audio_out_cfg));
    usb_audio_out_cfg.channel = channel;

    spk_ude.db_level = bsp_dac_get_gain_level(sys_cb.vol);

    usb_audio_ou_init_flag = 1;
}

#endif
