#ifndef _WIRELESS_CON_INTERACT_PROC_H
#define _WIRELESS_CON_INTERACT_PROC_H

#define WL_SAVE_VERSION             0       ///默认version是0才起作用

#define CON_INTERACT_MAX_LEN        (CMD_MAX_EXT_SIZE-4)

typedef struct inter_ctl_tag {
    u8 *ptr;
    u16 offset;
    u16 remain;
    u8 opcode;
    u8 state;
    u16 len;
} inter_ctl_t;

typedef struct ram_ctl_tag{
    u32 addr;
    u16 len;
} ram_ctl_t;

struct cmd_info_tag {
    uint (*prase)(u8 *buf, u8 len, inter_ctl_t *ctl);
    void (*rxdone)(inter_ctl_t *ctl);
};

typedef struct {
    uint8_t state;
    inter_ctl_t inter_ctl;
    ram_ctl_t ram_ctr;
} adapter_inter_str_t;

enum {
    CON_INTERACT_IDLE_STA = 0,
    CON_INTERACT_TX_GET_INFO_STA,
    CON_INTERACT_W4_GET_INFO_RSP_STA,
    CON_INTERACT_TX_GET_INFO_RSP_STA,
    CON_INTERACT_TX_XCFG_PARAM_STA,
    CON_INTERACT_TX_EQ_PARAM_STA,
    CON_INTERACT_TX_DRC_PARAM_STA,
    CON_INTERACT_TX_PATCH_LEN_STA,
    CON_INTERACT_TX_PATCH_PARAM_STA,
    CON_INTERACT_TX_COMPLETE_STA,
    CON_INTERACT_W4_TX_COMPLETE_CFM_STA,
    CON_INTERACT_TX_WR_RAM_STA,
    CON_INTERACT_TX_RD_RAM_STA,
    CON_INTERACT_TX_SET_RUN_STA,
};

enum {
    CMD_STA_OK,
    CMD_STA_ERR,
    CMD_STA_RX,
    CMD_STA_TX,
};

typedef struct {
    uint16_t version;
    uint8_t cmd_sta;
    uint8_t txbuf_nb;
    uint8_t flags;
} device_info_t;

typedef struct {
    inter_ctl_t inter_ctl;
    ram_ctl_t ram_ctr;
    u8 flags;
    bool cmd_allow;
    uint8_t state;
} device_inter_str_t;

typedef union inter_head_tag {
    u16 param0;
    struct {
        u16 data_len    : 12;
        u16 rsvd        : 2;
        u16 addr_flag   : 1;
        u16 cont_flag   : 1;
    };
} inter_head_t;

typedef struct {
    u32 start_addr;
    u32 run_addr;
    u8 patch_array[];
} patch_info_t;

void wireless_con_interact_init(void);
void wireless_con_interact_exit(uint8_t index);

void wireless_device_interact_init(void);
void wireless_con_interact_process(void);
void wireless_adapter_con_interact_process(void);
void wireless_device_con_interact_process(void);
void wireless_con_interact_kick(uint8_t index);
void wireless_rx_adapter_con_interact_data(u8 index, wireless_cmd_t *pdu, u8 len);
void wireless_rx_device_con_interact_data(u8 index, wireless_cmd_t *pdu, u8 len);
bool wl_con_interact_tx_skip_first(u8 index, uint8_t skip_en);

///????
void mic_eq_init_by_tbl(u8 *soft_eq_tbl, u32 soft_eq_tbl_len);
void mic_drc_init_by_tbl(u8 *drc_tbl, u32 drc_tbl_len);

#endif // _WIRELESS_CON_INTERACT_PROC_H
