/*****************************************************************************
 * Module    : thread_alg
 * File      : thread_alg.c
 * Function  : 低优先级线程回调函数，处理耗时的算法
 * Attention : 允许少量flash缺页
 *****************************************************************************/

#include "include.h"
#include "os_thread.h"


AT(.com_text.thread_alg)
void thread_alg_proc_msg_cb(u32 msg)
{
    switch (msg) {
#if WIRELESS_MIC_YLCRN_L1_32K_EN
        case YLCRN_L1_32K_PROCESS:
            ylcrn_L1_32k_mic_proc_cb();
            break;
#endif // WIRELESS_MIC_YLCRN_L1_32K_EN

        case KICK_ALG_PRIO_TRANS:
            alg_prio_trans_process();
            break;

        case DNN_L1_512FFT_PROCESS:
            dnn_L1_mic_proc_cb();
            break;

        case DNN_L2_PROCESS:
            dnn_L2_mic_proc_cb();
            break;

#if WIRELESS_MIC_DNN_L3_EN || ADAPTER_MIC_DNN_L3_EN
        case DNN_L3_PROCESS:
            dnn_L3_mic_proc_cb();
            break;
#endif

#if WIRELESS_MIC_DNN_L3_32K_EN
        case DNN_L3_32K_PROCESS:
            dnn_L3_32k_mic_proc_cb();
            break;
#endif // WIRELESS_MIC_DNN_L3_32K_EN

#if USER_ALG_EN
        case USER_PROCESS:
            user_mic_proc_cb();
            break;
#endif

#if WIRELESS_MIC_AINS4_EN
        case AINS4_48K_PROCESS:
            ains4_mic_proc_cb();
            break;
#endif // WIRELESS_MIC_AINS4_EN

        default:
            break;
    }
}
