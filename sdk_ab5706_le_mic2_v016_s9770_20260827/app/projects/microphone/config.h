/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H


//用户可选 CONFIG 方案配置
#define CONFIG_AB5732E_LE_MIC_RX       0          //本设备作为接收端，搭配SDK_AB5732E_LE_MIC TX端的无线话筒
#define CONFIG_AB5706A_LE_MIC_RX       1          //本设备作为接收端，搭配SDK_AB5706A_LE_MIC TX端的无线麦
#define CONFIG_AB5706A_LE_MIC_TX       2          //本设备作为发射端，搭配SDK_AB5706A_LE_MIC RX端的无线麦
#define CONFIG_AB5666_LE_MIC_RX        3          //本设备作为接收端，搭配SDK_AB5666C_LE_MIC TX端的无线麦
#define CONFIG_AB570X_LE_MIC_TX        4          //本设备作为发射端，搭配SDK_AB570X_LE_MIC RX端的无线麦
#define CONFIG_LOW_LATENCY_RX          5          //本设备作为接收端，低延时一拖一方案
#define CONFIG_LOW_LATENCY_TX          6          //本设备作为发射端，低延时一拖一方案
#define CONFIG_AB5766_LE_MIC_RX        7          //本设备作为接收端，搭配SDK_AB5766C_LE_MIC TX端的无线麦
#define CONFIG_AB5706A_INTERPHONE      8          //本设备作为对讲机方案

#define USER_CONFIG                    CONFIG_AB5732E_LE_MIC_RX

#if (USER_CONFIG == CONFIG_AB5706A_LE_MIC_RX)
    #include "config_ab5706a_rx.h"
#elif (USER_CONFIG == CONFIG_AB5706A_LE_MIC_TX)
    #include "config_ab5706a_tx.h"
#elif (USER_CONFIG == CONFIG_AB5732E_LE_MIC_RX)
    #include "config_ab5732e_rx.h"
#elif (USER_CONFIG == CONFIG_AB5666_LE_MIC_RX)
    #include "config_ab5666_rx.h"
#elif (USER_CONFIG == CONFIG_AB570X_LE_MIC_TX)
    #include "config_ab570x_tx.h"
#elif (USER_CONFIG == CONFIG_LOW_LATENCY_RX)
    #include "config_low_latency_rx.h"
#elif (USER_CONFIG == CONFIG_LOW_LATENCY_TX)
    #include "config_low_latency_tx.h"
#elif (USER_CONFIG == CONFIG_AB5766_LE_MIC_RX)
    #include "config_ab5766_rx.h"
#elif (USER_CONFIG == CONFIG_AB5706A_INTERPHONE)
    #include "config_ab5706a_interphone.h"
#else
    #include "config_ab5706a_rx.h"
#endif


#endif

