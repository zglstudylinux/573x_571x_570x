#include "include.h"
#include "func.h"
#include "func_le_dut.h"

#if FUNC_LE_DUT_EN
AT(.text.func.le.dut)
static void func_le_dut_process(void)
{
    func_process();
}

AT(.text.func.le.dut)
static void func_le_dut_enter()
{
    cfg_bt_work_mode = MODE_BQB_RF_BLE;

    func_bt_init();
    ble_dut_init();
}

static AT(.text.func.le.dut)
void func_le_dut_exit()
{
    bt_off();
    func_cb.last = FUNC_LE_DUT;
    sys_cb.bt_is_inited = 0;
}

AT(.text.func.le.dut)
void func_le_dut(void)
{
    printf("%s\n", __func__);

    func_le_dut_enter();

    while (func_cb.sta == FUNC_LE_DUT) {
        func_le_dut_process();
        func_dut_message(msg_dequeue());
    }

    func_le_dut_exit();
}
#endif
