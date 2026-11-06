#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : SDK版本配置
 *****************************************************************************/
#ifndef SDK_VERSION
    #define SDK_VERSION                 0x0160      //V0160
#endif

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#undef  SYS_ADJ_DIGVOL_EN
#define SYS_ADJ_DIGVOL_EN               1

#ifndef SYS_MODE_BREAKPOINT_EN
#define SYS_MODE_BREAKPOINT_EN          0
#endif // SYS_MODE_BREAKPOINT_EN

#ifndef ENERGY_LED_EN
#define ENERGY_LED_EN                   0
#endif

#ifndef PLUGIN_SYS_INIT_FINISH_CALLBACK
#define PLUGIN_SYS_INIT_FINISH_CALLBACK 0
#endif

#ifndef PLUGIN_FUNC_IDLE_ENTER_CHECK
#define PLUGIN_FUNC_IDLE_ENTER_CHECK    0
#endif

#ifndef DAC_OFF_FOR_BT_CONN_EN
#define DAC_OFF_FOR_BT_CONN_EN          0       //蓝牙连接时关闭DAC
#endif

#ifndef FLASH_SPEED_UP_EN
#define FLASH_SPEED_UP_EN               1
#endif

#ifndef MUSIC_AAC_SUPPORT
#define MUSIC_AAC_SUPPORT               0
#endif // MUSIC_AAC_SUPPORT

#ifndef USER_NTC
#define USER_NTC                        0
#endif


#if !EQ_DBG_IN_UART
#undef EQ_DBG_IN_UART_VUSB_EN
#define EQ_DBG_IN_UART_VUSB_EN          0
#endif


/*****************************************************************************
 * Module    :  UART功能选择（待整理）
 *****************************************************************************/
#if BT_AEC_DUMP_EN || BT_AEC_FRE_DUMP_EN || BT_SCO_DUMP_EN || BT_EQ_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_PLC_DUMP_EN
    #define BT_SCO_DUMP_TX_EN           1
#else
    #define BT_SCO_DUMP_TX_EN           0
#endif

#if (TEST_INTF_SEL == INTF_HUART) || EQ_DBG_IN_UART || BT_SCO_DUMP_TX_EN || QTEST_EN || FUNC_LE_DUT_EN/*(强制打开HUART)*/
    #define HUART_EN                    1
#endif

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#if !FUNC_MUSIC_EN
#undef  MUSIC_UDISK_EN
#undef  MUSIC_SDCARD_EN

#undef  MUSIC_WAV_SUPPORT
#undef  MUSIC_WMA_SUPPORT
#undef  MUSIC_APE_SUPPORT
#undef  MUSIC_FLAC_SUPPORT
#undef  MUSIC_SBC_SUPPORT
#undef  MUSIC_M4A_SUPPORT
#undef  MUSIC_AAC_SUPPORT
#undef  MUSIC_ID3_TAG_EN
#undef  MUSIC_FOLDER_SELECT_EN
#undef  MUSIC_AUTO_SWITCH_DEVICE
#undef  MUSIC_BREAKPOINT_EN
#undef  MUSIC_MODE_RETURN
#undef  MUSIC_PLAYDEV_BOX_EN
#undef  MUSIC_ID3_TAG_EN
#undef  MUSIC_REC_FILE_FILTER
#undef  MUSIC_LRC_EN
#undef  MUSIC_NAVIGATION_EN
#undef  MUSIC_ENCRYPT_EN

#define MUSIC_UDISK_EN              0
#define MUSIC_SDCARD_EN             0
#define MUSIC_WAV_SUPPORT           0
#define MUSIC_WMA_SUPPORT           0
#define MUSIC_APE_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define MUSIC_M4A_SUPPORT           0
#define MUSIC_SBC_SUPPORT           0
#define MUSIC_ID3_TAG_EN            0
#define MUSIC_AAC_SUPPORT           0
#define MUSIC_FOLDER_SELECT_EN      0
#define MUSIC_AUTO_SWITCH_DEVICE    0
#define MUSIC_BREAKPOINT_EN         0
#define MUSIC_QSKIP_EN              0
#define MUSIC_MODE_RETURN           0
#define MUSIC_PLAYDEV_BOX_EN        0
#define MUSIC_ID3_TAG_EN            0
#define MUSIC_REC_FILE_FILTER       0
#define MUSIC_LRC_EN                0
#define MUSIC_NAVIGATION_EN         0
#define MUSIC_ENCRYPT_EN            0
#endif // FUNC_MUSIC_EN

#if !CHARGE_EN
#undef  CHARGE_TRICK_EN
#undef  CHARGE_DC_IN
#undef  CHARGE_DC_NOT_PWRON
#define CHARGE_TRICK_EN             0
#define CHARGE_DC_NOT_PWRON         0
#define CHARGE_DC_IN()              0
#endif

#ifndef PWROFF_PRESS_TIME
#define PWROFF_PRESS_TIME           3               //1:大约1秒    3: 1.5s,  6: 2s,  9: 2.5s, 12: 3s
#endif // PWROFF_PRESS_TIME

#ifndef PWRON_PRESS_TIME
#define PWRON_PRESS_TIME            1500           //1.5s
#endif // PWRON_PRESS_TIME

#ifndef DOUBLE_KEY_TIME
#define DOUBLE_KEY_TIME             1
#endif // DOUBLE_KEY_TIME

#ifndef LPWR_WARING_TIMES
#define LPWR_WARING_TIMES           0xff
#endif // LPWR_WARING_TIMES

#if !BUZZER_EN
#undef BUZZER_INIT
#define BUZZER_INIT()
#undef BUZZER_ON
#define BUZZER_ON()
#undef BUZZER_OFF
#define BUZZER_OFF()
#endif

#ifndef LED_LOWBAT_EN
#define LED_LOWBAT_EN              0
#endif // RLED_LOWBAT_EN

#ifndef RLED_LOWBAT_FOLLOW_EN
#define RLED_LOWBAT_FOLLOW_EN       0
#endif // RLED_LOWBAT_FOLLOW_EN

#ifndef SLEEP_DAC_OFF_EN
#define SLEEP_DAC_OFF_EN            1
#endif // SLEEP_DAC_OFF_EN

#ifndef PWRON_FRIST_BAT_EN
#define PWRON_FRIST_BAT_EN          1
#endif // PWRON_FRIST_BAT_EN

#if !LED_DISP_EN
#undef LED_LOWBAT_EN
#undef RLED_LOWBAT_FOLLOW_EN
#undef BLED_FFT_EN
#undef BLED_LOW2ON_EN
#undef BLED_CHARGE_FULL_EN
#undef BT_RECONN_LED_EN
#define LED_LOWBAT_EN              0
#define RLED_LOWBAT_FOLLOW_EN       0
#define BLED_FFT_EN                 0
#define BLED_LOW2ON_EN              0
#define BLED_CHARGE_FULL_EN         0
#define BT_RECONN_LED_EN            0
#endif

#if !LED_DISP_EN
#undef LED_INIT
#define LED_INIT()
#undef LED_SET_ON
#define LED_SET_ON()
#undef LED_SET_OFF
#define LED_SET_OFF()
#endif

#if !LED_PWR_EN
#undef LED_PWR_INIT
#define LED_PWR_INIT()
#undef LED_PWR_SET_ON
#define LED_PWR_SET_ON()
#undef LED_PWR_SET_OFF
#define LED_PWR_SET_OFF()
#endif

#undef GUI_LCD_EN
#define GUI_LCD_EN                      0
#undef  MUSIC_NAVIGATION_EN
#define MUSIC_NAVIGATION_EN             0

#ifndef LOUDSPEAKER_MUTE_EN
#define LOUDSPEAKER_MUTE_EN             0
#endif

#if !LOUDSPEAKER_MUTE_EN
#undef LOUDSPEAKER_MUTE_INIT
#define LOUDSPEAKER_MUTE_INIT()
#undef LOUDSPEAKER_MUTE_DIS
#define LOUDSPEAKER_MUTE_DIS()
#undef LOUDSPEAKER_MUTE
#define LOUDSPEAKER_MUTE()
#undef LOUDSPEAKER_UNMUTE
#define LOUDSPEAKER_UNMUTE()
#undef LOUDSPEAKER_UNMUTE_DELAY
#define LOUDSPEAKER_UNMUTE_DELAY        0
#endif

#ifndef IR_INPUT_NUM_MAX
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999
#endif // IR_INPUT_NUM_MAX

#ifndef FMRX_SEEK_DISP_CH_EN
#define FMRX_SEEK_DISP_CH_EN            0
#endif // FMRX_SEEK_DISP_CH_EN

#ifndef SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN               0
#undef SD_IS_SOFT_DETECT
#define SD_IS_SOFT_DETECT()             0
#endif // SD_SOFT_DETECT_EN

#if ADKEY_MUX_LED_EN
#undef  ADKEY_EN
#undef  ADKEY_MUX_SDCLK_EN
#undef  ADKEY_PU10K_EN

#define ADKEY_EN                      1
#define ADKEY_MUX_SDCLK_EN            0
#define ADKEY_PU10K_EN                  0
#endif // ADKEY_MUX_LED_EN

#if ((SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G4)) && MUSIC_UDISK_EN && MUSIC_SDCARD_EN
#define SD_USB_MUX_IO_EN				1
#else
#define SD_USB_MUX_IO_EN				0
#endif

#ifndef DAC_DRC_EN
#define DAC_DRC_EN                      0
#endif

#if !PWRKEY_EN
#undef PWRKEY_IS_PRESS
#define PWRKEY_IS_PRESS()               0
#endif

#if !TKEY_EN
#undef  TKEY_SOFT_PWR_EN
#undef  TKEY_INEAR_EN
#undef  TKEY_DEBUG_EN
#undef  TKEY_IS_PRESS
#undef  TKEY_TEMP_EN
#define TKEY_SOFT_PWR_EN                0
#define TKEY_INEAR_EN                   0
#define TKEY_DEBUG_EN                   0
#define TKEY_IS_PRESS()                 0
#define TKEY_TEMP_EN                    0
#endif

#if TKEY_INEAR_EN
#undef  TKEY_TEMP_EN
#define TKEY_TEMP_EN                    1
#endif // TKEY_INEAR_EN

#if !TKEY_SOFT_PWR_EN
#undef TKEY_LOWPWR_WAKEUP_DIS
#define TKEY_LOWPWR_WAKEUP_DIS          0
#endif

#if !USER_INEAR_DETECT_EN
#undef TKEY_INEAR_EN
#undef USER_INEAR_DET_OPT
#undef INEAR_IS_ONLINE
#define TKEY_INEAR_EN                   0
#define USER_INEAR_DET_OPT              0
#define INEAR_IS_ONLINE()               0
#endif

#if !USER_INEAR_DET_OPT
#undef INEAR_OPT_PORT_INIT
#define INEAR_OPT_PORT_INIT()
#endif

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#if !FUNC_REC_EN
#undef  FMRX_REC_EN
#undef  AUX_REC_EN
#undef  MIC_REC_EN
#undef  REC_TYPE_SEL
#undef  REC_AUTO_PLAY
#undef  REC_FAST_PLAY
#undef  BT_REC_EN
#undef  BT_HFP_REC_EN
#undef  KARAOK_REC_EN
#undef  REC_STOP_MUTE_1S

#define FMRX_REC_EN                 0
#define AUX_REC_EN                  0
#define MIC_REC_EN                  0
#define REC_AUTO_PLAY               0
#define REC_FAST_PLAY               0
#define BT_REC_EN                   0
#define BT_HFP_REC_EN               0
#define KARAOK_REC_EN               0
#define REC_TYPE_SEL                REC_NO
#define REC_STOP_MUTE_1S            0
#endif //FUNC_REC_EN


#if FMRX_INSIDE_EN
#undef  FMRX_QN8035_EN
#define FMRX_QN8035_EN              0
//#else
//#undef  I2C_SW_EN
//#define I2C_SW_EN                   1
#endif

#if !I2C_SW_EN
#undef  I2C_MUX_SD_EN
#define I2C_MUX_SD_EN               0
#endif


/*****************************************************************************
 * Module    : karaok相关配置
 *****************************************************************************/
#if SYS_KARAOK_EN
#undef DAC_DNR_EN
#undef BT_HFP_REC_EN

#define DAC_DNR_EN                  0   //暂时先关掉动态降噪
#define BT_HFP_REC_EN               0   //Karaok不支持通话录音

#else
#undef SYS_ECHO_EN
#undef HIFI4_ECHO_EN
#undef HIFI4_REVERB_EN
#undef SYS_MAGIC_VOICE_EN
#undef HIFI4_PITCH_SHIFT_EN
#undef HIFI4_HOWLING_EN
#undef SYS_HOWLING_EN
#undef KARAOK_REC_EN

#define SYS_ECHO_EN                 0
#define HIFI4_ECHO_EN               0
#define HIFI4_REVERB_EN             0
#define SYS_MAGIC_VOICE_EN          0
#define HIFI4_PITCH_SHIFT_EN        0
#define HIFI4_HOWLING_EN            0
#define SYS_HOWLING_EN              0
#define KARAOK_REC_EN               0
#endif // SYS_KARAOK_EN

#if SYS_HOWLING_EN
#undef SYS_MAGIC_VOICE_EN
#define SYS_MAGIC_VOICE_EN          1
#endif


/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/
#if (LE_AB_LINK_APP_EN && AB_MATE_APP_EN)
#error "APP: please don't open LE_AB_LINK_APP_EN & AB_MATE_APP_EN at the same time\n"
#endif

#if (LE_AB_LINK_APP_EN && LE_USER_APP_EN)
#error "APP: please don't open LE_AB_LINK_APP_EN & LE_USER_APP_EN at the same time\n"
#endif

#if (AB_MATE_APP_EN && LE_USER_APP_EN)
#error "APP: please don't open AB_MATE_APP_EN & LE_USER_APP_EN at the same time\n"
#endif


#if LE_AB_LINK_APP_EN || AB_MATE_APP_EN || LE_BQB_RF_EN || GFPS_EN || LE_USER_APP_EN || LE_PRIV_EN || LE_FCC_TEST_EN
    #define LE_EN                   1
    #define BT_DUAL_MODE_EN         1
#else
    #define LE_EN                   0
    #define BT_DUAL_MODE_EN         0
#endif

#if GFPS_EN
    #define LE_ADDRESS_TYPE         3       //GAP_RANDOM_ADDRESS_RESOLVABLE
#else
    #define LE_ADDRESS_TYPE         0       //GAP_RANDOM_ADDRESS_TYPE_OFF
#endif

#if !LE_EN
#undef LE_AB_FOT_EN
#undef LE_PAIR_EN
#undef LE_SM_SC_EN
#define LE_AB_FOT_EN                0
#define LE_PAIR_EN                  0
#define LE_SM_SC_EN                 0
#endif

#if GFPS_EN
#undef LE_ADV_POWERON_EN
#define LE_ADV_POWERON_EN          0
#endif

#ifndef BT_A2DP_RECON_EN
#define BT_A2DP_RECON_EN            0
#endif

#ifndef BT_PAIR_SLEEP_EN
#define BT_PAIR_SLEEP_EN            0
#endif // BT_PAIR_SLEEP_EN

#ifndef BT_HFP_RINGS_BEFORE_NUMBER
#define BT_HFP_RINGS_BEFORE_NUMBER  0
#endif

#ifndef BT_A2DP_AAC_AUDIO_EN
#define BT_A2DP_AAC_AUDIO_EN        0
#endif // BT_A2DP_AAC_AUDIO_EN

#if !BT_A2DP_EN
#undef  BT_A2DP_AAC_AUDIO_EN
#define BT_A2DP_AAC_AUDIO_EN        0
#endif

#if BT_FCC_TEST_EN || LE_BQB_RF_EN || LE_FCC_TEST_EN    //FCC 默认PB3 (USB_DP) 波特率1500000通信, 关闭用到PB3的程序
#undef FUNC_USBDEV_EN
#undef MUSIC_UDISK_EN
#define FUNC_USBDEV_EN             0
#define MUSIC_UDISK_EN             0
//#if (UART0_PRINTF_SEL == PRINTF_PB3)
//#undef UART0_PRINTF_SEL
//#define UART0_PRINTF_SEL  PRINTF_NONE
//#endif
#endif

#if !BT_TWS_EN
#undef BT_TWS_SCO_EN
#define BT_TWS_SCO_EN               0
#undef BT_TWS_MS_SWITCH_EN
#define BT_TWS_MS_SWITCH_EN         0
#undef FOT_SUPPORT_TWS
#define FOT_SUPPORT_TWS               0
#undef BT_TWS_DBG_EN
#define BT_TWS_DBG_EN               0
#endif

#if BT_TWS_DBG_EN
    #if !BT_SPP_EN
    #error "TWS_DBG: please open BT_TWS_DBG_EN & BT_SPP_EN at the same time\n"
    #endif
#endif // BT_TWS_DBG_EN

#if AB_MATE_APP_EN
#undef EQ_APP_EN
#undef BT_A2DP_AVRCP_PLAY_STATUS_EN
#define EQ_APP_EN                   1
#define BT_A2DP_AVRCP_PLAY_STATUS_EN    1

#undef LE_AB_FOT_EN
#undef BT_AB_FOT_EN
#define LE_AB_FOT_EN                0
#define BT_AB_FOT_EN                0
#endif

#if BT_AB_FOT_EN || LE_AB_FOT_EN
#undef FOT_EN
#define FOT_EN                      1   //是否打开FOTA升级功能
#endif

#if BT_AB_FOT_EN
#if !BT_SPP_EN
#error "FOTA: please open BT_AB_FOT_EN & BT_SPP_EN at the same time\n"
#endif
#endif // BT_AB_FOT_EN

#if BT_HID_VOL_CTRL_EN
#undef  BT_HID_EN
#undef  BT_A2DP_VOL_CTRL_EN

#define BT_HID_EN                    1
#define BT_A2DP_VOL_CTRL_EN          1
#endif // BT_HID_VOL_CTRL_EN

#if !EQ_APP_EN
#undef EQ_APP_NUM
#define EQ_APP_NUM                      0
#endif

/*****************************************************************************
 * Module    : 音乐算法配置
 *****************************************************************************/
#if GLOBAL_MUSIC_EFFECT_EN
#undef EFFECT_DBG_ADJUST_EN
#undef EFFECT_DBG_ADJUST_IN_UART
#define EFFECT_DBG_ADJUST_EN           1
#define EFFECT_DBG_ADJUST_IN_UART      1
#endif

/*****************************************************************************
 * Module    : 通话算法相关配置
 *****************************************************************************/
#define BT_SCO_SMIC_EN                  0
#define BT_SCO_DMIC_EN                  0

#define BT_SCO_SMIC_AI_EN               0                           //是否打开自研单麦AI降噪算法
#define BT_SCO_SMIC_AI_LEVEL		    0                           //降噪量：0~40级（建议范围，默认0级）

#define BT_SCO_NR_USER_SMIC_EN          0                           //是否打开自定义单麦降噪功能
#define BT_SCO_AEC_USER_EN              0                           //是否打开自定义AEC功能，需要打开 BT_SCO_NR_USER_SMIC_EN

#define BT_SCO_FAR_NR_EN                0                           //是否打开远端降噪算法
#define BT_SCO_FAR_NOISE_LEVEL          5                           //降噪等级（0~5，越大降噪效果越好，音质越差）
#define BT_SCO_FAR_THR                  1                           //范围: 0~20

#if (BT_SCO_TX_NS_SEL != NS_NONE)
#undef BT_SCO_SMIC_EN
#define BT_SCO_SMIC_EN                  1
#undef BT_AEC_FF_MIC_REF_EN
#define BT_AEC_FF_MIC_REF_EN            0
#endif

#if (BT_SNDP_DMIC_EN || BT_SCO_DMIC_AI_EN || BT_SCO_NR_USER_DMIC_EN || BT_SNDP_FBDM_EN || BT_SNDP_DM_EN)
#undef BT_SCO_DMIC_EN
#define BT_SCO_DMIC_EN                  1
#endif

#if !BT_SCO_DMIC_EN && !BT_SCO_SMIC_EN
#undef ENC_DBG_EN
#define ENC_DBG_EN                      0
#endif

#if BT_SCO_SMIC_EN && BT_SCO_DMIC_EN
#error "NR cfg err: please choose only one of the NR algorithms at the same time!\n"
#endif

//#if (BT_SCO_AINS3_EN + BT_SCO_AINS4_EN + BT_SCO_SMIC_AI_EN) > 1
//#error "NR warning: please select only one nr algorithm in BT_SCO_AINS3_EN, BT_SCO_AINS4_EN, BT_SCO_SMIC_AI_EN\n"
//#endif

#if (BT_SCO_DUMP_EN || BT_AEC_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_EQ_DUMP_EN) && QTEST_EN
#warning "DUMP warning: please select only one nr algorithm in BT_XXX_DUMP_EN, VUSB_XXX_EN\n"
#endif

#if (BT_SCO_DUMP_EN + BT_AEC_DUMP_EN + BT_SCO_FAR_DUMP_EN + BT_EQ_DUMP_EN) > 1
#error "DUMP warning: please select only one dump in BT_AEC_DUMP_EN, BT_SCO_DUMP_EN, BT_SCO_FAR_DUMP_EN, BT_EQ_DUMP_EN\n"
#endif

/*****************************************************************************
 * Module    : 无线mic功能选择配置
 *****************************************************************************/
#define WIRELESS_EN                             (FUNC_ADAPTER_EN || FUNC_DEVICE_EN)

#if WIRELESS_SPK_TX_EN
    #define WIRELESS_CON_FULL_DUPLEX_EN     1       //是否支持全双工
#else
    #define WIRELESS_CON_FULL_DUPLEX_EN     0       //是否支持全双工
#endif // WIRELESS_SPK_TX_EN

#if !WIRELESS_EN
    #undef WIRELESS_MIC_EN
    #undef WIRELESS_SPK_EN
    #define WIRELESS_MIC_EN                     0
    #define WIRELESS_SPK_EN                     0
#else
  #if !WIRELESS_MIC_EN && !WIRELESS_SPK_EN
    #error "please enable at least one of WIRELESS_MIC_EN and WIRELESS_SPK_EN."
  #endif
#endif

#if FUNC_ADAPTER_EN && FUNC_DEVICE_EN
    #define WIRELESS_MIC_ROLE                   2       //0=无线麦TX, 1=无线麦RX, 2=同时支持TX&RX（通过配置选择）
#elif FUNC_ADAPTER_EN
    #define WIRELESS_MIC_ROLE                   1       //0=无线麦TX, 1=无线麦RX, 2=同时支持TX&RX（通过配置选择）
#elif FUNC_DEVICE_EN
    #define WIRELESS_MIC_ROLE                   0       //0=无线麦TX, 1=无线麦RX, 2=同时支持TX&RX（通过配置选择）
#else
    #define WIRELESS_MIC_ROLE                   0xff    //0=无线麦TX, 1=无线麦RX, 2=同时支持TX&RX（通过配置选择）
#endif

#if (WIRELESS_MIC_ROLE == 1 || WIRELESS_MIC_ROLE == 2)
    #define ADAPTER_EN                          1
#endif
#if (WIRELESS_MIC_ROLE == 0 || WIRELESS_MIC_ROLE == 2)
    #define DEVICE_EN                           1
#endif

#if !FUNC_ADAPTER_EN
    #undef ADAPTER_DAC_OUTPUT_EN
    #undef ADAPTER_I2S_OUTPUT_EN
    #undef ADAPTER_USB_MIC_RX_EN
    #define ADAPTER_DAC_OUTPUT_EN               0
    #define ADAPTER_I2S_OUTPUT_EN               0
    #define ADAPTER_USB_MIC_RX_EN               0
#endif

#if WIRELESS_MIC_SAMPLE_RATE_SELECT == SAMPLE_RATE_16K
    #define FRAME_SIZE_MIN                      20      //1.25ms
#elif WIRELESS_MIC_SAMPLE_RATE_SELECT == SAMPLE_RATE_24K
    #define FRAME_SIZE_MIN                      30      //1.25ms
#elif WIRELESS_MIC_SAMPLE_RATE_SELECT == SAMPLE_RATE_32K
    #define FRAME_SIZE_MIN                      40      //1.25ms
#elif WIRELESS_MIC_SAMPLE_RATE_SELECT == SAMPLE_RATE_48K
    #define FRAME_SIZE_MIN                      60      //1.25ms
#else
    #error "WIRELESS_MIC_SAMPLE_RATE_SELECT is not allowed!"
#endif

#if WIRELESS_MIC_SAMPLES_SELECT < FRAME_SIZE_MIN
    #error "WIRELESS_MIC_SAMPLES_SELECT is not allowed!"
#endif
#if (WIRELESS_MIC_SAMPLES_SELECT % FRAME_SIZE_MIN) != 0
    #error "WIRELESS_MIC_SAMPLES_SELECT is not allowed!"
#endif

#define WIRELESS_MIC_DFU_TX_INTERVAL            (WIRELESS_MIC_SAMPLES_SELECT/FRAME_SIZE_MIN)
#ifndef WIRELESS_MIC_TX_INTERVAL
    #define WIRELESS_MIC_TX_INTERVAL            WIRELESS_MIC_DFU_TX_INTERVAL
#endif

#if WIRELESS_CON_VERS < 64
  #if WIRELESS_MIC_TX_INTERVAL == 12
    #define WIRELESS_CON_INTERVAL               60
  #elif WIRELESS_MIC_TX_INTERVAL == 8
    #define WIRELESS_CON_INTERVAL               64
  #else
    #if ADAPTER_SAVE_PARAM_EN
    #define WIRELESS_CON_INTERVAL               12
    #else
    #define WIRELESS_CON_INTERVAL               60
    #endif
  #endif
    #define WIRELESS_CON_COMB_BUF_EN            0       //单包
    #define WIRELESS_CON_CRC_EN                 1

  #if WIRELESS_MIC_TX_INTERVAL == 0
    #error "WIRELESS_MIC_TX_INTERVAL is not allowed!"
  #elif WIRELESS_MIC_TX_INTERVAL == 1                   //1.25ms
    #undef WIRELESS_MIC_TX_INTERVAL
    #define WIRELESS_MIC_TX_INTERVAL            1
    #define WIRELESS_MIC_COMB_NB                WIRELESS_MIC_TX_INTERVAL
  #else
    #define WIRELESS_MIC_COMB_NB                (WIRELESS_MIC_TX_INTERVAL/WIRELESS_MIC_DFU_TX_INTERVAL)
  #endif
  #if (WIRELESS_MIC_SAMPLES_SELECT*WIRELESS_MIC_COMB_NB)%(WIRELESS_MIC_TX_INTERVAL*FRAME_SIZE_MIN) != 0
    #error "WIRELESS_MIC_TX_INTERVAL is not allowed!"
  #endif
  #if WIRELESS_MIC_TX_INTERVAL == 0
    #error "WIRELESS_MIC_TX_INTERVAL is not allowed!"
  #endif
//  #if WIRELESS_MIC_SAMPLES_SELECT%(WIRELESS_MIC_TX_INTERVAL*FRAME_SIZE_MIN) != 0
//    #error "WIRELESS_MIC_TX_INTERVAL is not allowed!"
//  #endif
  #if WIRELESS_CON_INTERVAL%WIRELESS_MIC_TX_INTERVAL != 0
    #error "WIRELESS_CON_INTERVAL is not allowed!"
  #endif
#else
    #error "WIRELESS_CON_VERS is not allowed!"
#endif

#if WIRELESS_MIC_COMB_NB == 0
    #undef WIRELESS_MIC_COMB_NB
    #define WIRELESS_MIC_COMB_NB                1
#endif

#ifndef WIRELESS_MIC_BROADCAST_EN
    #define WIRELESS_MIC_BROADCAST_EN            (WIRELESS_CON_VERS == 8)
#endif

#ifndef ADAPTER_SAVE_PARAM_EN
    #define ADAPTER_SAVE_PARAM_EN                0
    #define ADAPTER_FIX_DEVICE_EN                0
#endif

/*****************************************************************************
 * Module    : uart0 printf 功能自动配置(自动关闭SD卡，USB)
 *****************************************************************************/
#if (UART0_PRINTF_SEL == PRINTF_PB2)
    #if (SD0_MAPPING == SD0MAP_G2)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PA7)
    #if (SD0_MAPPING == SD0MAP_G1)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    #undef  FUNC_USBDEV_EN
    #undef  MUSIC_UDISK_EN
    #define FUNC_USBDEV_EN          0
    #define MUSIC_UDISK_EN          0
#elif (UART0_PRINTF_SEL == PRINTF_PE13)
    #if (SD0_MAPPING == SD0MAP_G4)
//    #undef  MUSIC_SDCARD_EN
//    #define MUSIC_SDCARD_EN         0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_NONE)
    //关闭所以打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif

#if !MUSIC_SDCARD_EN
#undef SD_DETECT_INIT
#undef SD_IS_ONLINE
#undef SD_DETECT_IS_BUSY

#define SD_DETECT_INIT()
#define SD_IS_ONLINE()              0
#define SD_DETECT_IS_BUSY()            0
#endif


/*****************************************************************************
 * Module    : usb device 功能配置
 *****************************************************************************/
#if !FUNC_USBDEV_EN && !ADAPTER_USB_MIC_RX_EN && !ADAPTER_USB_SPK_EN
    #undef  UDE_STORAGE_EN
    #undef  UDE_SPEAKER_EN
    #undef  UDE_HID_EN
    #undef  UDE_MIC_EN
    #undef  UDE_ENUM_TYPE

    #define UDE_STORAGE_EN              0
    #define UDE_SPEAKER_EN              0
    #define UDE_HID_EN                  0
    #define UDE_MIC_EN                  0
    #define UDE_ENUM_TYPE               0
#else
    #define UDE_ENUM_TYPE               (UDE_STORAGE_EN*0x01 + UDE_SPEAKER_EN*0x02 + UDE_HID_EN*0x04 + UDE_MIC_EN*0x08)
#endif

/*****************************************************************************
 * Module    : iis 功能配置
 *****************************************************************************/
#if I2S_EN

#if I2S_MODE_SEL
#undef  I2S_DMA_EN
#define I2S_DMA_EN                      1       //slave模式必须打开DMA功能
#endif

#endif // I2S_EN

/*****************************************************************************
 * Module    : 提示音配置
 *****************************************************************************/
#if (!WARNING_TONE_EN)
#undef WARNING_WSBC_RES_EN
#undef WARNING_MP3_RES_EN
#undef WARNING_WAV_RES_EN
#undef WARNING_PIANO_RES_EN
#define WARNING_WSBC_RES_EN             0
#define WARNING_MP3_RES_EN              0
#define WARNING_WAV_RES_EN              0
#define WARNING_PIANO_RES_EN            0

#undef WARNING_POWER_ON
#undef WARNING_POWER_OFF
#undef WARNING_FUNC_MUSIC
#undef WARNING_FUNC_BT
#undef WARNING_FUNC_CLOCK
#undef WARNING_FUNC_FMRX
#undef WARNING_FUNC_AUX
#undef WARNING_FUNC_USBDEV
#undef WARNING_FUNC_SPEAKER
#undef WARNING_LOW_BATTERY
#undef WARNING_BT_WAIT_CONNECT
#undef WARNING_BT_PAIR
#undef WARNING_BT_CONNECT
#undef WARNING_BT_DISCONNECT
#undef WARNING_BT_LOW_LATENCY
#undef WARNING_BT_CALL_CTRL
#undef WARNING_BT_RING_NUMBER
#undef WARNING_USB_SD
#undef WARNING_BT_HID_MENU
#undef WARNING_BTHID_CONN
#undef WARNING_MAX_VOLUME

#define WARNING_POWER_ON               0
#define WARNING_POWER_OFF              0
#define WARNING_FUNC_MUSIC             0
#define WARNING_FUNC_BT                0
#define WARNING_FUNC_CLOCK             0
#define WARNING_FUNC_FMRX              0
#define WARNING_FUNC_AUX               0
#define WARNING_FUNC_USBDEV            0
#define WARNING_FUNC_SPEAKER           0
#define WARNING_LOW_BATTERY            0
#define WARNING_BT_WAIT_CONNECT        0
#define WARNING_BT_PAIR                0
#define WARNING_BT_CONNECT             0
#define WARNING_BT_DISCONNECT          0
#define WARNING_BT_LOW_LATENCY         0
#define WARNING_BT_CALL_CTRL           0
#define WARNING_BT_RING_NUMBER         0
#define WARNING_USB_SD                 0
#define WARNING_BT_HID_MENU            0
#define WARNING_BTHID_CONN             0
#define WARNING_MAX_VOLUME             0
#endif

#if !FUNC_AUX_EN
#undef  WARNING_FUNC_AUX
#define WARNING_FUNC_AUX               0
#endif

#if !FUNC_FMRX_EN
#undef  WARNING_FUNC_FMRX
#define WARNING_FUNC_FMRX              0
#endif

#if !FUNC_USBDEV_EN
#undef  WARNING_FUNC_USBDEV
#define WARNING_FUNC_USBDEV            0
#endif

#if !FUNC_SPEAKER_EN
#undef  WARNING_FUNC_SPEAKER
#define WARNING_FUNC_SPEAKER           0
#endif

#if !BT_HFP_RING_NUMBER_EN
    #undef WARNING_BT_RING_NUMBER
    #define WARNING_BT_RING_NUMBER         0
#endif

#if !BT_LOW_LATENCY_EN
    #undef WARNING_BT_LOW_LATENCY
    #define WARNING_BT_LOW_LATENCY         0
#endif

#if ((!MUSIC_UDISK_EN) && (!MUSIC_SDCARD_EN))
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               0
#endif

#if SD_USB_MUX_IO_EN == 1
	#undef FUNC_USBDEV_EN
	#define FUNC_USBDEV_EN 			   0
#endif

#ifndef UPD_FILENAME
#define UPD_FILENAME                   "fw5000.upd"
#endif

#if ((SD0_MAPPING == SD0MAP_G1) || (SD0_MAPPING == SD0MAP_G4) || (SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G6))
#define SDCLK_AD_CH                    ADCCH_PA6
#elif (SD0_MAPPING == SD0MAP_G2)
#define SDCLK_AD_CH                    ADCCH_PB1
#else
#define SDCLK_AD_CH                    ADCCH_PE6
#endif
#define USB_SUPPORT_EN                 (MUSIC_UDISK_EN | FUNC_USBDEV_EN | ADAPTER_USB_MIC_RX_EN | ADAPTER_USB_SPK_EN)
#define SD_SUPPORT_EN                  (MUSIC_SDCARD_EN)

#if MUSIC_SDCARD_EN
#define SDCMD_MUX_DETECT_EN            1
#define SDCLK_MUX_DETECT_EN            1
#else
#undef  ADKEY_MUX_SDCLK_EN
#undef  I2C_MUX_SD_EN

#define I2C_MUX_SD_EN                  0
#define SDCMD_MUX_DETECT_EN            0
#define SDCLK_MUX_DETECT_EN            0
#define ADKEY_MUX_SDCLK_EN           0
#endif // MUSIC_SDCARD_EN

#if !SD_SUPPORT_EN
#undef  UDE_STORAGE_EN
#define UDE_STORAGE_EN                 0
#undef  SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN              0
#endif

#if ADAPTER_USB_MIC_STEREO_EN
#undef USB_MIC_DCH_EN
#define USB_MIC_DCH_EN                  1                           //是否开启USB MIC 2 Channel
#endif
/*****************************************************************************
 * Module    : 无线配对码功能配置
 *****************************************************************************/
#if WIRELESS_CON_PAIR_MODE

#undef ADAPTER_SET_TX1_CON_ID              //此处设置的为RX端连接两个TX的配对码，需要在发射端分别设置对应的WIRELESS_CON_CON_ID_SEL配合使用
#undef ADAPTER_SET_TX2_CON_ID
#define ADAPTER_SET_TX1_CON_ID      0x05
#define ADAPTER_SET_TX2_CON_ID      0x00

#else

#undef ADAPTER_SET_TX1_CON_ID
#undef ADAPTER_SET_TX2_CON_ID
#define ADAPTER_SET_TX1_CON_ID      0x00
#define ADAPTER_SET_TX2_CON_ID      0x00


#endif

/*****************************************************************************
* Module    : 有冲突或功能上不能同时打开的宏
*****************************************************************************/
#if (SYS_KARAOK_EN && BT_BACKSTAGE_EN)
#error "SYS_KARAOK_EN and BT_BACKSTAGE_EN (include BT_APP_EN) can't open at sametime"
#endif

#define DAC_EQ_NUM_TOTAL               (DAC_PT_NUM + EQ_APP_NUM + BT_MUSIC_EFFECT_DBB_BAND_CNT)

#if (DAC_EQ_NUM_TOTAL > 20)
#error "error: (DAC_PT_NUM + EQ_APP_NUM + BT_MUSIC_EFFECT_DBB_BAND_CNT) > 20"
#endif
/*****************************************************************************
 * Module    : 算法相关配置
 *****************************************************************************/
#if DNR_FRE_DUMP_EN && (!DNR_FRE_EN)
    #error "DNR_FRE_DUMP_EN is not allowed!"
#endif

#if AINS4_DUMP_EN && (!AINS4_EN)
    #error "AINS4_DUMP_EN is not allowed!"
#endif

#define TOOLKIT_AUDIO_DUMP_EN                   (DNR_FRE_DUMP_EN || AINS4_DUMP_EN)

/*****************************************************************************
* Module    : 计算FunctionKey
*****************************************************************************/
#if BT_TWS_EN
    #define FUNCKEY_TWS                 0xe0938553
#else
    #define FUNCKEY_TWS                 0
#endif
#endif // __CONFIG_EXTRA_H__

