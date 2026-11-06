#include "include.h"
#include "func.h"

AT(.text.func.device.msg)
void func_device_message(u16 msg)
{
    switch (msg) {
        case MSG_MUSIC_PLAY_PAUSE:
            break;

#if WIRELESS_MIC_SOFT_VOL_EN
        case MSG_VOL_UP:
            printf("MSG_VOL_UP\n");
            soft_gain_up();                    //话筒音量VOL+
            break;

        case MSG_VOL_DOWN:
            printf("MSG_VOL_DOWN\n");
            soft_gain_down();                  //话筒音量VOL-
            break;
#endif // WIRELESS_MIC_SOFT_VOL_EN

        case MSG_SYS_1S:
#if WIRELESS_MIC_BROADCAST_EN
            ble_per_ch_map_update();
#endif
            break;

        default:
            func_message(msg);
            break;
    }
}
