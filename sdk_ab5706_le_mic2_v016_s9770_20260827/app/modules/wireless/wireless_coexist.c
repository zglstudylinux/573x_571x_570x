
#include "include.h"

#if ADAPTER_UART_COMMAND_EN

#define EXTERNAL_ISO_INTV            48
#define EXTERNAL_ISO_WIN             22

#define EXTERNAL_ISO_LOST_CNT        4
#define EXTERNAL_ISO_EVT_THRESHOLD   3


static struct ble_time_s last_iso_evt_time;
static uint32_t valid_evt_cnt;

AT(.com_text.coexist.wireless)
bool external_iso_is_exist(void)
{
    int clk_diff;
    struct ble_time_s curr_time;

    bt_get_time(&curr_time.hs, &curr_time.hus);
    clk_diff = bt_clk_diff(last_iso_evt_time.hs, curr_time.hs);
    if ((valid_evt_cnt > EXTERNAL_ISO_EVT_THRESHOLD) &&
        (clk_diff >= 0) && (clk_diff < (EXTERNAL_ISO_INTV * EXTERNAL_ISO_LOST_CNT))) {
        return true;
    } else {
        return false;
    }
}

AT(.com_text.coexist.wireless)
void external_iso_anchor_point_get(uint32_t *hs, uint32_t *hus)
{
    *hs = last_iso_evt_time.hs;
    *hus = last_iso_evt_time.hus;
}

AT(.com_text.coexist.isr)
static void wireless_coexist_tick_cmd_process(void)
{
    uint8_t i;
    bool valid_evt = false;
    int clk_diff;
    uint32_t last_valid_evt_cnt = valid_evt_cnt;
    struct ble_time_s curr_time;

    bt_get_time(&curr_time.hs, &curr_time.hus);
    clk_diff = bt_clk_diff(last_iso_evt_time.hs, curr_time.hs);
    for (i = 1; i <= EXTERNAL_ISO_LOST_CNT; i++) {
        if ((clk_diff > (EXTERNAL_ISO_INTV * i - 2)) && (clk_diff < (EXTERNAL_ISO_INTV * i + 2))) {
            valid_evt = true;
            break;
        }
    }

    if (valid_evt) {
        if (valid_evt_cnt <= EXTERNAL_ISO_EVT_THRESHOLD) {
            valid_evt_cnt++;
        }
    } else {
        valid_evt_cnt = 0;
    }

    last_iso_evt_time.hs = curr_time.hs;
    last_iso_evt_time.hus = curr_time.hus;

    if ((valid_evt_cnt > EXTERNAL_ISO_EVT_THRESHOLD) && (last_valid_evt_cnt <= EXTERNAL_ISO_EVT_THRESHOLD)) {
        wireless_adapter_set_reconnect();
    }
}

AT(.com_text.coexist.isr)
void wireless_coexist_uart_isr(uint16_t cmd)
{
    switch (cmd) {
    case COEXIST_CMD_TICK:
        wireless_coexist_tick_cmd_process();
        break;
    case COEXIST_CMD_ENABLE_WL:
        wireless_adapter_set_enable();
        break;
    case COEXIST_CMD_DISABLE_WL:
        wireless_adapter_set_disable();
        break;
    case COEXIST_CMD_RECONN_WL:
        wireless_adapter_set_reconnect();
        break;
    case COEXIST_CMD_REQ_MIC0_INFO:
        msg_enqueue(EVT_COEXIST_CMD_REQ_MIC0_INFO);
        break;
    case COEXIST_CMD_REQ_MIC1_INFO:
        msg_enqueue(EVT_COEXIST_CMD_REQ_MIC1_INFO);
        break;
    default:
        break;
    }
}

#endif

