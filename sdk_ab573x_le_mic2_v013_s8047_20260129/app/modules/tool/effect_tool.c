#include "include.h"
#include "effect.h"

#define XTP_EFFECT_FRAME_HEAD_TAG           12
#define CAR_TOOL_SEED                       0xffff

void tx_ack(uint8_t *packet, uint16_t len);
u8 check_sum(u8 *buf, u16 size);

static u8 effect_tx_ack_buf[14] AT(.buf.set_effect.ack_buf);

AT(.text.bsp_set_effect)
static void bsp_effect_tx_ack(u8 *head_tag, u8 ack)
{
    head_tag[12] = ack;
    head_tag[13] = check_sum(head_tag, 13);
    tx_ack(head_tag, 14);
}

AT(.text.bsp_set_effect)
void bsp_effect_parse_cmd(void)
{
//    printf("-->%s\n", __func__);
//    print_r(eq_rx_buf, 20);
    u8 tool_ack = 0;
    u16 size = little_endian_read_16(eq_rx_buf, 12);
    u32 cal_crc = calc_crc(eq_rx_buf, size+XTP_EFFECT_FRAME_HEAD_TAG, CAR_TOOL_SEED);
    u32 res_crc = little_endian_read_16(eq_rx_buf, size+XTP_EFFECT_FRAME_HEAD_TAG);

#if BT_MUSIC_EFFECT_EN
    music_effect_audio_start();
#endif

    memcpy(effect_tx_ack_buf, eq_rx_buf, 12);

    do {
        if (cal_crc != res_crc) {
            printf("-->CRC_ERROR %x %x\n", res_crc, cal_crc);
            tool_ack = 1;           //crc校验错误
            break;
        }

        if (0 == memcmp((char *)&eq_rx_buf[0], "CFG_########", 12)) {       //全部发送，检查链路是否一致
            if ((u32)little_endian_read_16(eq_rx_buf, 14) != ALL_MODE_NAME_SUM) {
                tool_ack = 1;       //与上位机链路不匹配
            }
            break;
        }

        for(int i = 0; i < CFG_MAX; i++){
            if (0 == memcmp((char *)&eq_rx_buf[0], effect_info[i].effect_cfg_name, 12)) {
                printf("%s Online\n", effect_info[i].effect_cfg_name);
                if (effect_update_callback_tbl[i].effect_update_callback) {
                    effect_update_callback_tbl[i].effect_update_callback(&eq_rx_buf[14], little_endian_read_16(eq_rx_buf, 12), 1);
                } else {
                    printf("%s NO INIT CALLBACK\n", effect_info[i].effect_cfg_name);
                }
                break;
            }
            if (i == CFG_MAX-1) {
                tool_ack = 1;       //名称错误，没有对应模块
            }
        }
    } while(0);

#if BT_TWS_EN
    if ((bt_tws_is_connected() && eq_dbg_cb.rx_type) || (!eq_dbg_cb.rx_type && !bt_tws_is_slave())) {
        bt_tws_sync_eq_param();
    }
#endif // BT_TWS_EN

    bsp_effect_tx_ack(effect_tx_ack_buf, tool_ack);
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
}
