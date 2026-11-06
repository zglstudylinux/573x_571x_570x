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
#if WIRELESS_MIC_AINS4_EN || ADAPTER_MIC_AINS4_EN
        case AINS4_PROCESS:
            ains4_mic_proc_cb();
            break;
#endif

#if WIRELESS_MIC_DNN_L3_EN || ADAPTER_MIC_DNN_L3_EN
        case DNN_L3_PROCESS:
            dnn_L3_mic_proc_cb();
            break;
#endif

#if WIRELESS_MIC_DNN_L2_EN || ADAPTER_MIC_DNN_L2_EN
        case DNN_L2_PROCESS:
            dnn_L2_mic_proc_cb();
            break;
#endif

#if ADAPTER_HOWLING_DNN_EN
        case HOWLING_DNN_PROCESS:
            howling_dnn_mic_proc_cb();
            break;
#endif

#if ADAPTER_LOCAL_MIC_EN
        case ADAPTER_LOCALMIC_PROCESS:
            local_mic_effect_process();
            break;
#endif
        case DNR_FRE_PROCESS:
            dnr_fre_mic_proc_cb();
            break;

        case KICK_ALG_PRIO_TRANS:
            alg_prio_trans_process();
            break;

        default:
            break;
    }
}
