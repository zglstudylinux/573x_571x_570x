/*****************************************************************************
 * Module    : Config
 * File      : config_ab5732E_le_mic.h
 * Function  : 搭配AB5732E的无线麦配置文件
 *****************************************************************************/

#ifndef CONFIG_LOW_LATENCY_H
#define CONFIG_LOW_LATENCY_H
#include "config_define.h"


/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_MUSIC_EN                   0   //是否打开MUSIC功能
#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_DEVICE_EN                  1   //是否打开发射器功能（只做接收时关闭，节省空间）
#define FUNC_ADAPTER_EN                 1   //是否打开接收器功能（只做发送时关闭，节省空间）
#define FUNC_BT_DUT_EN                  1   //是否打开蓝牙的独立DUT测试模式
#define FUNC_BT_FCC_EN                  0   //是否打开蓝牙的独立FCC测试模式（IODM已独立模式，不需要打开FCC模式）
#define FUNC_AUX_EN                     0   //是否打开AUX功能
#define FUNC_USBDEV_EN                  0   //是否打开USB DEVICE功能
#define FUNC_SPEAKER_EN                 0   //是否打开Speaker模式
#define FUNC_IDLE_EN                    0   //是否打开IDLE功能


/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_120M                 //选择系统时钟
#define POWKEY_10S_RESET                xcfg_cb.powkey_10s_reset
#define SOFT_POWER_ON_OFF               1                       //是否使用软开关机功能
#define PWRKEY_2_HW_PWRON               0                       //用PWRKEY模拟硬开关
#define USB_SD_UPDATE_EN                0                       //是否支持UDISK/SD的离线升级
#define GUI_SELECT                      GUI_NO                  //GUI Display Select
#define UART0_PRINTF_SEL                PRINTF_VUSB //选择UART打印信息输出IO，或关闭打印信息输出
#define PWRON_ENTER_BTMODE_EN           0                       //是否上电默认进蓝牙模式
#define SLEEP_DAC_OFF_EN                (is_sleep_dac_off_enable()) //sfunc_sleep是否关闭DAC， 复用MICL检测方案不能关DAC。
#define SYS_VDDIO_LP_EN                 1                       //休眠模式是否打开切换VDDIO功能（省电，可能会影响VDDIO供电的外设）

#define OFFLINE_LOG_EN                  0                       //是否打开离线日志（打开之前要评估flash空间是否足够）
#define OFFLINE_LOG_SIZE                0x30000                 //离线日志存储区域大小
#define OFFLINE_LOG_START               (FLASH_SIZE - CM_SIZE - OFFLINE_LOG_SIZE) //离线日志存储区域起始位置（注意避让FOTA、自定义KEEP等其他功能）

/*****************************************************************************
 * Module    : 电源选择配置
 *****************************************************************************/
#define BUCK_MODE_EN                    xcfg_cb.buck_mode_en    //是否BUCK MODE
#define VDDIO_LIMIT_SEL                 2                       //vddio电流限制，0=100mA, 1=200mA, 2=300mA, 3=400mA, 4~7=no_limit
#define VDDBT_LIMIT_SEL                 2                       //vddbt电流限制，0=109mA, 1=194mA, 2=279mA, 3=366mA, 4~7=no_limit

/*****************************************************************************
 * Module    : SPIFLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_512K                //LQFP48芯片内置1MB，其它封装芯片内置512KB(实际导出prd文件要小于492K)
#define FLASH_CODE_SIZE                 492K                    //程序使用空间大小，code_size <= flash_size - [params(20k) + res2 + res.bin]
#define FLASH_RESERVE_SIZE              0x5000                  //程序保留空间大小，用作系统参数区params(20k)
#define FLASH_RES2_SIZE                 0x0000                  //参数保留空间大小，用作在线调参区res2(打开ANC_EQ_RES2_EN需要保留4K)
#define FLASH_ERASE_4K                  1                       //是否支持4K擦除
#define FLASH_DUAL_READ                 1                       //是否支持2线模式
#define FLASH_QUAD_READ                 0                       //是否支持4线模式
#define FLASH_SPEED_UP_EN               1                       //SPI FLASH提速。


/*****************************************************************************
 * Module    : 蓝牙功能配置
 *****************************************************************************/
#define BT_RF_EXT_CTL_EN                0   //外部射频控制，需要在rf_ctrl.c添加PA/LNA控制
#define BT_BQB_RF_EN                    0   //BR/EDR DUT测试模式，为方便测试不自动回连（仅用于BQB RF测试）
#define BT_FCC_TEST_EN                  0   //蓝牙FCC测试使能，默认PB3 波特率1500000通信（仅用于FCC RF测试）
#define BT_BACKSTAGE_EN                 0   //蓝牙后台管理（全模式使用蓝牙，暂不支持BLE后台）
#define BT_BACKSTAGE_PLAY_DETECT_EN     0   //非蓝牙模式下检测到手机蓝牙播放音乐，则切换到蓝牙模式
#define BT_NAME_DEFAULT                 "BT-BOX"     //默认蓝牙名称（不超过31个字符）
#define BT_NAME_WITH_ADDR_EN            0   //蓝牙名称是否附加地址信息（调试用，例如：btbox-***）
#define BT_LINK_INFO_PAGE1_EN           0   //是否使用PAGE1回连信息（打开后可以最多保存8个回连信息）
#define BT_POWER_UP_RECONNECT_TIMES     3   //上电回连次数
#define BT_TIME_OUT_RECONNECT_TIMES     20  //掉线回连次数
#define BT_SIMPLE_PAIR_EN               1   //是否打开蓝牙简易配对功能（关闭时需要手机端输入PIN码）
#define BT_DISCOVER_CTRL_EN             0   //是否使用按键打开可被发现（需自行添加配对键处理才能被连接配对）
#define BT_PWRKEY_5S_DISCOVER_EN        0   //是否使用长按5S开机进入可被发现(耳机长按开机功能)
#define BT_DISCOVER_TIMEOUT             100 //按键打开可被发现后，多久后仍无连接自动关闭，0不自动关闭，单位100ms
#define BT_ANTI_LOST_EN                 0   //是否打开蓝牙防丢报警
#define BT_DUT_MODE_EN                  1   //正常连接模式，是否使能DUT测试
#define BT_LOCAL_ADDR                   0   //蓝牙是否使用本地地址，0使用配置工具地址
#define BT_LOW_LATENCY_EN               0   //是否打开蓝牙低延时切换功能

#define BT_2ACL_EN                      1   //是否支持连接两部手机
#define BT_A2DP_EN                      1   //是否打开蓝牙音乐服务
#define BT_HFP_EN                       0   //是否打开蓝牙通话服务
#define BT_HSP_EN                       0   //是否打开蓝牙HSP通话服务
#define BT_SPP_EN                       1   //是否打开蓝牙串口服务
#define BT_ID3_TAG_EN                   0   //是否打开蓝牙ID3功能
#define BT_HID_EN                       0   //是否打开蓝牙HID服务
#define BT_ATT_EN                       0   //是否打开GATT_OVER_BREDR
#define BT_HID_TYPE                     0   //选择HID服务类型: 0=自拍器(VOL+, 部分Android不能拍照), 1=自拍器(VOL+和ENTER, 影响IOS键盘使用), 2=游戏手柄
#define BT_HID_MANU_EN                  0   //蓝牙HID是否需要手动连接/断开
#define BT_HID_DISCON_DEFAULT_EN        0   //蓝牙HID服务默认不连接，需要手动进行连接。
#define BT_HID_VOL_CTRL_EN              0   //是否支持HID调手机音量功能（需同时打开BT_HID_EN和BT_A2DP_VOL_CTRL_EN）
#define BT_HFP_CALL_PRIVATE_SWITCH_EN   0   //是否打开按键切换私密接听与蓝牙接听功能
#define BT_HFP_CALL_PRIVATE_FORCE_EN    0   //是否强制使用私密接听（仅在手机接听，不通过蓝牙外放）
#define BT_HFP_RECORD_DEVICE_VOL_EN     0   //是否支持分别记录不同连接设备的通话音量
#define BT_HFP_RING_NUMBER_EN           0   //是否支持来电报号
#define BT_HFP_INBAND_RING_EN           0   //是否支持手机来电铃声（部分android不支持，默认用本地RING提示音）
#define BT_HFP_BAT_REPORT_EN            0   //是否支持电量显示
#define BT_HFP_MSBC_EN                  0   //是否打开宽带语音功能
#define BT_HFP_VOIP_RECOGNITION_EN      0   //是否支持网络通话识别功能
#define BT_A2DP_AAC_AUDIO_EN            0   //是否支持蓝牙AAC音频格式
#define BT_A2DP_LHDC_AUDIO_EN           0   //是否支持蓝牙LHDC音频格式（请与Savitech购买Keypro授权，否则播放一小段时间后无声）
#define BT_A2DP_LDAC_AUDIO_EN           0   //是否支持蓝牙LDAC音频格式（需要加密狗授权，否则播放一小段时间后无声）
#define BT_A2DP_VOL_CTRL_EN             1   //是否支持音量与手机同步，（默认使用AVRCP协议，打开BT_HID_VOL_CTRL_EN后使用HID协议）
#define BT_A2DP_RECORD_DEVICE_VOL_EN    1   //是否支持分别记录不同连接设备的音量，使用设备时恢复当前设备音量
#define BT_A2DP_VOL_REST_EN             0   //是否支持连接不支持同步音量手机时复位音量
#define BT_A2DP_PLC_EN                  0   //是否打开解码异常修包功能（目前只支持TWS AAC解码错误时修包）

#define BT_A2DP_AVRCP_PLAY_STATUS_EN    0   //是否支持手机播放状态同步，可加快播放暂停响应速度
#define BT_A2DP_RECON_EN                0   //是否支持A2DP控制键（播放/暂停、上下曲键）回连
#define BT_A2DP_SUPTO_RESTORE_PLAY_EN   0   //是否支持蓝牙超距回连恢复播放
#define BT_A2DP_EXCEPT_RESTORE_PLAY_EN  0   //是否支持异常复位后回连恢复播放
#define BT_AVDTP_DYN_LATENCY_EN         0   //是否支持根据信号环境动态调整延迟
#define BT_SCO_DBG_EN                   0   //是否打开无线调试通话参数功能
#define BT_PBAP_EN                      0   //是否打开电话簿功能
#define BT_MAP_EN                       0   //是否打开蓝牙短信服务(用于获取设备时间，支持IOS/Android)
#define BT_SNATCH_EN                    0   //是否支持抢连

#define BT_TWS_EN                       0   //是否支持TWS
#define BT_TWS_SCO_EN                   1   //是否支持TWS双路通话
#define BT_TWS_PAIR_MODE                0   //0=通过蓝牙名字配对，1=通过ID配对
#define BT_TWS_PAIR_ID                  0x38393530
#define BT_TWS_PAIR_BONDING_EN          1   //是否支持TWS组队绑定，调用bt_tws_delete_link_info()删除配对信息可解除绑定
#define BT_TWS_MS_SWITCH_EN             1   //是否支TWS主从切换
#define BLE_TWS_MS_SWITCH_EN            1   //是否支BLE主从切换
#define BT_TWS_DBG_EN                   0   //是否支持BT-Assistant工具分析信号质量，需要打开BT_SPP_EN
#define BT_TWS_PUBLIC_ADDR_EN           0   //是否支持TWS配对后生成新地址连接手机（0：使用主耳地址，1：使用新地址，注意0、1程序不兼容不能相互配对）


/*****************************************************************************
 * Module    : BLE功能配置
 *****************************************************************************/
//BLE功能配置
#define LE_BQB_RF_EN                    0   //BLE DUT测试模式，使用串口通信（仅用于BQB RFPHY测试）
#define LE_PAIR_EN                      0   //是否使能BLE的加密配对
#define LE_SM_SC_EN                     0   //是否使能BLE的加密连接，需同时打开 LE_PAIR_EN
#define LE_ADV_POWERON_EN               1   //上电是否默认打开BLE广播

//gatt 配置
#define ATT_NUM                      100 //最大支持多少条gatt属性, att_handle 1 ~ ATT_NUM

#define LE_ADV0_EN                      0   //是否打开无连接广播功能
#define LE_WIN10_POPUP                  0   //是否打开win10 swift pair快速配对

//google快速配对，需要用到BLE和SPP，注意setting里BLE和SPP开关也需要打开
//支持与AB_Mate_APP同时打开，暂不支持和其他APP一起打开
#define GFPS_EN                         0   //是否打开谷歌快速配对功能

//APP功能相关（APP只能选1个）
#define APP_DEMO_EN                     0   //是否打开demo APP功能，包含ble广播，ble、spp、gatt over br/edr收发示例代码
#define AB_MATE_APP_EN                  0   //是否打开AB-Mate APP控制功能，AB-Mate FOTA通过AB_MATE_OTA_EN打开，TWS只支持连上后一起升级，使用.fot文件格式

#define FOT_EN                          0   //FOTA总开关
//独立FOTA功能配置
//独立FOTA主要用于第三方APP支持.fot文件格式，方便客户APP支持FOTA功能
//独立FOTA需要ble广播才能连接ble进行ota，SDK中配合打开APP_DEMO_EN或者其他有ble广播的APP
//独立FOTA，可通过wiki上的ab-ota-demo APP进行演示（可申请源码）
//AB-Mate APP自身支持FOTA，所以打开AB_MATE_APP_EN后会自动关闭独立FOTA
#define LE_AB_FOT_EN                    0   //是否打开BLE协议的独立FOTA服务，一般IOS使用（需要配合BLE APP使用）
#define BT_AB_FOT_EN                    0   //是否支持SPP协议的独立FOTA服务，一般Android使用（需要配合SPP APP使用）

//FOTA注意事项：
//1)FOTA程序大小需要限制在 (FLASH_SIZE/2) - 24K 以内，若1M flash不够，需要修改上面的FLASH_SIZE的宏改成2M
//2)Downloader的setting里也有设置BLE和SPP，需要留意打开
#define AB_FOT_TYPE                     AB_FOT_TYPE_PACK    //FOTA升级方式选择
#define RES_USERBIN_EN                  0                   //开启后会把提示音文件打包到userbin，OTA不升级这部分资源(更新这部分资源需要重新烧录)
#define FOT_SUPPORT_TWS                 0                   //是否支持TWS同步进行FOTA升级功能（打开后只能左右耳一起升级）
#define FOT_GUID_CHECK_EN               0                   //FOTA GUID校验使能（通过app.xm中的setid设置guid，guid校验不过禁止升级）
#define FOT_VER_CHECK_EN                0                   //FOTA 版本号校验使能（通过app.xm中的setver设置版本号，用于限制降级）
//flash 空间分配，根据实际情况调整，默认sdk需要把FLASH_SIZE设置到FSIZE_2M以上才够空间升级
#if AB_FOT_TYPE == AB_FOT_TYPE_PACK
#if FLASH_SIZE == FSIZE_1M
#if RES_USERBIN_EN
#define AB_FOT_PACK_START_ADDR          0xAF000             //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0x32000

#define USERBIN_START_ADDR              0xE1000             //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE)
#define USERBIN_SIZE                    0x19000             //100k
#else
#define AB_FOT_PACK_START_ADDR          0xAF000             //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0x4B000
#endif //RES_USERBIN_EN
#elif FLASH_SIZE == FSIZE_2M
#if RES_USERBIN_EN
#define AB_FOT_PACK_START_ADDR          0x100000            //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0xE1000

#define USERBIN_START_ADDR              0x1E1000            //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE)
#define USERBIN_SIZE                    0x19000             //100k
#else
#define AB_FOT_PACK_START_ADDR          0x12C000            //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0xC3000
#endif //RES_USERBIN_EN
#endif
#endif


/*****************************************************************************
 * Module    : 蓝牙通话算法配置
 *****************************************************************************/
//通话丢包修复
#define BT_PLC_EN                       1
#define BT_CALL_MAX_GAIN                xcfg_cb.bt_call_max_gain    //配置通话时DAC最大模拟增益
#define BT_PLC_DUMP_EN                  0                           //同时DUMP出修包前后数据, 及丢包状态。便于与CMODEL做对比

//通话回声消除算法
#define BT_ECHO_LEVEL                   0//xcfg_cb.bt_echo_level       //回声消除级别（级别越高，回声衰减越明显，但通话效果越差）(0~15)
#define BT_AEC_DUMP_EN                  0                           //是否打开AEC数据dump功能，dump:算法前 + 下行ref信号

#define BT_AEC_EN                       0                           //是否打开硬件AEC算法
#define BT_AEC_NLP_BYPASS_EN            0                           //是否打开nlp bypass
#define BT_AEC_FF_MIC_REF_EN            0                           //如果aec的ff_mic回声比talk_mic回声大，可使能这功能，用于双mic降噪

//单边通话
#define BT_ALC_EN                       0                           //是否使能ALC
#define BT_ALC_FADE_IN_DELAY            26                          //近端淡入延时(n*7.5ms)
#define BT_ALC_FADE_IN_STEP             1                           //近端淡入速度(64ms)
#define BT_ALC_FADE_OUT_DELAY           2                           //远端淡出延时(n*7.5ms)
#define BT_ALC_FADE_OUT_STEP            16                          //远端淡出速度(4ms)
#define BT_ALC_VOICE_THR                0x300


//通话MIC端功能配置（上行）
#define ENC_DBG_EN                      0                           //是否打开通话产测指令（兼容单、双MIC）

#define BT_SCO_DUMP_EN                  0                           //是否打开上行降噪算法数据dump功能，dump:算法前 + 远端ref + 算法后，（单、双麦可用）
#define BT_EQ_DUMP_EN                   0                           //是否打开上行EQ的数据dump功能，dump:算法前 + 算法后 + EQ后，（单、双麦可用）

#define BT_SCO_EQ_DRC_EN                0                           //DRC参数调试在 bt_mic_8k.drc //(msbc)bt_mic_16k.drc

#define BT_SCO_AGC_EN                   0                           //是否打开AGC算法

//通话MIC端降噪算法（上行，只能选其一）
#define BT_SCO_NR_EN					0	                        //是否打开自研单麦降噪(ains3)
#define BT_SCO_NR_LEVEL				    2	                        //降噪量：0~30级（默认0级）

#define BT_SCO_SMIC_AI_EN               0                           //是否打开自研单麦AI降噪算法
#define BT_SCO_SMIC_AI_LEVEL		    xcfg_cb.bt_dnn_level        //降噪量：0~40级（建议范围，默认0级）

#define BT_SCO_DMIC_AI_EN               0                           //是否打开自研双麦AI降噪算法

#define BT_SCO_AIAEC_DNN_EN             0                           //是否打开自研AIAEC+单麦DNN降噪算法
#define BT_SCO_AIAEC_DNN_LEVEL          xcfg_cb.bt_aiaec_level      //降噪量：0-30级
#define BT_SCO_AIAEC_NLP_LEVEL          xcfg_cb.bt_aiaec_echo_level //回声消除级别

#define BT_SCO_DMIC_AIAEC_EN            0                           //是否打开自研双麦+AIAEC降噪算法
#define BT_SCO_DMIC_AIAEC_NLMS_EN       1                           //是否打开NLMS
#define BT_SCO_DMIC_AIAEC_NLP_REF       0                           //0代表主mic，1代表副mic

//自定义通话降噪、AEC算法
#define BT_SCO_NR_USER_SMIC_EN          0                           //是否打开自定义单麦降噪功能
#define BT_SCO_NR_USER_DMIC_EN          0                           //是否打开自定义双麦降噪功能
#define BT_SCO_AEC_USER_EN              0                           //是否打开自定义AEC功能，需要打开 BT_SCO_NR_USER_SMIC_EN 或 BT_SCO_NR_USER_DMIC_EN

//通话喇叭端算法（下行）
#define BT_SCO_FAR_DUMP_EN              0                           //是否打开通话下行数据dump功能，dump:算法前 + 算法后

#define BT_SCO_FAR_NR_EN                0                           //是否打开远端降噪算法
#define BT_SCO_FAR_NOISE_LEVEL          5                           //降噪等级（0~5，越大降噪效果越好，音质越差）
#define BT_SCO_FAR_THR                  1                           //范围: 0~20
#define BT_SCO_FAR_VALUE_NS             3                           //范围: 0~50

#define BT_SCO_DAC_DRC_EN               0                           //是否打开通话下行————DRC，曲线在 eq/call_dac.drc 文件里面

#define BT_SCO_DAC_DNR_EN               0                           //是否打开通话下行————动态降噪，注：配合下行DRC使用，不建议单独开启DNR
#define BT_SCO_DAC_DNR_THR              (5)                         //设置通话下行动态降噪能量阈值

#define BT_SCO_CALLING_NR_EN            0                           //是否打开去电后响铃前喇叭的降噪算法
#define BT_SCO_CALLING_VOICE_POW        100                         //设置降噪阈值
#define BT_SCO_CALLING_VOICE_CNT        5                           //设置降噪次数

/*****************************************************************************
 * Module    : 无线功能选择配置
 *****************************************************************************/
#define WIRELESS_MIC_EN                         1               //是否支持MIC音频流（上行音频）
#define WIRELESS_SPK_EN                         0               //是否支持SPK音频流（下行音频）
#define WIRELESS_DUMP_EN                        0               //打印无线PER、RSSI等信息（调试用）

//公共配置
#define WIRELESS_CON_CODEC_SEL                  WS_CODEC_LC3B   //编解码选择
#define WIRELESS_CON_FREQ_BAND                  0               //无线mic工作频段  0:2402M~2480M  1:2200M~2278M  2:2320M~2398M  3:2500M~2578M  4: 2482M~2560M
#define WIRELESS_CON_LINK_NB                    1               //支持连几路无线麦（目前只支持1/2路）
#define WIRELESS_CON_VERS                       2               //传输机制版本（2=V3一般用于k歌话筒, 6=V7一般用于领夹麦, 8=V9用于一拖多)
#define WIRELESS_CON_BONDING_EN                 0               //组队绑定
#define WIRELESS_CON_RSSI_THR                   70              //设置连接范围RSSI(-dbm)，该值越小，组队范围越小(产线建议设成70左右，以防距离太远也能配)
#define WIRELESS_CON_PWR_CTR                    0               //是否支持自动调节pwr，近距离降低，远距离增加
#define WIRELESS_CON_ADV_EXT_EN                 0               //是否使能广播数据扩展功能(用于连接阶段的私有消息传输)
#define WIRELESS_CON_CON_ID_SEL                 0x00            //无线mic连接id，需要发射端跟接收端id一致才可连接，可以调用ble_set_con_id来动态修改id，实现动态连接
#define WIRELESS_CON_PAIR_MODE                  0               //是否使能配对码功能(一拖二两个发射端连接id不同,需要配合WIRELESS_CON_CON_ID_SEL使用)

//无线MIC音频配置（上行音频，DEVICE --> ADAPTER）
#define WIRELESS_MIC_SAMPLE_RATE_SELECT         SAMPLE_RATE_48K //无线mic采样率选择
#if WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3S
    #define WIRELESS_MIC_SAMPLES_SELECT             120             //无线mic每帧采样样点数选择，48khz 120/240
    #define WIRELESS_MIC_CHANNEL_SELECT             1               //无线mic声道选择，1为单声道，2为双声道（目前只支持单声道）
    #define WIRELESS_MIC_24B_PCM_EN                 0               //无线mic 24bit PCM使能
    #define WIRELESS_MIC_FRAME_SIZE                 25              //每帧压缩后大小（FRAME_SIZE*LINK_NB*RETRY_NB暂时不能超过72byte）
    #define WIRELESS_MIC_RETRY_NB                   3               //重传次数（暂时支持1/2）
    #define WIRELESS_MIC_ENC_MAX_US                 650             //编码运算时长（单位us）
    #define WIRELESS_MIC_DEC_MAX_US                 700             //解码运算时长（单位us，开PLC时约700us）
    #define WIRELESS_MIC_DAC_CACHE_US               200             //解码后dac cache缓存大小
    #define WIRELESS_MIC_SINGLE_PLC_EN              1               //解码后独立的PLC使能
    #define WIRELESS_MIC_TX_INTERVAL                4               //传输周期=n*1.25ms（默认值是2，改为4时传输次数可增加到5次）
#elif WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3F
    #define WIRELESS_MIC_SAMPLES_SELECT             240             //无线mic每帧采样样点数选择，48khz 120/240
    #define WIRELESS_MIC_CHANNEL_SELECT             1               //无线mic声道选择，1为单声道，2为双声道（目前只支持单声道）
    #define WIRELESS_MIC_24B_PCM_EN                 1               //无线mic 24bit PCM使能
    #define WIRELESS_MIC_FRAME_SIZE                 60              //每帧压缩后大小（FRAME_SIZE*LINK_NB*RETRY_NB暂时不能超过72byte）
    #define WIRELESS_MIC_RETRY_NB                   3               //重传次数（暂时支持1/2）
    #define WIRELESS_MIC_ENC_MAX_US                 1500            //编码运算时长（单位us，内部plc约2700us，无plc约1400us）
    #define WIRELESS_MIC_DEC_MAX_US                 1750            //解码运算时长（单位us，内部plc约2000us，无plc约1000us）
    #define WIRELESS_MIC_DAC_CACHE_US               200             //解码后dac cache缓存大小
    #define WIRELESS_MIC_SINGLE_PLC_EN              1               //解码后独立的PLC使能
    #define WIRELESS_MIC_TX_INTERVAL                4               //传输周期=n*1.25ms（默认值是2，改为4时传输次数可增加到5次）
#elif WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3B
    #define WIRELESS_MIC_SAMPLES_SELECT             60             //无线mic每帧采样样点数选择，48khz 120/240
    #define WIRELESS_MIC_CHANNEL_SELECT             1               //无线mic声道选择，1为单声道，2为双声道（目前只支持单声道）
    #define WIRELESS_MIC_24B_PCM_EN                 0               //无线mic 24bit PCM使能
    #define WIRELESS_MIC_FRAME_SIZE                 15              //每帧压缩后大小（FRAME_SIZE*LINK_NB*RETRY_NB暂时不能超过72byte）
    #define WIRELESS_MIC_RETRY_NB                   3               //重传次数（暂时支持1/2）
    #define WIRELESS_MIC_ENC_MAX_US                 600            //编码运算时长（单位us，内部plc约2700us，无plc约1400us）
    #define WIRELESS_MIC_DEC_MAX_US                 700            //解码运算时长（单位us，内部plc约2000us，无plc约1000us）
    #define WIRELESS_MIC_DAC_CACHE_US               200             //解码后dac cache缓存大小
    #define WIRELESS_MIC_SINGLE_PLC_EN              1               //解码后独立的PLC使能
    #define WIRELESS_MIC_TX_INTERVAL                1               //传输周期=n*1.25ms（默认值是2，改为4时传输次数可增加到5次）
#endif

//设备端私有配置
#define DEVICE_CON_CLK                          SYS_160M         //发射端连上接收端时跑的主频，一般不低于80M
#define DEVICE_ALG_EN_CLK                       SYS_160M        //发射端连上接收端，且算法打开（一般是降噪）的主频，请勿修改
#define DEVICE_DAC_OUTPUT_EN                    1               //发射端是否使能DAC输出（调试用）
#define DEVICE_I2S_INPUT_EN                     0               //设备端I2S输入功能(仅支持主机模式, 从机需要软件调速和缓存, 会影响低延迟)
#define DEVICE_MIC_FADE_IN_EN                   1               //设备端MIC是否使用软件淡入功能(避免外界声音比较大时,连上无线麦瞬间,声音比较突兀)

#define WIRELESS_MIC_EQ_DRC_EN                  0
#define WIRELESS_MIC_EQ_DRC_DELAY               WIRELESS_MIC_EQ_DRC_EN*130//EQ 运算时间，pacc运算时间较快
#define WIRELESS_MIC_ECHO_EN                    0               //发射端是否开启 ECHO音效
#define WIRELESS_MIC_ECHO_DELAY                 (WIRELESS_MIC_ECHO_EN*(1+WIRELESS_MIC_24B_PCM_EN)*120)//ECHO运算时间
#define WIRELESS_MIC_MAGIC_EN                   0               //发射端是否开启 MAGIC音效
#define WIRELESS_MIC_MAGIC_DELAY                WIRELESS_MIC_MAGIC_EN*220//MAGIC运算时间
#define WIRELESS_MIC_AGC_EN                     0               //发射端AGC算法使能(720样点对齐)
#define WIRELESS_MIC_AGC_DELAY                  WIRELESS_MIC_AGC_EN*0//AGC 运算时间
#define WIRELESS_MIC_ROOM_REVERB_EN             0               //房间混响使能
#define WIRELESS_MIC_ROOM_REVERB_DELAY          WIRELESS_MIC_ROOM_REVERB_EN*355//房间混响运算时间
#define WIRELESS_MIC_DNN_L2_EN                  0               //发射端DNN_L2降噪算法使能
#define WIRELESS_MIC_DNN_L3_EN                  0               //发射端DNN_L3降噪算法使能
#define WIRELESS_MIC_SRC_EN                     0               //软件SRC重采样模块 32K -> 48k 80samples ->120samples
#define WIRELESS_MIC_SRC_DELAY                  WIRELESS_MIC_SRC_EN*200//软件件SRC运算时间
#define WIRELESS_MIC_DNN_L3_32K_EN              0               //发射端DNN_L3_32K降噪算法使能
#define WIRELESS_MIC_DNR_FRE_EN                 0
#define WIRELESS_MIC_DNR_FRE_DELAY              WIRELESS_MIC_DNR_FRE_EN * 720   //发射端DNR_FRE运算时间
#define WIRELESS_MIC_YLCRN_L1_32K_EN            0               //发射端YLCRN_L1_32K降噪算法使能 ，开此降噪需要打开WIRELESS_MIC_SRC_EN宏
#define WIRELESS_MIC_AINS4_EN                   0               //AINS4算法使能  (发射端处理，默认开启后主频会抬至160M)

//适配器端私有配置
#define ADAPTER_CON_CLK                         SYS_160M         //接收端连上发射端时跑的主频，一般不低于80M
#define ADAPTER_ALG_EN_CLK                      SYS_160M        //接收端连上发射端，且算法打开（一般是降噪）的主频，请勿修改
#define ADAPTER_DAC_OUTPUT_EN                   1               //适配器dac输出MIC声音（调试用）
#define ADAPTER_I2S_IN_OUT_EN                   1               //接收端是否打开I2S双向传输功能(如果仅用到I2S_OUT功能, 可在模块内关闭I2S_RX_EN)
#define WIRELESS_CON_TX_BONDING_NUM             0               //设备端组队绑定rx个数 0：1rx，1：2rx
#define WIRELESS_DUAL_MIC_EN                    0               //是否支持多输入一路mic0 adc,仅支持单通道音频，和MIC数据糅合后走编码发送流程
#define WIRELESS_MIC_SOFT_GAIN_EN               0               //发射端是否开启带保护的数字增益功能
#define ADAPTER_LOCAL_MIC_MIX_EN                0               //接收端本地麦使能，支持EQ/DRC 是否连接无线麦都可以工作
#define ADAPTER_LOCAL_MIC_EQ_DRC_EN             0               //接收端本地麦是否使能支持EQ/DRC(仅对本地MIC进行EQ/DRC处理,不能和ADAPTER_MIC_DRC_EN同时开)
#define ADAPTER_MIX_DRC_EN                      0               //混音DRC功能(用于一拖二混音,还有noisegate)
#define ADAPTER_AGC_EN                          0               //AGC算法使能
#define ADAPTER_ECHO_EN                         0               //混响使能(仅支持24bit)
#define ADAPTER_MAGIC_EN                        0               //魔音使能
#define ADAPTER_FREQ_SHIFT_EN                   0               //移频使能
#define ADAPTER_ROOM_REVERB_EN                  0               //房间混响使能
#define ADAPTER_HOWLING_DNN_EN                  0               //HOWLING_DNN(AI_MASK)使能 (仅支持16bit)
#define ADAPTER_DNN_L1_EN                       0               //DNN_L1(512FFT)使能 (仅支持16bit)
#define ADAPTER_NORMAL_LE_EN                    0               //适配器是否支持多开一条普通BLE连接，可用于支持FOTA或简单透传
#define ADAPTER_AB_FOT_EN                       0               //适配器是否支持FOTA升级，若编译后代码超过492k，需要把FLASH_SIZE改成2M才能支持FOTA
#define ADAPTER_AB_FOT_DEVICE_SUPPORT           0 				//适配器FOTA支持同步升级发射端

/*****************************************************************************
 * Module    : WL算法参数配置
 *****************************************************************************/
//混响算法
#define ECHO_EN                                 (WIRELESS_MIC_ECHO_EN || ADAPTER_ECHO_EN)
#define ECHO_LEVEL			                    70              //attenuation  range:0-90
#define ECHO_DRY_USER                           32767           //干度 range:0-32767
#define ECHO_WET_USER                           20000           //湿度 range:0-32767
#define ECHO_DELAY_MAX_LEVEL                    9               //echo延迟等级划分，按照固定的间隔划分成这么多等级，用于后续等级加减
#define ECHO_DELAY_DEFAULT_LEVEL                8               //echo第一次上电默认等级 range:1-ECHO_DELAY_MAX_LEVEL
#define ECHO_DELAY_BUF_SIZE                     6000            //echo缓存大小（单位：样点数），根据剩余空间配置大小
#define ECHO_ATTENUATION_MAX_LEVEL              16

//魔音算法
#define MAGIC_EN                                (WIRELESS_MIC_MAGIC_EN)
#define MAGIC_EFFECT_DEFAULT_LEVEL              1               //magic第一次上电默认效果等级，0:原声，1:男神，2:女神，3:娃娃音，4:魔兽

//房间混响算法
#define ROOM_REVERB_EN                          (WIRELESS_MIC_ROOM_REVERB_EN||ADAPTER_ROOM_REVERB_EN)
#define ROOM_REVERB_LEVEL			            99              //衰减率range:0-100
#define ROOM_REVERB_DRY			                32767           //干度 range:0-65535 数值越大人声音越突出
#define ROOM_REVERB_WET			                26000           //湿度 range:0-32767 数值越大人混响效果越突出

//软件增益(新)
#define SOFT_GAIN_MAX_LEVEL                     8              // 64/32/16/8级音量表
#define SOFT_GAIN_DEFAULT_LEVEL                 0               //第一次上电默认等级

//AI DNN小模型自研降噪算法
#define DNN_L2_EN                               (WIRELESS_MIC_DNN_L2_EN )               //DNN_L2降噪使能
#define DNN_L2_LEVEL                            2900

//AI DNN小模型自研降噪算法
#define DNN_L3_EN                               (WIRELESS_MIC_DNN_L3_EN)               //DNN_L3降噪使能
#define DNN_L3_LEVEL                            2900

//AI DNN小模型自研降噪算法
#define DNN_L3_32K_EN                            (WIRELESS_MIC_DNN_L3_32K_EN)          //DNN_L1_32K降噪使能
#define DNN_L3_32K_LEVEL                         2900

//DNR FRE轻量降噪
#define DNR_FRE_EN                              (WIRELESS_MIC_DNR_FRE_EN || ADAPTER_DNR_FRE_EN)

//AI YLCRN L1 32K大模型自研降噪算法
#define YLCRN_L1_32K_EN                         (WIRELESS_MIC_YLCRN_L1_32K_EN)          //YLCRN_L1_32K降噪使能
#define YLCRN_L1_32K_LEVEL                       100                                   //1~32768 越小值降噪越深

//AINS4降噪算法
#define AINS4_EN                                (WIRELESS_MIC_AINS4_EN)
#define AINS4_NOISE_NT                          500            //0~32768   最好每500一个step，值越大底噪越大，值越小底噪越小

/*****************************************************************************
* Module    : AUX功能配置
******************************************************************************/
#define AUX_CHANNEL_CFG                 0//(CH_AUXL_ADC0 | CH_AUXR_ADC1) //选择LINEIN通路
#define MIC_CHANNEL_CFG                 CH_MIC0                     //选择MIC的通路
#define AUX_2_SDADC_EN                  0                           //AUX是否进SDADC, 否则直通DAC。进SDADC可以调AUX EQ, 及AUX录音等功能
#define AUX_SNR_EN                      0                           //AUX模式动态降噪(AUX模拟直通也能用)
#define AUX_DETECT_EN                   0                           //是否打开AUX检测

///通过配置工具选择检测GPIO
#define AUX_DETECT_INIT()            aux_detect_init()
#define AUX_IS_ONLINE()              aux_is_online()
#define AUX_DETECT_IS_BUSY()         aux_detect_is_busy()

/*****************************************************************************
 * Module    : PWM配置
 *****************************************************************************/
#define TIMER5_PWM_EN                   0           //TIMER PWM 功能

/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                  0
#define UDE_SPEAKER_EN                  1
#define UDE_HID_EN                      1
#define UDE_MIC_EN                      1

#define UDE_PROD_NAME                   "USB Mirophone"
#define UDE_SUPPLIER                    "Generic"
#define UDE_SERIAL_NB                   "20250201905926"
#define UDE_PID                         0x0124
#define UDE_VID                         0x0C21

#define USB_DET_VER_SEL                 0                           //USB插入检测方式,0-旧方式,1-新方式
#define UAC_VER_SEL                     1                           //0-none, 1-UAC1.0, 2-UAC2.0/UAC3.0
#define CFG_DESC_CFG_TYPE               0                           //UAC2.0/3.0: 0-不兼容I15自定义音频设置,VID可修改; 1-兼容I15自定义音频,VID不可改; 2-不适用; 3-兼容I15自定义音频,24bits mic, VID不可改
#define CFG_USB_MAXPOWER                0x96                        //usb Max Power, unit is 2mA
#define CFG_GET_CONF_LEN                1                           //0-兼容海贝音乐软件上的usb独占模式音量调节,1-默认
#define USB_DRIVER_MAX_EN               0                           //USB驱动能力MAX使能
#define UDM_VOL_DEFAULT_SEL             0                           //MIC电脑端默认音量: 100%: 0, 90%: 1, 80%: 2, 70%: 3
#define UDA_BALANCE_VOL_EN              1                           //是否使能Window/MacBook USB左右声道均衡调节功能(切换使能PC需要卸载设备)
#define USB_IPHONE_PREMOTE_EN           0                           //使能后开启iPhone/iPad音乐PP键快进/快退功能(AppleMusic/网易云音乐支持此功能)

#define USB_SPK_SPL_BIT_EN              SPK_SPL_SEL                 //Speaker采样率使能(在win7下,不兼容44.1k,播放会出现杂音)
#define USB_SPK_24BITS_EN               1                           //USB 24Bits采样率使能
#define USB_SPK_SYNC_MODE_EN            1                           //是否开启USB Speaker下行Sync模式

#define USB_MIC_DCH_EN                  1                           //是否开启USB MIC 2 Channel
#define USB_MIC_SPL_BIT_EN              MIC_SPL_SEL                 //MIC采样率使能
#define USB_MIC_24BITS_EN               1                           //MIC 24Bits采样率使能

/*****************************************************************************
* Module    : SDDAC配置控制
******************************************************************************/
#define DAC_CH_SEL                      4                           //DAC_MONO ~ DAC_VCMBUF_DUAL
#define DAC_VCM_SEL                     0                           //DAC VCM电压，取值范围0~6(1.2v~1.8v, step=0.1v)（内部调试用）
#define DAC_MAXOUT_VCM_SEL              4                           //DAC大功率VCM电压，DAC_MAXOUT_EN=1时使用（内部调试用）
#define DAC_FAST_SETUP_EN               0                           //DAC快速上电，有噪声需要外部功放MUTE
#define DAC_MAX_GAIN                    xcfg_cb.dac_max_gain        //配置DAC最大模拟增益，默认设置为dac_vol_table[VOL_MAX]
#define DAC_24BITS_EN                   1                           //是否使能24bit模式
#define DAC_OUT_SPR                     xcfg_cb.dac_spr_sel         //dac out sample rate
#define DAC_VCM_CAPLESS_EN              xcfg_cb.dac_vcm_less_en     //DAC VCM省电容方案,使用内部VCM
#define DAC_MAXOUT_EN                   xcfg_cb.dac_maxout_en       //DAC大功率模式
#define DAC_PULL_DOWN_DELAY             80                          //控制DAC隔直电容的放电时间, 无电容时可设为0，减少开机时间。
#define DAC_DRC_EN                      1                           //是否使能DRC功能(After EQ0)
#define DAC_OFF_FOR_BT_CONN_EN          xcfg_cb.dac_off_for_conn

#define DAC_PT_EN                       0                           //是否打开DAC产测校准功能
#define DAC_PT_NUM                      4                           //DAC产测使用EQ条数(最多6条)

/*****************************************************************************
* Module    : EQ和ANC配置
******************************************************************************/
#define EQ_MODE_EN                      1           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define EQ_DBG_IN_UART                  1           //是否使能UART在线调节EQ
#define EQ_DBG_IN_UART_VUSB_EN          0           //打开vusb在线调EQ的功能，vusb使用dma方式，默认波特率1.5M,打开智能仓或快测功能时波特率为9600
#define EQ_DBG_IN_SPP                   1           //是否使能SPP在线调节EQ

#define EQ_APP_EN                       0           //是否打开APP独立调EQ功能
#define EQ_APP_NUM                      6           //APP独立调EQ使用EQ条数


/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
//按键通用配置
#define KEY_MAX_NB                      3           //按键数量（不包括IR）
#define DOUBLE_KEY_TIME                 (xcfg_cb.double_key_time)                       //按键双击响应时间（单位50ms）
#define PWRON_PRESS_TIME                (500*xcfg_cb.pwron_press_time)                  //长按PWRKEY多长时间开机
#define PWROFF_PRESS_TIME               xcfg_cb.pwroff_press_time                       //长按PWRKEY多长时间关机

//pwrkey
#define PWRKEY_EN                       1           //PWRKEY的使用，0为不使用
#define PWRKEY_IS_PRESS()			    pwrkey_is_pressed()

//touch key
#define TKEY_EN                         0           //TouchKEY的使用，0为不使用
#define TKEY_SOFT_PWR_EN                0           //是否使用TouchKey进行软开关机
#define TKEY_LOWPWR_WAKEUP_DIS          0           //是否电池低电时关掉触摸唤醒, 无保护板的电池需要打开。
#define TKEY_INEAR_EN                   0           //是否使用TouchKey的入耳检测功能
#define TKEY_TEMP_EN                    0           //是否使用TouchKey的温度检测功能
#define TKEY_DEBUG_EN                   0           //仅调试使用，用于确认TKEY的参数
#define TKEY_IS_PRESS()			        (tkey_is_pressed() && TKEY_SOFT_PWR_EN)

//adkey
#define ADKEY_EN                        0           //ADKEY的使用， 0为不使用
#define ADKEY_CH                        ADCCH_PE7
#define ADKEY_MUX_SDCLK_EN              0           //是否使用复用SDCLK的ADKEY, 共用USER_ADKEY的按键table
#define ADKEY_MUX_LED_EN                0           //是否使用ADKEY与LED复用, 共用USER_ADKEY的流程(ADKEY与BLED配置同一IO)
#define ADKEY_PU10K_EN                  1           //ADKEY是否使用内部10K上拉, 按键数量及阻值见port_key.c
//adkey2
#define ADKEY2_EN                       0           //ADKEY2的使用，0为不使用
#define ADKEY2_CH                       ADCCH_PE6

//io key
#define IOKEY_EN                        0           //IOKEY的使用， 0为不使用
#define IOKEY_GPIO_SEL_EN               0           //IOKEY操作方式选择，0=直接操作寄存器, 1=通过GPIO_SEL0/1选择
#define IOKEY_GPIO_SEL0                 IO_PE0
#define IOKEY_GPIO_SEL1                 IO_PE1
#define IOKEY_MID01_EN                  0

//knob
#define KNOB_KEY_EN                     0           //旋钮的使用，0为不使用
#define KNOB_KEY_LEVEL                  16          //旋钮的级数

/*****************************************************************************
 * Module    : LED指示灯配置
 *****************************************************************************/
#define LED_DISP_EN                     1           //是否使用LED指示灯(蓝灯)
#define LED_PWR_EN                      1           //充电及电源指示灯(红灯)
#define LED_LOWBAT_EN                   0                           //电池低电是否闪红灯
#define BLED_CHARGE_FULL                xcfg_cb.charge_full_bled    //充电满是否亮蓝灯
#define BT_RECONN_LED_EN                0//xcfg_cb.bt_reconn_led_en    //蓝牙回连状态是否不同的闪灯方式

#define LED_INIT()                      bled_func.port_init(&bled_gpio)
#define LED_SET_ON()                    bled_func.set_on(&bled_gpio)
#define LED_SET_OFF()                   bled_func.set_off(&bled_gpio)

#define LED_PWR_INIT()                  rled_func.port_init(&rled_gpio)
#define LED_PWR_SET_ON()                rled_func.set_on(&rled_gpio)
#define LED_PWR_SET_OFF()               rled_func.set_off(&rled_gpio)

/*****************************************************************************
 * Module    : MIC外部LDO供电
 *****************************************************************************/
#define MIC_LDO_INIT()
#define MIC_LDO_EN()
#define MIC_LDO_DIS()


/*****************************************************************************
 * Module    : 电量检测及低电
 *****************************************************************************/
#define VBAT_DETECT_EN                  1           //电池电量检测功能
#define VBAT_FILTER_USE_PEAK            0           //电池检测滤波方式: 0=取平均值，1=取峰值(适用于播音乐时电池波动较大的音箱方案).
#define LPWR_WARNING_VBAT               xcfg_cb.lpwr_warning_vbat   //低电提醒电压
#define LPWR_OFF_VBAT                   xcfg_cb.lpwr_off_vbat       //低电关机电压
#define LPWR_REDUCE_VOL_EN              0                           //低电是否降低音量
#define LPWR_WARING_TIMES               0xff                        //报低电次数

/*****************************************************************************
 * Module    : 充电功能选择
 *****************************************************************************/
#define CHARGE_EN                       1           //是否打开充电功能
#define CHARGE_TRICK_EN                 xcfg_cb.charge_trick_en     //是否打开涓流充电功能
#define CHARGE_DC_NOT_PWRON             xcfg_cb.charge_dc_not_pwron //DC插入，是否软开机。 1: DC IN时不能开机
#define CHARGE_VOLT_FOLLOW_EN           xcfg_cb.charge_voltage_follow // 打开电压跟随模式，适用于支持快充的充电仓
#define CHARGE_DC_IN()                  ((RTCCON >> 20) & 0x01)
#define CHARGE_INBOX()                  ((RTCCON >> 22) & 0x01)
#define CHARGE_LOW_POWER_EN             0           //是否打开充电低功耗模式,打开此宏时，不能 INTF_HUART 传输数据
#define CHARGE_WORKING_WHILE_CHARGING   0           //是否打开边充电边工作功能，开启此功能需要关闭CHARGE_DC_NOT_PWRON和充电仓功能

//充电截止电流
#define CHARGE_STOP_CURR                xcfg_cb.charge_stop_curr
//充电截止电压：0:4.2v 1:4.35v 2:4.4v 3:4.45v
#define CHARGE_STOP_VOLT                0
//恒流充电（电池电压大于2.9v）电流
#define CHARGE_CONSTANT_CURR            xcfg_cb.charge_constant_curr
//涓流截止电压：0:2.9v; 1:3v
#define CHARGE_TRICK_STOP_VOLT          1
//涓流充电（电池电压小于2.9v）电流
#define CHARGE_TRICKLE_CURR             xcfg_cb.charge_trickle_curr
//恒压差充电差值选择：0:187.5mV  1:250mV  2:312mV  3:375mV
#define CHARGE_VOLT_FOLLOW_DIFF         3
//漏电电流选择
#define CHARGE_LEAKAGE_CURR             xcfg_cb.ch_leakage_sel   //0~3
//充电截止控制(电压满足后截止电流未满足的情况下的充电计时)
#define CHARGE_STOP_TIME                xcfg_cb.charge_stop_time

/*****************************************************************************
 * Module    :  充电仓功能选择
 *****************************************************************************/
#define CHARGE_BOX_EN                   0               //是否打开充电仓功能
#define CHARGE_BOX_TYPE                 CBOX_NOR        //充电仓类型选择，0=普通充电仓，1=智能充电仓，2=没有电池的普通充电仓
#define CHARGE_BOX_INTF_SEL             INTF_UART1      //充电仓通信接口选择
#define CHARGE_BOX_DELAY_DISC           3500            //智能充电仓时，延迟断线进入充电（0=不延迟, n=延迟n毫秒）
//仓维持电压选择
#define CHARGE_BOX_KEEP_VOLT            xcfg_cb.ch_inbox_sel   //0=1.1V, 1=1.7V

/*****************************************************************************
* Module    : NTC预警关机功能配置
******************************************************************************/
#define USER_NTC                        0
#define ADCCH_NTC                       ADCCH_PB0

/*****************************************************************************
 * Module    : 外部PA/LNA配置
 *****************************************************************************/

#define RF_EN_GP                        E
#define RF_TX_GP                        B
#define RF_RX_GP                        B
#define RF_EN_BIT                       BIT(2)
#define RF_TX_BIT                       BIT(5)
#define RF_RX_BIT                       BIT(4)

#define RF_EN_GPIODE                    SET_MACRO(GPIO, SET_MACRO(RF_EN_GP, DE))
#define RF_EN_GPIOFEN                   SET_MACRO(GPIO, SET_MACRO(RF_EN_GP, FEN))
#define RF_EN_GPIODIR                   SET_MACRO(GPIO, SET_MACRO(RF_EN_GP, DIR))
#define RF_EN_GPIOSET                   SET_MACRO(GPIO, SET_MACRO(RF_EN_GP, SET))
#define RF_EN_GPIOCLR                   SET_MACRO(GPIO, SET_MACRO(RF_EN_GP, CLR))

#define RF_TX_GPIODE                    SET_MACRO(GPIO, SET_MACRO(RF_TX_GP, DE))
#define RF_TX_GPIOFEN                   SET_MACRO(GPIO, SET_MACRO(RF_TX_GP, FEN))
#define RF_TX_GPIODIR                   SET_MACRO(GPIO, SET_MACRO(RF_TX_GP, DIR))
#define RF_TX_GPIOSET                   SET_MACRO(GPIO, SET_MACRO(RF_TX_GP, SET))
#define RF_TX_GPIOCLR                   SET_MACRO(GPIO, SET_MACRO(RF_TX_GP, CLR))

#define RF_RX_GPIODE                    SET_MACRO(GPIO, SET_MACRO(RF_RX_GP, DE))
#define RF_RX_GPIOFEN                   SET_MACRO(GPIO, SET_MACRO(RF_RX_GP, FEN))
#define RF_RX_GPIODIR                   SET_MACRO(GPIO, SET_MACRO(RF_RX_GP, DIR))
#define RF_RX_GPIOSET                   SET_MACRO(GPIO, SET_MACRO(RF_RX_GP, SET))
#define RF_RX_GPIOCLR                   SET_MACRO(GPIO, SET_MACRO(RF_RX_GP, CLR))

#define RF_CTL_INIT()                   RF_RX_GPIOFEN &= ~RF_RX_BIT; RF_RX_GPIODE |= RF_RX_BIT; RF_RX_GPIODIR &= ~RF_RX_BIT; RF_RX_GPIOCLR = RF_RX_BIT;\
                                        RF_TX_GPIOFEN &= ~RF_TX_BIT; RF_TX_GPIODE |= RF_TX_BIT; RF_TX_GPIODIR &= ~RF_TX_BIT; RF_TX_GPIOCLR = RF_TX_BIT;\
                                        RF_EN_GPIOFEN &= ~RF_EN_BIT; RF_EN_GPIODE |= RF_EN_BIT; RF_EN_GPIODIR &= ~RF_EN_BIT; RF_EN_GPIOSET = RF_EN_BIT
#define RF_CTL_EXIT()                   RF_RX_GPIODE &= ~RF_RX_BIT; RF_RX_GPIODIR |= RF_RX_BIT;\
                                        RF_TX_GPIODE &= ~RF_TX_BIT; RF_TX_GPIODIR |= RF_TX_BIT;\
                                        RF_EN_GPIODE &= ~RF_EN_BIT; RF_EN_GPIODIR |= RF_EN_BIT;
#define RF_CTL_TX()                     RF_TX_GPIOSET = RF_TX_BIT; RF_RX_GPIOCLR = RF_RX_BIT
#define RF_CTL_RX()                     RF_RX_GPIOSET = RF_RX_BIT; RF_TX_GPIOCLR = RF_TX_BIT
#define RF_CTL_IDLE()                   RF_RX_GPIOCLR = RF_RX_BIT; RF_TX_GPIOCLR = RF_TX_BIT


/*****************************************************************************
 * Module    :  产测功能选择
 *****************************************************************************/
#define TBOX_TEST_EN                    0               //是否支持测试盒DUT产测指令，通过PB3/VUSB通讯
#define IODM_TEST_EN                    0               //是否打开小牛测控功能
#define QTEST_EN                        0               //是否打开测试盒快测功能，默认VUSB 通讯测试
#define QTEST_IS_ENABLE()               xcfg_cb.qtest_en    //setting中是否打开了快测功能
#define TEST_INTF_SEL                   INTF_UART1      //产测通信接口选择


/*****************************************************************************
 * Module    : 其他配置
 *****************************************************************************/
#define RGB_SERIAL_EN                   0           //串行RGB推灯功能
#define ENERGY_LED_EN                   0           //能量灯软件PWM显示,声音越大,点亮的灯越多.
#define TSEN_DETECT_EN                  0			//温度检测


/*****************************************************************************
 * Module    : Sensor配置
 *****************************************************************************/
#define SC7A20_EN						0			//是否使能敲击芯片功能（需要配置I2C）

/*****************************************************************************
 * Module    : SD0配置
 *****************************************************************************/
#define SD0_MAPPING                     SD0MAP_G1   //选择SD0 mapping

///通过配置工具选择检测GPIO
#define SD_DETECT_INIT()                sdcard_detect_init()
#define SD_IS_ONLINE()                  sdcard_is_online()
#define SD_DETECT_IS_BUSY()             sdcard_detect_is_busy()

/*****************************************************************************
 * Module    : I2C配置
 *****************************************************************************/
#define I2C_HW_EN                       0           //是否使能硬件I2C功能
#define I2C_MAPPING                     I2CMAP_PE7E6 //选择I2C mapping

#define I2C_SW_EN                       0           //是否使能软件I2C功能
#define I2C_MUX_SD_EN                   0           //是否I2C复用SD卡的IO

#if I2C_MUX_SD_EN
#define I2C_SCL_IN()                    SD_CMD_DIR_IN()
#define I2C_SCL_OUT()                   SD_CMD_DIR_OUT()
#define I2C_SCL_H()                     SD_CMD_OUT_H()
#define I2C_SCL_L()                     SD_CMD_OUT_L()
#define I2C_SDA_IN()                    SD_DAT_DIR_IN()
#define I2C_SDA_OUT()                   SD_DAT_DIR_OUT()
#define I2C_SDA_H()                     SD_DAT_OUT_H()
#define I2C_SDA_L()                     SD_DAT_OUT_L()
#define I2C_SDA_IS_H()                  SD_DAT_STA()
#else
#define I2C_SCL_IN()                    GPIOADIR |= BIT(13);
#define I2C_SCL_OUT()                   {GPIOADIR &= ~BIT(13); GPIOADE |= BIT(13);}
#define I2C_SCL_H()                     GPIOASET = BIT(13);
#define I2C_SCL_L()                     GPIOACLR = BIT(13);
#define I2C_SDA_IN()                    {GPIOADIR |= BIT(14); GPIOAPU |= BIT(14);}
#define I2C_SDA_OUT()                   {GPIOADIR &= ~BIT(14); GPIOADE |= BIT(14);}
#define I2C_SDA_H()                     GPIOASET = BIT(14);
#define I2C_SDA_L()                     GPIOACLR = BIT(14);
#define I2C_SDA_IS_H()                  (GPIOA & BIT(14))
#endif // I2C_MUX_SD_EN

#define I2C_SDA_SCL_OUT()               {I2C_SDA_OUT(); I2C_SCL_OUT();}
#define I2C_SDA_SCL_H()                 {I2C_SDA_H(); I2C_SCL_H();}

/*****************************************************************************
 * Module    : I2S配置
 *****************************************************************************/
#define I2S_EN                          (I2S_AUDIO_IN_EN ||  I2S_AUDIO_OUT_EN || I2S_AUDIO_IN_AND_OUT_EN)          //是否使能I2S功能
#define I2S_DEVICE                      I2S_DEV_NO          //I2S设备选择
#define I2S_MAPPING_SEL                 I2S_IO_G1           //I2S IO口选择
#define I2S_BIT_MODE                    I2S_16BIT           //I2S数据位宽选择 16bit;32bit
#define I2S_DATA_MODE                   I2S_NORMAL          //I2S数据格式选择 left-justified mode; normal mode(left-justified mode 待调试)
#define I2S_DMA_EN                      1                   //I2S数据源选择 0:src; 1:dma(src 模式待调试)
#define I2S_MCLK_EN                     0                   //I2S_MASTER是否打开MCLK
#define I2S_MCLK_SEL                    2                   //I2S MCLK选择 0:64fs 1:128fs 2:256fs
#define I2S_PCM_MODE                    0                   //I2S是否打开PCM mode(PCM mdoe 待调试)
#define I2S_DAC_OUT_SET                 SPR_48000           //做从机时，配置dac采样率，做主机时可忽略(暂时支持48K采样率)
#define I2S_MODE_SEL                    I2S_MASTER          //I2S主从模式选择 0: master; 1:slave

#define I2S_AUDIO_IN_EN                 DEVICE_I2S_INPUT_EN   //是否使能IIS输入音频
#define I2S_AUDIO_OUT_EN                0                     //是否使能适配器端IIS输出音频
#define I2S_AUDIO_IN_AND_OUT_EN         ADAPTER_I2S_IN_OUT_EN //是否使能适配器IIS双工通信

/*****************************************************************************
 * Module    : 调音工具配置
******************************************************************************/
#define EFFECT_DBG_ADJUST_EN            1          //是否使能音效离线调试
#define EFFECT_DBG_ADJUST_IN_UART       1          //是否使能UART在线调试音效,该功能需要打开EFFECT_DBG_ADJUST_EN和EQ_DBG_IN_UART

/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 1           //是否打开提示音功能, 总开关
#define WARNING_WSBC_RES_EN             0           //是否支持wsbc提示音
#define WARNING_MP3_RES_EN              1           //是否支持mp3提示音
#define WARNING_WAV_RES_EN              1           //是否支持wav提示音
#define WARNING_PIANO_RES_EN            1           //是否支持piano提示音

#define WARNING_BREAK_EN                1            //是否支持提示音打断功能（主要是打断开机提示音和TWS副耳断开提示音）
#define WARING_MAXVOL_TYPE              RES_TYPE_TONE            //最大音量提示音类型
#define LANG_SELECT                     LANG_EN_ZH   //提示音语言选择
#define WARNING_SYSVOL_ADJ_EN           1            //播放提示音过程中，系统音量是否支持退避

#define WARNING_POWER_ON                1
#define WARNING_POWER_OFF               1
#define WARNING_FUNC_MUSIC              0
#define WARNING_FUNC_BT                 1
#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                0
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             1
#define WARNING_BT_WAIT_CONNECT         0
#define WARNING_BT_PAIR                 0            //BT PAIRING提示音
#define WARNING_BT_CONNECT              1
#define WARNING_BT_DISCONNECT           1
#define WARNING_LEFT_RIGHT_CH           0           //1=左右先后播报left/right channel，2=左右同时播报left/right channel
#define WARNING_BT_LOW_LATENCY          0
#define WARNING_BT_CALL_CTRL            0           //接听、挂断、回拨提示音
#define WARNING_BT_RING_NUMBER          0           //来电报号
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              1
#define WARNING_MIN_VOLUME              0
#define WARNING_BT_HID_MENU             0            //BT HID MENU手动连接/断开HID Profile提示音
#define WARNING_BTHID_CONN              0            //BTHID模式是否有独立的连接/断开提示音
#define WARNING_TAKE_PHOTO              0


#define SW_VERSION		1		        //只能使用数字(u16)，例如版本号V1.0.2，ab_mate用102表示，ab_fot用0x102表示
#define HW_VERSION		"V0.0.1"		//只能使用数字0-9,ota需要转码
#include "config_extra.h"

#endif // CONFIG_LOW_LATENCY_H
