#include "include.h"
#include "func.h"

#if FUNC_LE_DUT_EN
AT(.text.func.bt)
void func_dut_message(u16 msg)
{
    switch (msg) {
    case MSG_SYS_1S:
        break;

    case EVT_HCI_CMD:
         ble_hci_cmd_kick(); //kick蓝牙线程执行hci命令
         break;

    default:
        func_message(msg);
        break;
    }
}
#endif
