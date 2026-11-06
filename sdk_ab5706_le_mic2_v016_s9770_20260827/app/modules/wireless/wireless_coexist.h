#ifndef _WIRELESS_COEXIST_H
#define _WIRELESS_COEXIST_H

enum coexist_cmd {
    COEXIST_CMD_INVALID,
    COEXIST_CMD_TICK,
    COEXIST_CMD_ENABLE_WL,
    COEXIST_CMD_DISABLE_WL,
    COEXIST_CMD_RECONN_WL,
    COEXIST_CMD_REQ_MIC0_INFO,
    COEXIST_CMD_REQ_MIC1_INFO,
    COEXIST_CMD_MIC0_CONNECTED,
    COEXIST_CMD_MIC1_CONNECTED,
    COEXIST_CMD_MIC0_DISCONNECTED,
    COEXIST_CMD_MIC1_DISCONNECTED,
};

struct ble_time_s {
    uint32_t hs;
    uint32_t hus;
};

void wireless_adapter_set_reconnect(void);
void wireless_adapter_set_enable(void);
void wireless_adapter_set_disable(void);
void uart2_putchar(char ch);

void wireless_coexist_uart_isr(uint16_t cmd);

#endif
