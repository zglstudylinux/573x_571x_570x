#include "include.h"
#include "tws_res.h"


#if BT_TWS_EN


#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


void res_play_add_cb_do(uint16_t param);
void res_play_done_cb_do(uint16_t param);


//----------------------------------------------------------------------------
//callback

//主耳发完消息后，把提示音添加到本地列表
void tws_local_res_add_cb(uint16_t param)
{
    uint tws_flag = (uint8_t)(param>>12) & RES_FLAG_TWS;

    //主耳检查本地是否需要播放
    if(tws_flag != RES_FLAG_TWS) {
        if(tws_flag & RES_FLAG_LOCAL) {
            tws_flag = RES_FLAG_LOCAL;     //设置本地播放标志
        } else {
            tws_flag = 0;
        }
    }
    param &= ~(RES_FLAG_TWS<<12);
    param |= tws_flag<<12;

    res_play_add_cb_do(param);
}

//副耳收到消息后，把提示音添加到本地列表
void tws_remote_res_add_cb(uint16_t param)
{
    uint tws_flag = (uint8_t)(param>>12) & RES_FLAG_TWS;

    //副耳检查本地是否需要播放
    if(tws_flag != RES_FLAG_TWS) {
        if(tws_flag & RES_FLAG_REMOTE) {
            tws_flag = RES_FLAG_LOCAL;     //设置本地播放标志
        } else {
            tws_flag = 0;
        }
    }
    param &= ~(RES_FLAG_TWS<<12);
    param |= tws_flag<<12;

    res_play_add_cb_do(param);
}

void tws_remote_res_done_cb(uint16_t param)
{
    res_play_done_cb_do(param);
}

//副耳发起提示音请求，由主耳控制同步播报
void tws_slave_res_add_req_cb(uint16_t param)
{
    uint8_t res_idx = (uint8_t)param;
    uint8_t res_flag = (uint8_t)(param>>12) & 0xf;
    uint8_t rem_flag = (res_flag & RES_FLAG_REMOTE);
    uint8_t loc_flag = (res_flag & RES_FLAG_LOCAL);

    if(!bt_tws_is_slave()) {
        switch(res_idx) {
        case RES_IDX_MAX_VOL:
        case RES_IDX_LOW_BATTERY:
        case RES_IDX_LANGUAGE_EN:
        case RES_IDX_LANGUAGE_ZH:
        case RES_IDX_CALL_HANGUP:
        case RES_IDX_CALL_REJECT:
        case RES_IDX_REDIALING:
        case RES_IDX_MUSIC_MODE:
        case RES_IDX_GAME_MODE:
#if ANC_EN
        case RES_IDX_NR_DISABLE:
        case RES_IDX_ANC:
        case RES_IDX_TRANSPARENCY:
#endif
            //副耳发起的提示音，对调一下属性，再由主耳控制播报
            res_flag &= ~RES_FLAG_TWS;
            if(rem_flag) {
                res_flag |= RES_FLAG_LOCAL;
            }
            if(loc_flag) {
                res_flag |= RES_FLAG_REMOTE;
            }
            bsp_res_play_do(res_idx, res_flag);
            break;
        }
    }
}
#endif // BT_TWS_EN
