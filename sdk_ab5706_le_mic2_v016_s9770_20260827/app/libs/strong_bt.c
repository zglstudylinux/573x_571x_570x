/**********************************************************************
*
*   strong_bt.c
*   定义库里面bt部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


#if (!BT_FCC_TEST_EN && !FUNC_BT_FCC_EN) || !LE_EN
uint8_t vs_ble_test(void const *cmd) {
    return 0x11;
}

#if !LE_BQB_RF_EN && !LE_FCC_TEST_EN
uint8_t ble_test_start(void* params) {
    return 0x0c;
}
uint8_t ble_test_stop(void) {
    return 0x0c;
}
#endif
#endif

#if BT_FCC_TEST_EN || FUNC_BT_FCC_EN
#if LE_EN
uint8_t vs_ble_test_do(void const *cmd);
uint8_t vs_ble_test(void const *cmd) {
    return vs_ble_test_do(cmd);
}
#endif
#else
AT(.com_text.weak.bt21.isr.test.fcc)
bool bt_acl_test_rx_end(uint8_t index, void *par) {
    return false;
}
uint8_t vs_fcc_test_cmd(void const *param) {
    return 0x11;
}

#if !LE_BQB_RF_EN && !LE_FCC_TEST_EN
void hci_h3c_init(void) {}
void bt_uart_init(void){}
AT(.com_text.weak.stack.uart_isr)
bool bt_uart_isr(void) {
    return false;
}
#endif
#endif

#if !BT_HID_DOUYIN_EN
void btstack_hid_douyin(uint keycode){}
#endif

WEAK uint8_t bt_get_connected_num(void) {return 0;}

#if !BT_HFP_REC_EN
AT(.com_text.weak.bt_rec)
void bt_sco_rec_mix_do(u8 *buf, u32 samples) {}
void bt_sco_rec_fill_far_buf(u16 *buf, u16 samples) {}
#endif


bool app_spp_rx_callback(uint8_t ch, u8 *data,u16 len) {return 0;}
void app_spp_connect_callback(uint8_t ch) {}
void app_spp_disconnect_callback(uint8_t ch) {}
