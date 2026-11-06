#include "include.h"
#include "func.h"
#include "func_device.h"

AT(.text.func.device)
void func_device_message(u16 msg)
{
    switch (msg) {
#if FUNC_BT_EN
    case MSG_FUNC_BT:
        func_cb.sta = FUNC_BT;
        break;
#endif
#if WIRELESS_CON_AND_BT_PAIRING_EN
    case MSG_PAIRING:
        //进入配对状态
        printf("MSG_PAIRING\n");
        wireless_device_pairing_enable();
        break;
#endif

#if WIRELESS_MIC_AINS4_EN
    //PP键开关AINS4降噪，NEXT/PREV键调降噪量，实现在plugin.c
    case MSG_NR_TOGGLE:
        printf("**************************\n");
        plugin_ains4_toggle();
        break;

    case MSG_NR_LEVEL_UP:
        plugin_ains4_level_up();
        break;

    case MSG_NR_LEVEL_DOWN:
        plugin_ains4_level_down();
        break;
#endif // WIRELESS_MIC_AINS4_EN

#if WIRELESS_MIC_SOFT_GAIN_EN
    case MSG_VOL_UP:
        printf("MSG_VOL_UP\n");
        soft_gain_up();                    //话筒音量VOL+
        break;

    case MSG_VOL_DOWN:
        printf("MSG_VOL_DOWN\n");
    	soft_gain_down();                  //话筒音量VOL-
        break;
#endif // WIRELESS_MIC_SOFT_GAIN_EN

    case MSG_SYS_1S:
#if WIRELESS_MIC_BROADCAST_EN
        wireless_per_ch_map_update();
#endif
        break;

    default:
        func_message(msg);
        break;
    }
}
