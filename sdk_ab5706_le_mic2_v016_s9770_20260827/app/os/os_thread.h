#ifndef __OS_THREAD_
#define __OS_THREAD_

#include "decoder_prio_trans.h"
#include "encoder_prio_trans.h"
#include "alg_prio_trans.h"

//------------------------------------------------------------------------------------------
//高优先级（要求在一帧时间内完成）
enum {
    KICK_DEC_PRIO_TRANS     = 0,
    KICK_DEC_PRIO_TRANS1,

    KICK_DEC_WARNING        = 0x20,

    KICK_DEC_MAX_MSG        = 0x3f,
};

void os_send_dec_proc_msg(u32 msg);
#define kick_decoder_prio_trans(idx)            os_send_dec_proc_msg(KICK_DEC_PRIO_TRANS + (idx))

//------------------------------------------------------------------------------------------
//高优先级（要求在一帧时间内完成）
enum {
    KICK_ENC_PRIO_TRANS     = 0,

    KICK_ENC_MAX_MSG        = 0x3f,
};

void os_send_enc_proc_msg(u32 msg);
#define kick_encoder_prio_trans()               os_send_enc_proc_msg(KICK_ENC_PRIO_TRANS)


//------------------------------------------------------------------------------------------
//低优先级
enum {
    AINS4_PROCESS           = 0,
    DNN_L2_PROCESS,
    DNN_L3_PROCESS,
    HOWLING_DNN_PROCESS,
    ADAPTER_LOCALMIC_PROCESS,
    DNR_FRE_PROCESS,
    KICK_ALG_PRIO_TRANS,
};

void os_alg_sem_pend(uint timeout);             //alg线程等待信号量，timeout单位5ms
void os_alg_sem_post(void);                     //alg线程发送信号量
void os_send_alg_proc_msg(u32 msg);
#define kick_alg_prio_trans()                   os_send_alg_proc_msg(KICK_ALG_PRIO_TRANS)
#define ains4_mic_proc_kick_start()             os_send_alg_proc_msg(AINS4_PROCESS)
#define dnn_L2_mic_proc_kick_start()            os_send_alg_proc_msg(DNN_L2_PROCESS)
#define dnn_L3_mic_proc_kick_start()            os_send_alg_proc_msg(DNN_L3_PROCESS)
#define howling_dnn_mic_proc_kick_start()       os_send_alg_proc_msg(HOWLING_DNN_PROCESS)
#define kick_loc_mic_effect_proc()              os_send_alg_proc_msg(ADAPTER_LOCALMIC_PROCESS)
#define dnr_fre_mic_proc_kick_start()           os_send_alg_proc_msg(DNR_FRE_PROCESS)

#endif // __THREAD_ALG_
