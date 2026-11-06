#include "include.h"
#include "func.h"
AT(.text.func.wireless_mic)
void func_adapter_message(u16 msg)
{
    switch (msg) {
#if WIRELESS_CON_AND_BT_PAIRING_EN 
        case MSG_PAIRING:
            if(wireless_cb.connected_sta & BIT(0)) {
                wireless_disconnect_req(0);
            }
            if(wireless_cb.connected_sta & BIT(1)) {
                wireless_disconnect_req(1);
            }
            printf("MSG_PAIRING\n");
            wireless_bonding_clr();
            wireless_adapter_pairing_enable(1);
            break;
#endif
        case MSG_SYS_1S:
#if WIRELESS_CON_PWR_CTR
            ws_pwr_ctr_tx_cmd_process();
#endif
            break;

#if WIRELESS_MIC_AINS4_EN
        //PP键开关AINS4降噪，NEXT/PREV键调降噪量，实现在plugin.c
        case MSG_NR_TOGGLE:
            plugin_ains4_toggle();
            break;

        case MSG_NR_LEVEL_UP:
            plugin_ains4_level_up();
            break;

        case MSG_NR_LEVEL_DOWN:
            plugin_ains4_level_down();
            break;
#endif // WIRELESS_MIC_AINS4_EN

    default:
        func_message(msg);
        break;
    }
}
