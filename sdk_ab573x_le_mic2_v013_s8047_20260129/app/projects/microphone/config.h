/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H


//用户可选 CONFIG 方案配置
#define CONFIG_AB5732E_LE_MIC       0          //搭配SDK_AB5732E_LE_MIC的无线话筒
#define CONFIG_AB5706A_LE_MIC       1          //搭配SDK_AB5706A_LE_MIC的无线麦
#define CONFIG_AB5712F_LE_MIC       2          //搭配SDK_AB5712F_LE_MIC的无线麦
#define CONFIG_LOW_LATENCY          3          //低延时一拖一方案
#define CONFIG_AB5732E_INTERPHONE   4          //搭配SDK_AB5732E的对讲机，配置工具都选择wireless_mic_emit烧录
#define CONFIG_AB5700_KBOX          5          //搭配SDK_AB5700_KBOX的无线话筒
#define CONFIG_AB5732E_DIFF_CON_VERS 6         //搭配SDK_AB5732E_LE_MIC，一拖二的两路的con_vers不同的配置
#define USER_CONFIG                 CONFIG_AB5706A_LE_MIC


#if (USER_CONFIG == CONFIG_AB5732E_LE_MIC)
    #include "config_ab5732e_le_mic.h"
#elif (USER_CONFIG == CONFIG_AB5706A_LE_MIC)
    #include "config_ab5706a_le_mic.h"
#elif (USER_CONFIG == CONFIG_AB5712F_LE_MIC)
    #include "config_ab5712f_le_mic.h"
#elif (USER_CONFIG == CONFIG_LOW_LATENCY)
    #include "config_low_latency.h"
#elif (USER_CONFIG == CONFIG_AB5732E_INTERPHONE)
    #include "config_ab5732e_interphone.h"
#elif (USER_CONFIG == CONFIG_AB5700_KBOX)
    #include "config_ab5700_kbox.h"
#elif (USER_CONFIG == CONFIG_AB5732E_DIFF_CON_VERS)
    #include "config_ab5732e_diff_con_vers.h"
#else
    #include "config_ab5732e_le_mic.h"
#endif


#endif
