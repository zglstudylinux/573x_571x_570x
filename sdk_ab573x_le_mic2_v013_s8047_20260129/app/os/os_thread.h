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
};

void os_dec_sem_pend(void);
void os_dec_sem_post(void);
void os_send_dec_proc_msg(u32 msg);
#define kick_decoder_prio_trans(idx)            os_send_dec_proc_msg(KICK_DEC_PRIO_TRANS + (idx))


//------------------------------------------------------------------------------------------
//中优先级（要求在一帧时间内完成）
enum {
    KICK_ENC_PRIO_TRANS     = 0,
    KICK_ENC_BUF_PROC,
};

void os_send_enc_proc_msg(u32 msg);
#define kick_encoder_prio_trans()               os_send_enc_proc_msg(KICK_ENC_PRIO_TRANS)


//------------------------------------------------------------------------------------------
//低优先级
enum {
    KICK_ALG_PRIO_TRANS = 0,
    DNN_L1_512FFT_PROCESS,
    DNN_L2_PROCESS,
    DNN_L3_PROCESS,
    DNN_L3_32K_PROCESS,
    USER_PROCESS,
    YLCRN_L1_32K_PROCESS,
    AINS4_48K_PROCESS,
};

void os_alg_sem_pend(uint timeout);             //alg线程等待信号量，timeout时间>=2（单位5ms）
void os_alg_sem_post(void);
void os_send_alg_proc_msg(u32 msg);
#define ains4_mic_proc_kick_start()             os_send_alg_proc_msg(AINS4_48K_PROCESS)
#define kick_alg_prio_trans()                   os_send_alg_proc_msg(KICK_ALG_PRIO_TRANS)
#define dnn_L1_mic_proc_kick_start()            os_send_alg_proc_msg(DNN_L1_512FFT_PROCESS)
#define dnn_L2_mic_proc_kick_start()            os_send_alg_proc_msg(DNN_L2_PROCESS)
#define dnn_L3_mic_proc_kick_start()            os_send_alg_proc_msg(DNN_L3_PROCESS)
#define dnn_L3_32k_mic_proc_kick_start()        os_send_alg_proc_msg(DNN_L3_32K_PROCESS)
#define ylcrn_L1_32k_mic_proc_kick_start()      os_send_alg_proc_msg(YLCRN_L1_32K_PROCESS)
#define user_mic_proc_kick_start()              os_send_alg_proc_msg(USER_PROCESS)
#endif // __THREAD_ALG_
