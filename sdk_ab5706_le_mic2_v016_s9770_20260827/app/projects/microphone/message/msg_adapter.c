#include "include.h"
#include "func.h"
#include "usb_device/usb_enum.h"

AT(.text.func.adapter.msg)
void func_adapter_message(u16 msg)
{
    switch (msg) {
        case MSG_MUSIC_PLAY_PAUSE:
#if WARNING_POWER_ON
    bsp_res_play(RES_IDX_POWERON);

    //等待开机提示音播报完，并完成设备插入检测
    do {
        func_process();
    } while(bsp_res_is_playing());
#endif // WARNING_POWER_ON
            break;

    case MSG_SYS_1S:
#if WIRELESS_CON_PWR_CTR
        ws_pwr_ctr_tx_cmd_process();
#endif
#if ADAPTER_HUART_OUTPUT_EN && ADAPTER_HUART_COMMAND_EN
//        huart_tx(ch, sizeof(ch));
//        huart_cmd_sync();
#endif
        break;

#if ADAPTER_USB_MIC_RX_EN | ADAPTER_USB_SPK_EN
    case EVT_PC_INSERT:
        ude_info_init();
        usb_device_enter(UDE_ENUM_TYPE);    //内部会调用usb_init初始化控制器
        sys_cb.ude_flag = true;
        break;

    case EVT_PC_REMOVE:
        pc_remove();
        usb_device_exit();
        sys_cb.ude_flag = false;
        break;

    case EVT_UDE_SET_VOL:
        ude_set_spk_vol_cb();
        break;

    case EVT_UDE_SET_VOL_L_R:
        ude_set_spk_volume_db_l_r();
        break;

    case EVT_UDE_SET_MUTE:
        printf("UDE_SET_MUTE\n");
        break;
#endif
#if ADAPTER_HUART_COMMAND_EN
    case EVT_UART_COMMAND_PROC:
        uart_command_rx_proc();
        break;
#endif
#if ADAPTER_UART_COMMAND_EN
    case EVT_COEXIST_CMD_REQ_MIC0_INFO:
        if (wireless_cb.connected_sta & BIT(0)) {
             uart2_putchar(COEXIST_CMD_MIC0_CONNECTED);
        } else {
             uart2_putchar(COEXIST_CMD_MIC0_DISCONNECTED);
        }
        break;
    case EVT_COEXIST_CMD_REQ_MIC1_INFO:
        if (wireless_cb.connected_sta & BIT(1)) {
             uart2_putchar(COEXIST_CMD_MIC1_CONNECTED);
        } else {
             uart2_putchar(COEXIST_CMD_MIC1_DISCONNECTED);
        }
        break;
#endif

    default:
        func_message(msg);
        break;
    }
}
