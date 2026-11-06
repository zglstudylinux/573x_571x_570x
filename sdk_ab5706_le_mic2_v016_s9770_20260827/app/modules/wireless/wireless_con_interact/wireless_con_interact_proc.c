#include "include.h"
#include "wireless.h"

#if WIRELESS_EN && ADAPTER_SAVE_PARAM_EN

#define CON_INTERACT_DEBUG_EN                    0

#if CON_INTERACT_DEBUG_EN
#define CON_INTERACT_DEBUG(...)                  printf(__VA_ARGS__)
#define CON_INTERACT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define CON_INTERACT_DEBUG(...)
#define CON_INTERACT_DEBUG_R(...)
#endif

#define CMD_ALLOW_KEY           0x9e8a3564

uint wl_xcfg_prase(u8 *buf, u8 len, inter_ctl_t *ctl);
void wl_xcfg_rxdone(inter_ctl_t *ctl);
uint wl_eq_prase(u8 *buf, u8 len, inter_ctl_t *ctl);
void wl_eq_rxdone(inter_ctl_t *ctl);
uint wl_drc_prase(u8 *buf, u8 len, inter_ctl_t *ctl);
void wl_drc_rxdone(inter_ctl_t *ctl);
uint wl_tx_complete_prase(uint8_t *buf, u8 len, inter_ctl_t *ctl);
uint wl_skip_first_prase(uint8_t *buf, u8 len, inter_ctl_t *ctl);
uint wl_wr_ram_prase(u8 *buf, u8 len, inter_ctl_t *ctl);
void wl_wr_ram_rxdone(inter_ctl_t *ctl);
uint wl_rd_ram_prase(u8 *buf, u8 len, inter_ctl_t *ctl);
uint wl_set_run_prase(u8 *buf, u8 len, inter_ctl_t *ctl);
uint wl_get_info_prase(u8 *buf, u8 len, inter_ctl_t *ctl);

static const struct cmd_info_tag cmd_info_tbl[] = {
    [CON_INTERACT_GET_INFO]     = {wl_get_info_prase,       NULL,           },
    [CON_INTERACT_XCFG_PARAM]   = {wl_xcfg_prase,           wl_xcfg_rxdone, },
    [CON_INTERACT_EQ_PARAM]     = {wl_eq_prase,             wl_eq_rxdone,   },
    [CON_INTERACT_DRC_PARAM]    = {wl_drc_prase,            wl_drc_rxdone,  },
    [CON_INTERACT_TX_COMPLETE]  = {wl_tx_complete_prase,    wl_drc_rxdone,  },
    [CON_INTERACT_SKIP_FIRST]   = {wl_skip_first_prase,     wl_drc_rxdone,  },

    [CON_INTERACT_SET_RUN]      = {wl_set_run_prase,        NULL,           },
    [CON_INTERACT_WR_RAM]       = {wl_wr_ram_prase,         wl_wr_ram_rxdone,           },
    [CON_INTERACT_RD_RAM]       = {wl_rd_ram_prase,         NULL,           },
};

static uint inter_cmd_prase(u8 opcode, u8 *buf, u8 len, inter_ctl_t *inter_ptr)
{
    return cmd_info_tbl[opcode].prase(buf, len, inter_ptr);
}

static uint inter_rx_proc(u8 opcode, u8 *buf, u8 len, inter_ctl_t *inter_ptr)
{
    inter_head_t *p = (void *)buf;
    u16 trans_offset = p->cont_flag? p->data_len : 0;

    if (inter_ptr->offset != trans_offset || len <= 2) {
        return CMD_STA_ERR;
    }
    if(opcode != inter_ptr->opcode || inter_ptr->ptr == NULL) {
        return CMD_STA_ERR;
    }

    if(p->addr_flag) {
        len -= 2+4;
        buf += 2+4;
    } else {
        len -= 2;
        buf += 2;
    }

    if (inter_ptr->remain > len) {
        memcpy(inter_ptr->ptr + inter_ptr->offset, buf, len);
        inter_ptr->offset += len;
        inter_ptr->remain -= len;
        return CMD_STA_RX;
    } else {
        memcpy(inter_ptr->ptr + inter_ptr->offset, buf, inter_ptr->remain);
        inter_ptr->offset += inter_ptr->remain;
        inter_ptr->remain = 0;

        if(cmd_info_tbl[inter_ptr->opcode].rxdone != NULL) {
            cmd_info_tbl[inter_ptr->opcode].rxdone(inter_ptr);
        }
        return CMD_STA_OK;
    }
}

void wl_inter_rx_proc(u8 *buf, u8 opcode, inter_ctl_t *inter_ptr, u8 len)
{
    do {
        switch(inter_ptr->state) {
        case CMD_STA_ERR:
        case CMD_STA_OK:
            inter_ptr->opcode    = opcode;
            inter_ptr->remain    = 0;    //new opcode
            inter_ptr->state     = inter_cmd_prase(opcode, buf, len, inter_ptr);
            CON_INTERACT_DEBUG("cmd_prase: %d, %d\n", opcode, len);
            if(inter_ptr->state < CMD_STA_RX) {
                break;
            }
        case CMD_STA_RX:
            inter_ptr->state = inter_rx_proc(opcode, buf, len, inter_ptr);
            if(inter_ptr->state == CMD_STA_ERR) {
                CON_INTERACT_DEBUG("cmd_rx: err\n");
            }
            break;
        }
    } while(0);
}

#if ADAPTER_EN
adapter_inter_str_t adapter_inter_str[WIRELESS_CON_LINK_NB];

static bool wl_inter_tx_do(u8 index, u8 opcode, u8 *buf, u8 len, inter_ctl_t *inter_ptr, u8 addr_flag, u8 *addr)
{
    inter_head_t tx_head;

    wireless_cmd_t pdu;
    pdu.cmd = PRIVATE_CON_INTERACT_DATA;
    pdu.buf[0] = opcode;
    tx_head.cont_flag = (inter_ptr->offset==0) ? 0 : 1;
    tx_head.addr_flag = addr_flag;
    tx_head.data_len = (inter_ptr->offset==0) ? inter_ptr->len : inter_ptr->offset;

    PUT_LE16(&pdu.buf[1], tx_head.param0);
    if (addr_flag) {
        PUT_LE32(&pdu.buf[3], addr);
        memcpy((u8 *)&pdu.buf[7], buf, len);
    } else {
        memcpy((u8 *)&pdu.buf[3], buf, len);
    }

    return wireless_send_cmd(index, (u8 *)&pdu, (len+4+addr_flag*4));
}

bool inter_tx_proc(uint8_t index, inter_ctl_t *inter_ptr, u8 addr_flag, u8 *addr)
{
    u8 con_inter_max_len = addr_flag? (CON_INTERACT_MAX_LEN-4) : CON_INTERACT_MAX_LEN;

    if (inter_ptr->remain > con_inter_max_len) {
        if (wl_inter_tx_do(index, inter_ptr->opcode, (u8 *)(inter_ptr->ptr + inter_ptr->offset), con_inter_max_len, inter_ptr, addr_flag, addr)) {
            inter_ptr->remain -= con_inter_max_len;
            inter_ptr->offset += con_inter_max_len;
        }
    } else {
        if (inter_ptr->remain == 0) {
            if (wl_inter_tx_do(index, inter_ptr->opcode, NULL, inter_ptr->remain, inter_ptr, addr_flag, addr)) {
                return true;
            }
        } else {
            if (wl_inter_tx_do(index, inter_ptr->opcode, (u8 *)(inter_ptr->ptr + inter_ptr->offset), inter_ptr->remain, inter_ptr, addr_flag, addr)) {
                inter_ptr->remain = 0;
                inter_ptr->offset = 0;
                return true;
            }
        }
    }

    return false;
}

bool wl_inter_tx_proc(u8 index, u8 *buf, uint16_t buf_len, u8 opcode, inter_ctl_t *inter_ptr, u8 addr_flag, u8 *addr)
{
    do {
        switch(inter_ptr->state) {
        case CMD_STA_OK:
            if (inter_ptr->remain == 0) {
                inter_ptr->opcode    = opcode;
                inter_ptr->state     = CMD_STA_TX;
                inter_ptr->len       = buf_len;
                inter_ptr->remain    = buf_len;
                inter_ptr->ptr       = buf;
                inter_ptr->offset    = 0;
            }
            if(inter_ptr->state == CMD_STA_TX) {
                continue;
            }
            break;

        case CMD_STA_TX:
            if (inter_tx_proc(index, inter_ptr, addr_flag, addr)) {
                inter_ptr->state = CMD_STA_OK;
                return true;
            }
            break;
        }
    } while(0);

    return false;
}

bool wl_adapter_tx_proc(u8 index, u8 *buf, uint16_t buf_len, u8 opcode, u8 addr_flag, u8 *addr)
{
    if (wl_inter_tx_proc(index, buf, buf_len, opcode, &adapter_inter_str[index].inter_ctl, addr_flag, addr)) {
        return true;
    }

    return false;
}

void wireless_con_interact_process(void)
{
    uint8_t *buf;
    uint16_t buf_len;

#if ADAPTER_FIX_DEVICE_EN
    patch_info_t *patch_info_ptr = (patch_info_t *)(RES_BUF_5669T6_PATCH_BIN);
#endif

    for (uint8_t i = 0; i < WIRELESS_CON_LINK_NB; i++) {
    ///主循环，判断是否有事件需要处理
        if (adapter_inter_str[i].state != CON_INTERACT_IDLE_STA) {
            CON_INTERACT_DEBUG("con_intract_state = %d, %d\n", i, adapter_inter_str[i].state);
        }
        switch(adapter_inter_str[i].state) {
            case CON_INTERACT_IDLE_STA:
                break;

            case CON_INTERACT_TX_GET_INFO_STA:
                uint8_t allow_key_buf[4];
                PUT_LE32(allow_key_buf, CMD_ALLOW_KEY);      //ve
                if (wl_adapter_tx_proc(i, allow_key_buf, 4, CON_INTERACT_GET_INFO, 0, NULL)) {
                    adapter_inter_str[i].state = CON_INTERACT_W4_GET_INFO_RSP_STA;
                }
                break;

            case CON_INTERACT_TX_XCFG_PARAM_STA:
                buf = wl_xcfg_param_get(i, &buf_len);
                if (wl_adapter_tx_proc(i, buf, buf_len, CON_INTERACT_XCFG_PARAM, 0, NULL)) {
                    adapter_inter_str[i].state = CON_INTERACT_TX_EQ_PARAM_STA;
                }
                break;

            case CON_INTERACT_TX_EQ_PARAM_STA:
                if (!xcfg_cb.eq_replace_en) {
                    adapter_inter_str[i].state = CON_INTERACT_TX_DRC_PARAM_STA;
                } else {
                    buf = wl_eq_param_get(&buf_len);
                    if (wl_adapter_tx_proc(i, buf, buf_len, CON_INTERACT_EQ_PARAM, 0, NULL)) {
                        adapter_inter_str[i].state = CON_INTERACT_TX_DRC_PARAM_STA;
                    }
                }
                break;


            case CON_INTERACT_TX_DRC_PARAM_STA:
                if (!xcfg_cb.drc_replace_en) {
                    adapter_inter_str[i].state = CON_INTERACT_TX_WR_RAM_STA;
                } else {
                    buf = wl_drc_param_get(&buf_len);
                    if (wl_adapter_tx_proc(i, buf, buf_len, CON_INTERACT_DRC_PARAM, 0, NULL)) {
                        adapter_inter_str[i].state = CON_INTERACT_TX_WR_RAM_STA;
                    }
                }
                break;

            case CON_INTERACT_TX_PATCH_LEN_STA:
                break;

            case CON_INTERACT_TX_PATCH_PARAM_STA:
                break;

            case CON_INTERACT_TX_WR_RAM_STA:
#if ADAPTER_FIX_DEVICE_EN
                adapter_inter_str[i].ram_ctr.addr = patch_info_ptr->start_addr;
//                uint8_t test_buf1[40] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x40};
                if (wl_adapter_tx_proc(i, patch_info_ptr->patch_array, ((u32)(RES_LEN_5669T6_PATCH_BIN) - 8), CON_INTERACT_WR_RAM, 1, (u8 *)adapter_inter_str[i].ram_ctr.addr)) {
                    adapter_inter_str[i].state = CON_INTERACT_TX_RD_RAM_STA;
                }
#else
                    adapter_inter_str[i].state = CON_INTERACT_TX_RD_RAM_STA;
#endif
                break;

            case CON_INTERACT_TX_RD_RAM_STA:
//                adapter_inter_str[i].ram_ctr.addr = 0x1b020;
//                uint8_t test_buf2[2];
//                PUT_LE16(test_buf2, 8);
//                if (wl_adapter_tx_proc(i, test_buf2, 2, CON_INTERACT_RD_RAM, 1, (u8 *)adapter_inter_str[i].ram_ctr.addr)) {
                    adapter_inter_str[i].state = CON_INTERACT_TX_SET_RUN_STA;
//                }
                break;

            case CON_INTERACT_TX_SET_RUN_STA:
#if ADAPTER_FIX_DEVICE_EN
                adapter_inter_str[i].ram_ctr.addr = patch_info_ptr->run_addr;
                uint8_t test_buf2[2];
                PUT_LE16(test_buf2, 8);
                if (wl_adapter_tx_proc(i, test_buf2, 2, CON_INTERACT_SET_RUN, 1, (u8 *)adapter_inter_str[i].ram_ctr.addr)) {
                    adapter_inter_str[i].state = CON_INTERACT_TX_COMPLETE_STA;
                }
#else
                    adapter_inter_str[i].state = CON_INTERACT_TX_COMPLETE_STA;
#endif
                break;

            case CON_INTERACT_TX_COMPLETE_STA:
                if (wl_adapter_tx_proc(i, NULL, 0, CON_INTERACT_TX_COMPLETE, 0, NULL)) {
                    adapter_inter_str[i].state = CON_INTERACT_W4_TX_COMPLETE_CFM_STA;
                }
                break;

            case CON_INTERACT_W4_TX_COMPLETE_CFM_STA:
                if (wireless_cmd_is_empty(i)) {
                    wireless_con_upd_ind(i, 120);
                    wl_skip_first_set(i, 0);
                    adapter_inter_str[i].state = CON_INTERACT_IDLE_STA;
                }
                break;

            default:
                break;
        }
    }
}

void wireless_con_interact_kick(uint8_t index)
{
    if (adapter_inter_str[index].state == CON_INTERACT_IDLE_STA) {
        adapter_inter_str[index].state++;
    }
}

AT(.text.wireless_cmd.private_sync)
void wireless_rx_adapter_con_interact_data(u8 index, wireless_cmd_t *pdu, u8 len)
{
    u8 sub_cmd = pdu->buf[0];

    CON_INTERACT_DEBUG("wireless_rx_adapter_con_interact_data = %d\n", sub_cmd);
    switch(sub_cmd) {
        case CON_INTERACT_GET_INFO|CON_INTERACT_RSP:
            if (adapter_inter_str[index].state == CON_INTERACT_W4_GET_INFO_RSP_STA) {
                adapter_inter_str[index].state = CON_INTERACT_TX_XCFG_PARAM_STA;
            }
            break;

        case CON_INTERACT_RD_RAM|CON_INTERACT_RSP:
            break;

        default:
            break;
    }
}

void wireless_con_interact_init(void)
{
    memset(adapter_inter_str, 0, sizeof(adapter_inter_str_t)*2);
    adapter_inter_str[0].state = adapter_inter_str[1].state = CON_INTERACT_IDLE_STA;
}

void wireless_con_interact_exit(uint8_t index)
{
    memset(&adapter_inter_str[index], 0, sizeof(adapter_inter_str_t));
    adapter_inter_str[index].state = CON_INTERACT_IDLE_STA;
}

#else


static device_inter_str_t device_inter_str;

AT(.text.wireless_cmd.usb)
static bool wl_device_inter_tx(u8 opcode, u8 *buf, u8 buf_len)
{
    inter_head_t tx_head;
    wireless_cmd_t pdu;

    pdu.cmd = PRIVATE_CON_INTERACT_DATA;
    pdu.buf[0] = opcode;
    tx_head.cont_flag = 0;
    tx_head.data_len = buf_len;

    PUT_LE16(&pdu.buf[1], tx_head.param0);

    memcpy(&pdu.buf[3], buf, buf_len);

    return wireless_send_cmd(0, (u8 *)&pdu, buf_len+4);
}

static bool wl_tx_sta_info(u8 cmd_sta, u8 flags)
{
    uint8_t buf[5];

    PUT_LE16(buf, 0x0001);      //version

    buf[2] = cmd_sta;           //last cmd_sta
    buf[3] = CMD_MAX_NB-1;      //free txbuf_nb
    buf[4] = flags;             //flags

    return wl_device_inter_tx((CON_INTERACT_GET_INFO | CON_INTERACT_RSP), buf, 5);
}

AT(.text.wireless_cmd.usb)
static bool wl_con_interact_tx_rd_cmd(uint8_t *buf, uint8_t buf_len)
{
    return wl_device_inter_tx(CON_INTERACT_RD_RAM|CON_INTERACT_RSP, buf, buf_len);
}

void wireless_rx_device_con_interact_data(u8 index, wireless_cmd_t *pdu, u8 len)
{
    u8 opcode = pdu->buf[0];
    u8 *buf = &pdu->buf[1];

    if(opcode != CON_INTERACT_GET_INFO) {
        if(!device_inter_str.cmd_allow) {
            wl_tx_sta_info(CMD_STA_ERR, device_inter_str.flags);
            return;
        }
    }

    wl_inter_rx_proc(buf, opcode, &device_inter_str.inter_ctl, len-2);
}

void wireless_device_interact_init(void)
{
    memset(&device_inter_str, 0x00, sizeof(device_inter_str));
}

uint wl_get_info_prase(u8 *buf, u8 len, inter_ctl_t *ctl)
{
    device_inter_str.cmd_allow = (bool)(*(u32 *)(buf+2) == CMD_ALLOW_KEY);
    wl_tx_sta_info(device_inter_str.state, device_inter_str.flags);
    return CMD_STA_OK;
}

uint wl_tx_complete_prase(uint8_t *buf, u8 len, inter_ctl_t *ctl)
{
    sys_cb.mic_alg_en = 1;
    wl_skip_first_set(0, 0);

    return CMD_STA_OK;
}

uint wl_skip_first_prase(uint8_t *buf, u8 len, inter_ctl_t *ctl)
{
    u8 index = 0;

    wl_skip_first_set(index, buf[0]);

    return CMD_STA_OK;
}

uint wl_rd_ram_prase(u8 *buf, u8 len, inter_ctl_t *ctl)
{
    uint8_t rd_ram_buf[10];

    if(len != 8) {
        return CMD_STA_ERR;
    }

    memcpy((u8 *)&device_inter_str.ram_ctr, buf+2, len-2);

    if (device_inter_str.ram_ctr.len < 10) {
        memcpy(rd_ram_buf, (u8 *)device_inter_str.ram_ctr.addr, device_inter_str.ram_ctr.len);
        wl_con_interact_tx_rd_cmd(rd_ram_buf, device_inter_str.ram_ctr.len);
    } else {
        return CMD_STA_ERR;
    }

    return CMD_STA_OK;
}

#endif

#endif
