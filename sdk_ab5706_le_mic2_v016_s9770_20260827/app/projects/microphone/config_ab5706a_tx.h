/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef CONFIG_AB5706A_TX_H
#define CONFIG_AB5706A_TX_H
#include "config_define.h"


/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_DEVICE_EN                  1   //是否打开发射器功能（只做接收时关闭，节省空间）
#define FUNC_ADAPTER_EN                 0   //是否打开接收器功能（只做发送时关闭，节省空间）
#define FUNC_BT_EN                      0   //是否打开蓝牙功能
#define FUNC_LE_DUT_EN                  0   //是否打开DUT测试模式(TX-PB3,RX-PB4,波特率:9600)


/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define FPGA_EN                         0                       //FPGA调试(关闭部分与硬件相关的模拟配置)
#define SYS_CLK_SEL                     SYS_24M                 //选择系统时钟
#define POWKEY_10S_RESET                xcfg_cb.powkey_10s_reset
#define SOFT_POWER_ON_OFF               1                       //是否使用软开关机功能
#define PWRKEY_2_HW_PWRON               0                       //用PWRKEY模拟硬开关
#define USB_SD_UPDATE_EN                0                       //是否支持UDISK/SD的离线升级
#define GUI_SELECT                      GUI_NO                  //GUI Display Select
#define UART0_PRINTF_SEL                PRINTF_VUSB             //选择UART打印信息输出IO，或关闭打印信息输出
#define PWRON_ENTER_BTMODE_EN           0                       //是否上电默认进蓝牙模式
#define SYS_VDDIO_LP_EN                 1                       //休眠模式是否打开切换VDDIO功能（省电，可能会影响VDDIO供电的外设）

/*****************************************************************************
 * Module    : SPIFLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_256K              //LQFP48芯片内置1MB，5706B封装256KBYTE,其它封装芯片内置512KB(实际导出prd文件要小于492K)
#define FLASH_CODE_SIZE                 236K                    //程序使用空间大小，code_size <= flash_size - [params(20k) + res2 + res.bin]
#define FLASH_RESERVE_SIZE              0x5000                  //程序保留空间大小，用作系统参数区params(20k)
#define FLASH_RES2_SIZE                 0x0000                  //参数保留空间大小，用作在线调参区res2(不支持)
#define FLASH_ERASE_4K                  1                       //是否支持4K擦除
#define FLASH_DUAL_READ                 1                       //是否支持2线模式
#define FLASH_QUAD_READ                 0                       //是否支持4线模式
#define FLASH_SPEED_UP_EN               1                       //SPI FLASH提速。

/*****************************************************************************
 * Module    : 无线功能选择配置
 *****************************************************************************/
#define WIRELESS_MIC_EN                         1               //设备端是否支持MIC发射（上行音频）
#define WIRELESS_SPK_EN                         0               //适配器是否支持SPK发射（下行音频，暂不支持）

#define WIRELESS_DUMP_EN                        0               //打印无线PER、RSSI等信息（调试用）

//公共配置
#define WIRELESS_CON_CODEC_SEL                  WS_CODEC_LC3S   //编解码选择
#define WIRELESS_CON_FREQ_BAND                  0               //无线mic工作频段  0:2402M~2480M  1:2200M~2278M  2:2320M~2398M  3:2500M~2578M  4: 2482M~2560M
#define WIRELESS_CON_LINK_NB                    2               //支持连几路无线麦（目前只支持1/2路）
#define WIRELESS_CON_VERS                       2               //传输机制版本（2=V3一般用于k歌话筒, 6=V7一般用于领夹麦, 8=V9用于一拖多)
#define WIRELESS_CON_BONDING_EN                 0               //组队绑定
#define WIRELESS_CON_RSSI_THR                   70              //设置连接范围RSSI(-dbm)，该值越小，组队范围越小(产线建议设成70左右，以防距离太远也能配)
#define WIRELESS_CON_PWR_CTR                    0               //是否支持自动调节pwr，近距离降低，远距离增加
#define WIRELESS_CON_CON_ID_SEL                 0x00            //无线mic连接id，需要发射端跟接收端id一致才可连接，可以调用ble_set_con_id来动态修改id，实现动态连接
#define WIRELESS_CON_PAIR_MODE                  0               //是否使能配对码功能(一拖二两个发射端连接id不同,需要配合WIRELESS_CON_CON_ID_SEL使用)

//无线MIC配置（上行音频，DEVICE --> ADAPTER）
#define WIRELESS_MIC_SAMPLE_RATE_SELECT         SAMPLE_RATE_48K //无线mic采样率选择
#define WIRELESS_MIC_SAMPLES_SELECT             120             //无线mic每帧采样样点数选择  16k：20, ADPCM压缩后是samples*2/4+3
#define WIRELESS_MIC_CHANNEL_SELECT             1               //无线mic声道选择，1为单声道，2为双声道（目前只支持单声道）
#define WIRELESS_MIC_FRAME_SIZE                 25              //每帧压缩后大小（FRAME_SIZE*LINK_NB*RETRY_NB暂时不能超过72byte）
#define WIRELESS_MIC_RETRY_NB                   3               //重传次数（暂时支持1/2）
#define WIRELESS_MIC_ENC_MAX_US                 400             //编码运算时长（单位us）
#define WIRELESS_MIC_DEC_MAX_US                 550             //解码运算时长（单位us，开PLC时约700us）
#define WIRELESS_MIC_TX_INTERVAL                2               //传输周期=n*1.25ms（默认值是2，改为4时传输次数可增加到5次）
#define WIRELESS_CON_TX_BONDING_NUM             0               //设备端组队绑定rx个数 0：1rx，1：2rx

//设备端私有配置
#define WIRELESS_MIC_AINS3_EN                   0               //AINS3_48K算法使能  (发射端处理，默认开启后主频会抬至160M)
#define WIRELESS_MIC_DNN_L2_EN                  0               //发射端DNN_L1算法使能 (发射端处理，默认开启后主频会抬至160M)
#define WIRELESS_MIC_DNN_L3_EN                  0               //发射端DNN_L3算法使能 (发射端处理，默认开启后主频会抬至160M)
#define WIRELESS_MIC_ECHO_EN                    0               //发射端是否开启 ECHO音效
#define WIRELESS_MIC_ECHO_DELAY                 WIRELESS_MIC_ECHO_EN*120//ECHO运算时间
#define WIRELESS_MIC_MAGIC_EN                   0               //发射端是否开启 MAGIC音效
#define WIRELESS_MIC_MAGIC_DELAY                WIRELESS_MIC_MAGIC_EN*220//MAGIC运算时间
#define WIRELESS_MIC_DAC_OUTPUT_EN              0               //发射端是否使能DAC输出
#define WIRELESS_MIC_SRC_EN                     0               //硬件SRC重采样模块 32K -> 48k 80samples ->120samples
#define WIRELESS_MIC_SRC_DELAY                  WIRELESS_MIC_SRC_EN*361//硬件SRC运算时间
#define WIRELESS_MIC_AINS4_EN                   0               //AINS4算法使能  (发射端处理，默认开启后主频会抬至160M)
#define WIRELESS_MIC_AGC_EN                     0               //发射端AGC算法使能(720样点对齐)
#define WIRELESS_MIC_AGC_DELAY                  WIRELESS_MIC_AGC_EN*0//AGC 运算时间
#define WIRELESS_MIC_EQ_DRC_EN                  0
#define WIRELESS_MIC_EQ_DRC_DELAY               WIRELESS_MIC_EQ_DRC_EN*323//EQ 运算时间
#define WIRELESS_MIC_SOFT_VOL_EN                0               //是否使能软件调整麦克风音量
#define WIRELESS_MIC_SOFT_VOL_DELAY             WIRELESS_MIC_SOFT_VOL_EN * 150
#define WIRELESS_MIC_DNR_FRE_EN                 0
#define WIRELESS_MIC_DNR_FRE_DELAY              WIRELESS_MIC_DNR_FRE_EN * 500   //发射端DNR_FRE运算时间
#define WIRELESS_MIC_DNR_EN                     0               //发射端是否使能MIC能量检测


//适配器端私有配置
#define ADAPTER_DAC_OUTPUT_EN                   1
#define ADAPTER_MIX_DRC_EN                      0               //混音DRC功能(用于一拖二混音,还有noisegate)
#define ADAPTER_FREQ_SHIFT_EN                   0               //移频使能
#define ADAPTER_ECHO_EN                         0               //混响使能 (默认开启后主频会抬至160M)
#define ADAPTER_MAGIC_EN                        0               //魔音使能 (默认开启后主频会抬至160M)
#define ADAPTER_ROOM_REVERB_EN                  0               //房间混响使能
#define ADAPTER_HOWLING_DNN_EN                  0               //接收端HOWLING_DNN使能
#define ADAPTER_MIC_AINS4_EN                    0               //接收端AINS4算法使能
#define ADAPTER_MIC_DNN_L2_EN                   0              //接收端DNN_L2算法使能
#define ADAPTER_MIC_DNN_L3_EN                   0               //接收端DNN_L3算法使能
#define ADAPTER_USB_MIC_RX_EN                   0               //接收端是否打开usb mic
#define ADAPTER_USB_SPK_EN                      0               //接收端是否打开usb spk
#define ADAPTER_AGC_EN                          0               //接收端AGC算法使能
#define ADAPTER_I2S_OUTPUT_EN                   0               //接收端是否打开I2S输出
#define ADAPTER_I2S_IN_EN                       0               //接收端是否打开I2S输入功能
#define ADAPTER_I2S_IN_OUT_EN                   0               //接收端是否打开I2S双向传输功能
#define ADAPTER_HUART_OUTPUT_EN                 0               //适配器是否支持HUART输出MIC音频,使用DMA输出。可以同时DAC输出
#define ADAPTER_HUART_INPUT_EN                  0               //适配器是否支持HUART输入数字信号,和麦数据混音后输出
#define ADAPTER_HUART_COMMAND_EN                0               //适配器是否支持HUART音频命令(用于将HUART TX端的数据命令同步到HUART RX端)
#define ADAPTER_DNR_FRE_EN                      0               //适配器是否打开 DNR_FRE
#define ADAPTER_MIC_DNR_EN                      0               //适配器是否使能MIC能量检测

//适配器端本地MIC
#define ADAPTER_LOCAL_MIC_EN                    0               //本地麦使能
#define ADAPTER_LOCAL_MIC_EQ_DRC_EN             0               //本地麦是否使能支持EQ/DRC， 独立EQ/DRC

///通过配置工具选择检测GPIO
#define MIC_DETECT_EN                           0                       //是否使能MIC检测
#define MIC_DETECT_INIT()                       mic_detect_init()
#define MIC_IS_ONLINE()                         mic_is_online()
#define IS_DET_MIC_BUSY()                       is_detect_mic_busy()
#define MIC_TRIM_EN                             1               //是否使能mic trim 开启才能用内部电阻电容(不使用内部电阻电容时，关掉可省不少空间)

/*****************************************************************************
 * Module    : WS算法参数配置
 *****************************************************************************/

//软件增益
#define SOFT_GAIN_MAX_LEVEL                     8              //64/32/16/8级音量表
#define SOFT_GAIN_DEFAULT_LEVEL                 8              //第一次上电默认等级

 //AINS4降噪算法
#define AINS4_EN                                (ADAPTER_MIC_AINS4_EN || WIRELESS_MIC_AINS4_EN)
#define AINS4_DUMP_EN                           0               //打印算法数据（调试用）
#define AINS4_NOISE_NT                          500            //0~20000   最好每500一个step，值越大底噪越大，值越小底噪越小

//AI DNN小模型自研降噪算法
#define DNN_L2_EN                               (WIRELESS_MIC_DNN_L2_EN || ADAPTER_MIC_DNN_L2_EN)               //DNN降噪使能
#define DNN_L2_LEVEL                            2900

//AI DNN中模型自研降噪算法
#define DNN_L3_EN                               (WIRELESS_MIC_DNN_L3_EN || ADAPTER_MIC_DNN_L3_EN)               //DNN降噪使能
#define DNN_L3_LEVEL                            2900

//混响算法
#define ECHO_EN                                 (ADAPTER_ECHO_EN || WIRELESS_MIC_ECHO_EN)
#define ECHO_LEVEL			                    70              //attenuation  range:0-90
#define ECHO_DRY_USER                           32767           //干度 range:0-32767
#define ECHO_WET_USER                           20000           //湿度 range:0-32767
#define ECHO_DELAY_MAX_LEVEL                    9               //echo延迟等级划分，按照固定的间隔划分成这么多等级，用于后续等级加减
#define ECHO_DELAY_DEFAULT_LEVEL                8               //echo第一次上电默认等级 range:1-ECHO_DELAY_MAX_LEVEL
#define ECHO_DELAY_BUF_SIZE                     6000            //echo缓存大小（单位：样点数），根据剩余空间配置大小
#define ECHO_ATTENUATION_MAX_LEVEL              16
//房间混响算法
#define ROOM_REVERB_EN                          (ADAPTER_ROOM_REVERB_EN)
#define ROOM_REVERB_LEVEL			            99              //衰减率range:0-100
#define ROOM_REVERB_DRY			                32767           //干度 range:0-65535 数值越大人声音越突出
#define ROOM_REVERB_WET			                26000           //湿度 range:0-32767 数值越大人混响效果越突出

//魔音算法
#define MAGIC_EN                                (ADAPTER_MAGIC_EN || WIRELESS_MIC_MAGIC_EN) //宏只能二选一，同时打开会编译报错
#define MAGIC_EFFECT_DEFAULT_LEVEL              1               //magic第一次上电默认效果等级，0:原声，1:男神，2:女神，3:娃娃音，4:魔兽

//AGC算法
#define AGC_EN                                  (ADAPTER_AGC_EN)

//HOWLING算法
#define HOWLING_DNN_EN                          (ADAPTER_HOWLING_DNN_EN)

//DNR FRE轻量降噪
#define DNR_FRE_EN                              (WIRELESS_MIC_DNR_FRE_EN || ADAPTER_DNR_FRE_EN)
#define DNR_FRE_DUMP_EN                         0               //打印算法数据（调试用）

//能量检测算法
#define MIC_DNR_EN                              (ADAPTER_MIC_DNR_EN)

/*****************************************************************************
* Module    : MIC功能配置
******************************************************************************/
#define MIC_CHANNEL_CFG                 CH_MIC0                     //选择MIC的通路


/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                  0
#define UDE_SPEAKER_EN                  0
#define UDE_HID_EN                      1
#define UDE_MIC_EN                      1

#define UDE_PROD_NAME                   "USB Headphone"
#define UDE_SUPPLIER                    "Generic"
#define UDE_SERIAL_NB                   "20240726905926"
#define UDE_PID                         0x0124                      //产品ID
#define UDE_VID                         0x0C21                      //厂商ID，需要修改为自家的ID

#define USB_DET_VER_SEL                 0                           //USB插入检测方式,0-旧方式,1-新方式，2-旧方式无拔出检测
#define UAC_VER_SEL                     1                           //0-none, 1-UAC1.0, 2-UAC2.0/UAC3.0
#define CFG_DESC_CFG_TYPE               0                           //UAC2.0/3.0: 0-不兼容I15自定义音频设置,VID可修改; 1-兼容I15自定义音频,VID不可改; 2-不适用; 3-兼容I15自定义音频,24bits mic, VID不可改
#define CFG_USB_MAXPOWER                0x96                        //usb Max Power, unit is 2mA
#define CFG_GET_CONF_LEN                1                           //0-兼容海贝音乐软件上的usb独占模式音量调节,1-默认
#define USB_DRIVER_MAX_EN               0                           //USB驱动能力MAX使能
#define UDM_VOL_DEFAULT_SEL             0                           //MIC电脑端默认音量: 100%: 0, 90%: 1, 80%: 2, 70%: 3
#define UDA_BALANCE_VOL_EN              0                           //是否使能Window/MacBook USB左右声道均衡调节功能(切换使能PC需要卸载设备)
#define USB_IPHONE_PREMOTE_EN           0                           //使能后开启iPhone/iPad音乐PP键快进/快退功能(AppleMusic/网易云音乐支持此功能)

#define USB_SPK_SPL_EN                  SPK_SPL_SEL                 //Speaker采样率选择(在win7下,不兼容44.1k,播放会出现杂音)
#define USB_SPK_BITS_EN                 SPK_BITS_SEL                //USB采样位宽选择(16bits/24bits/32bit)
#define USB_SPK_SYNC_MODE_EN            0                           //是否开启USB Speaker下行Sync模式

#define USB_MIC_DCH_EN                  0                           //是否开启USB MIC 2 Channel
#define USB_MIC_SPL_EN                  MIC_SPL_SEL                 //MIC采样率选择
#define USB_MIC_BITS_EN                 MIC_BITS_SEL                //MIC采样位宽选择(16bits/24bits)

/*****************************************************************************
* Module    : SDDAC配置控制
******************************************************************************/
#define DAC_CH_SEL                      xcfg_cb.dac_sel             //DAC_MONO ~ DAC_VCMBUF_DUAL
#define DAC_FAST_SETUP_EN               0                           //DAC快速上电，有噪声需要外部功放MUTE
#define DAC_OUT_SPR                     xcfg_cb.dac_spr_sel         //dac out sample rate
#define DAC_VCM_CAPLESS_EN              xcfg_cb.dac_vcm_less_en     //DAC VCM省电容方案,使用内部VCM
#define DAC_DUMMY_DIFF_EN               0                           //DAC 伪差分输出，需要配置DAC双声道  L:inverse L=(L+R)/2 R=(L+R)/2
#define DAC_PULL_DOWN_DELAY             80                          //控制DAC隔直电容的放电时间, 无电容时可设为0，减少开机时间。
#define DAC_DNR_EN                      0                           //是否使能动态降噪
#define DAC_DRC_EN                      0                           //是否使能DRC功能(After EQ0)

#define DAC_PT_EN                       0                           //是否打开DAC产测校准功能
#define DAC_PT_NUM                      4                           //DAC产测使用EQ条数(最多6条)

/*****************************************************************************
* Module    : EQ配置
******************************************************************************/
#define EQ_MODE_EN                      1           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define EQ_DBG_IN_UART                  1           //是否使能UART在线调节EQ
#define EQ_DBG_IN_UART_VUSB_EN          0           //打开vusb在线调EQ的功能，vusb使用dma方式，默认波特率1.5M,打开智能仓或快测功能时波特率为9600
#define EQ_DBG_IN_SPP                   0           //是否使能SPP在线调节EQ

#define EQ_APP_EN                       0           //是否打开APP独立调EQ功能
#define EQ_APP_NUM                      6           //APP独立调EQ使用EQ条数

/*****************************************************************************
 * Module    : 调音工具配置
******************************************************************************/
#define EFFECT_DBG_ADJUST_EN            1          //是否使能音效离线调试
#define EFFECT_DBG_ADJUST_IN_UART       1          //是否使能UART在线调试音效,该功能需要打开EFFECT_DBG_ADJUST_EN和EQ_DBG_IN_UART


/*****************************************************************************
 * Module    : I2S配置
 *****************************************************************************/
#define I2S_EN                          (I2S_AUDIO_IN_EN ||  I2S_AUDIO_OUT_EN || I2S_AUDIO_IN_OUT_EN)          //是否使能I2S功能
#define I2S_DEVICE                      I2S_DEV_NO          //I2S设备选择
#define I2S_MAPPING_SEL                 I2S_IO_G2           //I2S IO口选择
#define I2S_BIT_MODE                    I2S_32BIT           //I2S数据位宽选择 16bit;32bit(16bit mode 待调试)
#define I2S_DATA_MODE                   I2S_NORMAL          //I2S数据格式选择 left-justified mode; normal mode(left-justified mode 待调试)
#define I2S_DMA_EN                      1                   //I2S数据源选择 0:src; 1:dma(src 模式待调试)
#define I2S_MASTER_EN                   1                   //I2S是否为主机模式
#define I2S_MCLK_EN                     1                   //I2S_MASTER是否打开MCLK
#define I2S_MCLK_SEL                    2                   //I2S MCLK选择 0:64fs 1:128fs 2:256fs
#define I2S_PCM_MODE                    0                   //I2S是否打开PCM mode(PCM mdoe 待调试)
#define I2S_DAC_OUT_SET                 SPR_48000           //做从机时，配置dac采样率，做主机时可忽略(暂时支持48K采样率)
#define I2S_DMA_EN                      1                   //I2S数据传输都采用DMA方式
#define I2S_SAMPLES_OUT_SET             I2S_48000           //I2S输出采样率设置(I2S_16000,I2S_48000)

#define I2S_AUDIO_IN_EN                (ADAPTER_I2S_IN_EN)          //是否使能IIS输入音频
#define I2S_AUDIO_OUT_EN               (ADAPTER_I2S_OUTPUT_EN)      //是否使能适配器端IIS输出音频
#define I2S_AUDIO_IN_OUT_EN            (ADAPTER_I2S_IN_OUT_EN)      //是否使能适配器IIS双工通信

/*****************************************************************************
 * Module    : SPI配置
 *****************************************************************************/
#define SPI_HW_EN                       0              //是否使能硬件SPI功能（默认使用SPI1）
#define SPI_2WIRE_EN                    0              //1: 2线半双工模式（SPICLK、SPIDO和SPIDI复用）; 0: 3线全双工模式（独立的SPICLK、SPIDI、SPIDO）
#define SPI_3WIRE_DUAL_MODE_EN          0              //是否使能2bit数据位宽传输模式
#define SPI_MASTER_EN                   1              //0:SALAVE 1:MASTER
#define SPI_IRQ_EN                      0              //是否使能SPI中断功能(仅适用于DMA方式,TX或RX完成时,产生中断)
#define SPI_MAPPING                     SPI1MAP_G3     //选择SPI mapping
#define SPI_BAUD_RATE                   2000000        //SPI波特率2M

#define SPI_CS_EN                       0              //是否使能SPI CS功能
#define SPI_CS_GP                       A
#define SPI_CS_BIT                      BIT(15)

/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
//按键通用配置
#define KEY_MAX_NB                      6           //按键数量（不包括IR）
#define DOUBLE_KEY_TIME                 (xcfg_cb.double_key_time)                       //按键双击响应时间（单位50ms）
#define PWRON_PRESS_TIME                (500*xcfg_cb.pwron_press_time)                  //长按PWRKEY多长时间开机
#define PWROFF_PRESS_TIME               xcfg_cb.pwroff_press_time                       //长按PWRKEY多长时间关机

//pwrkey
#define PWRKEY_EN                       1           //PWRKEY的使用，0为不使用
#define PWRKEY_IS_PRESS()               pwrkey_is_pressed()

//touch key
#define TKEY_EN                         0           //TouchKEY的使用，0为不使用
#define TKEY_SOFT_PWR_EN                0           //是否使用TouchKey进行软开关机
#define TKEY_LOWPWR_WAKEUP_DIS          0           //是否电池低电时关掉触摸唤醒, 无保护板的电池需要打开。
#define TKEY_INEAR_EN                   0           //是否使用TouchKey的入耳检测功能
#define TKEY_TEMP_EN                    0           //是否使用TouchKey的温度检测功能
#define TKEY_DEBUG_EN                   0           //仅调试使用，用于确认TKEY的参数
#define TKEY_IS_PRESS()                 (tkey_is_pressed() && TKEY_SOFT_PWR_EN)

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
#define IOKEY_GPIO_SEL0                 IO_PF0
#define IOKEY_GPIO_SEL1                 IO_PF1
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
 * Module    : 功放mute及耳机检测配置
 *****************************************************************************/
//耳机插入检测GPIO
#define EARPHONE_DETECT_EN              0           //是否打开耳机检测
#define SDCMD_MUX_DETECT_EARPHONE       0           //是否复用SDCMD检测耳机插入
#define EARPHONE_DETECT_INIT()
#define EARPHONE_IS_ONLINE()
#define IS_DET_EAR_BUSY()

//功放MUTE控制GPIO
#define LOUDSPEAKER_MUTE_EN             1           //是否使能功放MUTE
#define LOUDSPEAKER_MUTE_INIT()         loudspeaker_mute_init()
#define LOUDSPEAKER_MUTE_DIS()          loudspeaker_disable()
#define LOUDSPEAKER_MUTE()              loudspeaker_mute()
#define LOUDSPEAKER_UNMUTE()            loudspeaker_unmute()
#define LOUDSPEAKER_UNMUTE_DELAY        6           //UNMUTE延时配置，单位为5ms
//AB/D类功放控制
#define LOUDSPEAKER_MUTE_AB_D_EN        0           //分压式AB/D切换
#define LOUDSPEAKER_MUTE_AB_D_PULSE_EN  1           //一线脉冲AB/D切换
#define AMPLIFIER_SEL_INIT()            amp_sel_cfg_init(xcfg_cb.ampabd_io_sel)
#define AMPLIFIER_SEL_D()               amp_sel_cfg_d()
#define AMPLIFIER_SEL_AB()              amp_sel_cfg_ab()


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
#define CHARGE_DC_IN()                  ((RTCCON >> 20) & 0x01)
#define CHARGE_INBOX()                  ((RTCCON >> 22) & 0x01)
#define CHARGE_LOW_POWER_EN             0           //是否打开充电低功耗模式,打开此宏时，不能 INTF_HUART 传输数据
#define CHARGE_5V_POWER_SUPPLY_EN       0           //是否使用5V VUSB进行供电(此时不用vbat供电,注意要关闭CHARGE_DC_NOT_PWRON,并调大恒流充电电流)
#define CHARGE_ON_WORK_EN               0           //是否打开边用边充电的功能

//充电截止电流
#define CHARGE_STOP_CURR                xcfg_cb.charge_stop_curr
//充电截止电压：0:4.2v 1:4.35v 2:4.4v 3:4.45v
#define CHARGE_STOP_VOLT                0
//恒流充电（电池电压大于2.9v）电流
#define CHARGE_CONSTANT_CURR            xcfg_cb.charge_constant_curr  //最大可配置恒流充电电流400mA，为了留些裕量配置文件中最大可配置设为360mA
//涓流截止电压：0:2.9v; 1:3v
#define CHARGE_TRICK_STOP_VOLT          1
//涓流充电（电池电压小于2.9v）电流
#define CHARGE_TRICKLE_CURR             xcfg_cb.charge_trickle_curr
//恒压差充电差值选择：0:187.5mV  1:250mV  2:312mV  3:375mV
#define CHARGE_VOLT_FOLLOW_DIFF         3
//漏电电流选择
#define CHARGE_LEAKAGE_CURR             2   //0~3
//仓维持电压选择
#define CHARGE_BOX_KEEP_VOLT            0   //0=1.1V, 1=1.7V

#define IPHONE_POWER_VAL                50  //苹果充电电流设置
#define IPHONE_POWER_INDEX              190 //苹果充电电流设置

/*****************************************************************************
* Module    : NTC预警关机功能配置
******************************************************************************/
#define USER_NTC                        0
#define ADCCH_NTC                       ADCCH_PB0


/*****************************************************************************
 * Module    : 其他配置
 *****************************************************************************/
#define RGB_SERIAL_EN                   0           //串行RGB推灯功能
#define PWM_RGB_EN                      0           //PWM RGB三色灯功能
#define ENERGY_LED_EN                   0           //能量灯软件PWM显示,声音越大,点亮的灯越多.
#define TSEN_DETECT_EN                  0           //温度检测

/*****************************************************************************
 * Module    : 外部PA/LNA配置
 *****************************************************************************/
#define BT_RF_PWR_BALANCE_EN            0           //RF电流配平宏，对电流底噪有一定的优化

/*****************************************************************************
 * Module    : Sensor配置
 *****************************************************************************/
#define SC7A20_EN                       0           //是否使能敲击芯片功能（需要配置I2C）

/*****************************************************************************
 * Module    : I2C配置
 *****************************************************************************/
#define I2C_HW_EN                       0           //是否使能硬件I2C功能
#define I2C_MAPPING                     I2CMAP_PA14PA13 //选择I2C mapping

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
 * Module    : UART普通串口配置
 *****************************************************************************/
#define USER_UART1_EN                   0                //是否使能uart1功能
#define USER_UART1_BAND                 9600             //uart1波特率
#define USER_UART1_MAPPING              UTX1MAP_G1       //选择uart1 mapping

#define USER_UART2_EN                   0                //是否使能uart2功能
#define USER_UART2_BAND                 115200           //uart2波特率
#define USER_UART2_MAPPING              UTX2MAP_G1       //选择uart2 mapping

#define USER_DOUBLE_LINE_EN             0                //是否使能uart的双线模式

/*****************************************************************************
 * Module    : RTC时钟配置
 *****************************************************************************/
#define RTC_CLOCK_EN                    0           //RTC时钟使能
#define RTC_CALIBRATION_CYCLE           900         //15分钟校准周期(关机之后, 多长时间唤醒一次去校准时间, 单位: 秒)

/*****************************************************************************
 * Module    : LEDC配置
 *****************************************************************************/
#define LEDC_HW_EN                      0           //是否使能模块LEDC功能
#define LEDC_MAPPING                    LEDCMAP_PB2 //选择LEDC mapping
#define LEDC_RGB_NUM                    12
#define LEDC_MODE_NUM                   2                    //灯效效果数量
/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 0           //是否打开提示音功能, 总开关
#define WARNING_WSBC_RES_EN             0           //是否支持wsbc提示音(暂未调试)
#define WARNING_MP3_RES_EN              1           //是否支持mp3提示音
#define WARNING_WAV_RES_EN              0           //是否支持wav提示音(暂未调试)
#define WARNING_PIANO_RES_EN            0           //是否支持piano提示音

#define WARNING_BREAK_EN                1           //是否支持提示音打断功能（主要是打断开机提示音和TWS副耳断开提示音）
#define WARING_MAXVOL_TYPE              RES_TYPE_TONE            //最大音量提示音类型
#define LANG_SELECT                     LANG_EN_ZH   //提示音语言选择（flash空间有限时选单语）
#define WARNING_SYSVOL_ADJ_EN           1            //播放提示音过程中，系统音量是否支持退避

#define WARNING_POWER_ON                1
#define WARNING_POWER_OFF               1
#define WARNING_FUNC_MUSIC              0
#define WARNING_FUNC_BT                 0
#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                0
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             0
#define WARNING_BT_WAIT_CONNECT         0
#define WARNING_BT_PAIR                 0            //BT PAIRING提示音
#define WARNING_BT_CONNECT              0
#define WARNING_BT_DISCONNECT           0
#define WARNING_BT_LOW_LATENCY          0
#define WARNING_BT_CALL_CTRL            0           //接听、挂断、回拨提示音
#define WARNING_BT_RING_NUMBER          0           //来电报号
#define WARNING_LEFT_RIGHT_CH           0           //1=左右先后播报left/right channel，2=左右同时播报left/right channel
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              0
#define WARNING_MIN_VOLUME              0
#define WARNING_BT_HID_MENU             0            //BT HID MENU手动连接/断开HID Profile提示音
#define WARNING_BTHID_CONN              0            //BTHID模式是否有独立的连接/断开提示音
#define WARNING_TAKE_PHOTO              0


#define SW_VERSION      "V0.0.1"        //只能使用数字0-9,ota需要转码
#define HW_VERSION      "V0.0.1"        //只能使用数字0-9,ota需要转码
#include "config_extra.h"

#endif // USER_CONFIG_H
