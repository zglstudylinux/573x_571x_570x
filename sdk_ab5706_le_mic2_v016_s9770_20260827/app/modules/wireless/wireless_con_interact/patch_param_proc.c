#include "include.h"
#include "wireless.h"

#if WIRELESS_EN && ADAPTER_SAVE_PARAM_EN
typedef struct __attribute__((packed)) {
    inter_head_t head;
    void *addr;
    u8 data[];
} ram_cmd_t;

uint wl_wr_ram_prase(u8 *buf, u8 len, inter_ctl_t *ctl)
{
    ram_cmd_t *p = (ram_cmd_t *)buf;
    if(len < 6) {
        return CMD_STA_ERR;
    }

    //获取写RAM地址、长度
    ctl->ptr      = p->addr;
    ctl->remain   = p->head.data_len;
    ctl->offset   = 0;

    return CMD_STA_RX;
}

//NO_INLINE void test_printf(u8 *data, u8 len)
//{
//    printf("test_printf\n");
//}

void wl_wr_ram_rxdone(inter_ctl_t *ctl)
{
//    test_printf(0, 0);
    //print_r(ctl->ptr, 40);
}

uint wl_set_run_prase(u8 *buf, u8 len, inter_ctl_t *ctl)
{
    ram_cmd_t *p = (void *)buf;
    if(len < 6) {
        return CMD_STA_ERR;
    }

    //获取运行地址及参数
    ((void (*)(u8 *, u8))p->addr)(p->data, len-6);

    return CMD_STA_OK;
}

#endif
