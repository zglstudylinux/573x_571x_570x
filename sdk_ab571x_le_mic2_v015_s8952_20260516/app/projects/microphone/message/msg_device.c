#include "include.h"
#include "func.h"
#include "func_device.h"

AT(.text.func.device)
void func_device_message(u16 msg)
{
    switch (msg) {

//    case 0xe0:
//        mic_enc_mute_set(1);
//        ylcrn_L3_mic_mute_set(1);
//        ains4_mic_param_set(0,1000);
//        ains4_mic_mute_set(0);
//        delay_5ms(10);
//        mic_enc_mute_set(0);
//
//        my_printf("AINS4 EN\n");
//        break;
//
//    case 0x08:
//        mic_enc_mute_set(1);
//        ains4_mic_mute_set(1);
//        ylcrn_L3_mic_param_set(1000);
//        ylcrn_L3_mic_mute_set(0);
//        delay_5ms(10);
//        mic_enc_mute_set(0);
//        my_printf("AI EN\n");
//        break;

//    case 0x09:
//        my_printf("KEY2\n");
//        break;

    case MSG_SYS_1S:
#if WIRELESS_MIC_BROADCAST_EN
        ble_per_ch_map_update();
#endif
        break;

#if WIRELESS_MIC_DNR_FRE_EN || WIRELESS_MIC_DNN_L1_EN
    //PP循环切换降噪算法，VOL+/VOL-前后切换，实现在plugin.c
    case MSG_NR_TOGGLE:
        plugin_nr_toggle();
        break;

    case MSG_NR_LEVEL_UP:
        plugin_nr_next();
        break;

    case MSG_NR_LEVEL_DOWN:
        plugin_nr_prev();
        break;
#endif // WIRELESS_MIC_DNR_FRE_EN || WIRELESS_MIC_DNN_L1_EN

    default:
        func_message(msg);
        break;
    }
}
