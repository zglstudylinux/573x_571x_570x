/*****************************************************************************
 * Module    : Configs
 * File      : config_define.h
 * Function  : 定义用户参数常量
 *****************************************************************************/
#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

/*****************************************************************************
 * Module    : 显示相关配置选择列表
 *****************************************************************************/
//显示驱动屏选择
#define DISPLAY_NO                      0                                       //无显示模块
#define DISPLAY_LCDSEG                  0x200                                   //选用断码屏做为显示驱动
#define DISPLAY_LEDSEG                  0x400                                   //选用数码管做为显示驱动

#define GUI_NO                          DISPLAY_NO                              //无主题，无显示
#define GUI_LCDSEG                      (DISPLAY_LCDSEG | 0x00)                 //断码屏默认主题
#define GUI_LEDSEG_5C7S                 (DISPLAY_LEDSEG | 0x00)                 //5C7S 数码管
#define GUI_LEDSEG_7P7S                 (DISPLAY_LEDSEG | 0x01)                 //7PIN 数码管 按COM方式扫描
#define GUI_LEDSEG_3P7S                 (DISPLAY_LEDSEG | 0x02)                 //3PIN 7段数码管
#define GUI_LEDSEG_6C6S                 (DISPLAY_LEDSEG | 0x04)                 //6C6S 6段数码管

/*****************************************************************************
 * Module    : FLASH大小定义
 *****************************************************************************/
 #define FSIZE_1M                       0x100000
 #define FSIZE_2M                       0x200000
 #define FSIZE_4M                       0x400000
 #define FSIZE_512K                     0x80000

/*****************************************************************************
 * Module    : FOTA升级方式选择列表
 *****************************************************************************/
#define AB_FOT_TYPE_ADAPT               0   //支持压缩升级与非压缩升级,通过解析升级文件来自动识别
#define AB_FOT_TYPE_PACK                1   //FOTA压缩升级（代码做压缩处理，升级完成需做解压才可正常运行）
#define AB_FOT_TYPE_NORMAL              2   //FOTA非压缩升级（代码完全双备份，不做压缩处理，升级完成可直接运行）

/*****************************************************************************
 * Module    : 提示音语言配置选择列表
 *****************************************************************************/
#define LANG_EN         0               //英文提示音
#define LANG_ZH         1               //中文提示音
#define LANG_EN_ZH      2               //英文、中文双语提示音


/*****************************************************************************
 * Module    : 提示音资源类型选择列表
 *****************************************************************************/
#define RES_TYPE_INVALID    0           //非法提示音
#define RES_TYPE_TONE       1           //PIANO音调，可叠加到music
#define RES_TYPE_PIANO      2           //PIANO提示音，可叠加到music
#define RES_TYPE_MP3        3           //MP3提示音，不能叠加
#define RES_TYPE_WAV        4           //WAV提示音，可叠加到music
#define RES_TYPE_ESBC       5           //ESBC提示音（暂不支持）
#define RES_TYPE_WSBC       6           //WSBC提示音，可叠加到music
/*****************************************************************************
 * Module    : 采样率选择列表
 *****************************************************************************/
//USB采样率选择
#define SPL_48000_EN            BIT(0)
#define SPL_44100_EN            BIT(1)
#define SPL_8000_EN             BIT(2)
#define SPL_96000_EN            BIT(3)
#define SPK_SPL_SEL             (SPL_48000_EN)
#define MIC_SPL_SEL             (SPL_48000_EN)

//USB采样位宽选择
#define USB_16BITS_EN           BIT(0)
#define USB_24BITS_EN           BIT(1)
#define USB_32BITS_EN           BIT(2)
#define SPK_BITS_SEL            (USB_16BITS_EN | USB_24BITS_EN)                 //支持配置16/24bit
#define MIC_BITS_SEL            (USB_16BITS_EN | USB_24BITS_EN)                 //支持配置16/24bit

/*****************************************************************************
 * Module    : 采样率选择列表
 *****************************************************************************/
#define SAMPLE_RATE_48K     0
#define SAMPLE_RATE_32K     3
#define SAMPLE_RATE_24K     4
#define SAMPLE_RATE_16K     6

/*****************************************************************************
 * Module    : ADC通路选择列表
 *****************************************************************************/
#define ADCCH_PB0       0               //SARADC channel 0
#define ADCCH_PB1       1               //SARADC channel 1
#define ADCCH_PB2       2               //SARADC channel 2
#define ADCCH_PB3       3               //SARADC channel 3
#define ADCCH_PB4       4               //SARADC channel 4
#define ADCCH_WKO       5               //SARADC channel 5                  WK0/PB5
#define ADCCH_PB6       6               //SARADC channel 6                  WK1
#define ADCCH_PB7       7               //SARADC channel 7                  WK2
#define ADCCH_PE4       8               //SARADC channel 8
#define ADCCH_PE5       9               //SARADC channel 9                  ADS7844 ADC0
#define ADCCH_PE6       10              //SARADC channel 10
#define ADCCH_PE7       11              //SARADC channel 11
#define ADCCH_VUSB      12              //SARADC channel 12                 WKO/PB5
#define ADCCH_BGOP      13              //SARADC channel 13
#define ADCCH_VBAT      14              //SARADC channel 14
#define ADCCH_VRTC      15              //SARADC channel 15
#define ADCCH_TSENSOR   15              //SARADC channel 15
#define ADCCH_SVRC      15              //SARADC channel 15

/*****************************************************************************
 * Module    : Timer3 Capture Mapping选择列表 (FUNCMCON2)
 *****************************************************************************/
#define TMR3MAP_PB2     (1 << 4)       //G1  capture mapping: PB2
#define TMR3MAP_PB3     (2 << 4)       //G2  capture mapping: PB3
#define TMR3MAP_PB4     (3 << 4)       //G3  capture mapping: PB4
#define TMR3MAP_PB5     (4 << 4)       //G4  capture mapping: PB5
#define TMR3MAP_PB6     (5 << 4)       //G5  capture mapping: PB6
#define TMR3MAP_PB7     (6 << 4)       //G6  capture mapping: PB7
#define TMR3MAP_PE5     (7 << 4)       //G7  capture mapping: PE5
#define TMR4MAP_PE6     (8 << 4)       //G8  capture mapping: PE6
#define TMR5MAP_PE7     (9 << 4)       //G9  capture mapping: PE7

/*****************************************************************************
 * Module    : IRRX Mapping选择列表 (FUNCMCON2)
 *****************************************************************************/
#define IRMAP_PB2       (1 << 20)       //G1  capture mapping: PB2
#define IRMAP_PB3       (2 << 20)       //G2  capture mapping: PB3
#define IRMAP_PB4       (3 << 20)       //G3  capture mapping: PB4
#define IRMAP_PB5       (4 << 20)       //G4  capture mapping: PB5
#define IRMAP_PB6       (5 << 20)       //G5  capture mapping: PB6
#define IRMAP_PB7       (6 << 20)       //G6  capture mapping: PB7
#define IRMAP_PE5       (7 << 20)       //G7  capture mapping: PE5
#define IRMAP_PE6       (8 << 20)       //G8  capture mapping: PE6
#define IRMAP_PE7       (9 << 20)       //G9  capture mapping: PE7


/*****************************************************************************
 * Module    : Clock output Mapping选择列表 (FUNCMCON1)
 *****************************************************************************/
#define CLKOMAP_PB3     (1 << 0)        //G1 Clock output mapping: PB3
#define CLKOMAP_PB4     (2 << 0)        //G2 Clock output mapping: PB4
#define CLKOMAP_PB6     (3 << 0)        //G3 Clock output mapping: PB6
#define CLKOMAP_PB7     (4 << 0)        //G4 Clock output mapping: PB7
#define CLKOMAP_PE6     (5 << 0)        //G5 Clock output mapping: PE6
#define CLKOMAP_PE7     (6 << 0)        //G6 Clock output mapping: PE7


/*****************************************************************************
 * Module    : sd0 Mapping选择列表 (FUNCMCON0)
 *****************************************************************************/
#define SD0MAP_G1       (1 << 0)       //G1  SDCMD(PE5),  SDCLK(PE6),  SDDAT0(PE7)
#define SD0MAP_G2       (2 << 0)       //G2  SDCMD(PB4),  SDCLK(PB2),  SDDAT0(PB3)
#define SD0MAP_G3       (3 << 0)       //G3  SDCMD(PB5),  SDCLK(PB6),  SDDAT0(PB7)
#define SD0MAP_G4       (4 << 0)       //G4  SDCMD(PB0),  SDCLK(PB1),  SDDAT0(PB2)


/*****************************************************************************
 * Module    : spi1 Mapping选择列表 (FUNCMCON1)
 *****************************************************************************/
#define SPI1MAP_G1     (1 << 4)        //G1 SPI1CLK(PE6), SPI1DI(PE5), SPI1DO(PE7)
#define SPI1MAP_G2     (2 << 4)        //G2 SPI1CLK(PB3), SPI1DI(PB2), SPI1DO(PB4)
#define SPI1MAP_G3     (3 << 4)        //G3 SPI1CLK(PB6), SPI1DI(PB5), SPI1DO(PB7)
#define SPI1MAP_G4     (4 << 4)        //G4 SPI1CLK(PB0), SPI1DI(PB2), SPI1DO(PB1)


/*****************************************************************************
 * Module    : uart0 Mapping选择列表
 *****************************************************************************/
#define UTX0MAP_PB2     (1 << 8)        //G1 uart0 tx: PB2
#define UTX0MAP_PE7     (2 << 8)        //G2 uart0 tx: PE7
#define UTX0MAP_PB3     (3 << 8)        //G3 uart0 tx: PB3  //USBDP
#define UTX0MAP_PB7     (4 << 8)        //G4 uart0 tx: PB7
#define UTX0MAP_PE3     (5 << 8)        //G5 uart0 tx: PE3
#define UTX0MAP_VUSB    (6 << 8)        //G6 uart0 tx: VUSB

#define URX0MAP_PB1     (1 << 12)       //G1 uart0 rx: PB1
#define URX0MAP_PE6     (2 << 12)       //G2 uart0 rx: PE6
#define URX0MAP_PB4     (3 << 12)       //G3 uart0 rx: PB4
#define URX0MAP_PB6     (4 << 12)       //G4 uart0 rx: PB6
#define URX0MAP_PE2     (5 << 12)       //G5 uart0 rx: PE2
#define URX0MAP_TX      (7 << 12)       //G7 uart0 map to TX pin by UT0TXMAP select(1线模式)

/*****************************************************************************
 * Module    : 录音文件类型列表
 *****************************************************************************/
#define REC_NO          0
#define REC_WAV         1              //PCM WAV
#define REC_ADPCM       2              //ADPCM WAV
#define REC_MP3         3
#define REC_SBC         4

/*****************************************************************************
* Module    : I2S0设备列表 (FUNCMCON2)
*****************************************************************************/
#define I2S_DEV_NO      0
#define I2S_DEV_TAS5711 1
#define I2S_DEV_WM8978  2

#define I2S_MASTER      0
#define I2S_SLAVE       1

//        G1    G2    G3
//DI  :   PB7   PB1   PE0
//DO  :   PE7   PB2   PE3
//BCLK:   PE5   PB3   PB3
//LRCLK:  PE6   PB4   PB4
//MCLK:   PE4   PB0   PE1

#define I2S_IO_G1           0
#define I2S_IO_G2           1
#define I2S_IO_G3           2

/*****************************************************************************
* Module    : SPDIF Channel列表
*****************************************************************************/
#define SPF_RX_PB6_CH0      0
#define SPF_RX_PB7_CH1      1
#define SPF_RX_PE3_CH2      2
#define SPF_RX_PE4_CH3      3
#define SPF_RX_PE7_CH4      4
#define SPF_RX_PE8_CH4      5

#define SPF_TX_PA10_CH0     0
#define SPF_TX_PA11_CH1     1
#define SPF_TX_PA12_CH2     2
#define SPF_TX_PB12_CH3     3
#define SPF_TX_PB13_CH4     4
#define SPF_TX_PE0_CH4      5

/*****************************************************************************
* Module    : DAC SELECT
*****************************************************************************/
#define DAC_MONO            0              //DAC单声道输出
#define DAC_DUAL            1              //DAC双声道输出
#define DAC_VCMBUF_MONO     2              //DAC VCMBUF单声道输出
#define DAC_VCMBUF_DUAL     3              //DAC VCMBUF双声道输出
#define DAC_DIFF_MONO       4              //DAC DIFF单声道
#define DAC_DIFF_DUAL       5              //DAC DIFF双声道

/*****************************************************************************
* Module    : DAC OUT Sample Rate
*****************************************************************************/
#define DAC_OUT_44K1        0               //dac out sample rate 44.1K
#define DAC_OUT_48K         1               //dac out sample rate 48K
#define DAC_OUT_88K2        2               //dac out sample rate 88.2k
#define DAC_OUT_96K         3               //dac out sample rate 96k

/*****************************************************************************
* Module    : DAC LDOH Select
*****************************************************************************/
#define AU_LDOH_2V4         0               //VDDAUD LDO voltage 2.4V
#define AU_LDOH_2V5         1               //VDDAUD LDO voltage 2.5V
#define AU_LDOH_2V7         2               //VDDAUD LDO voltage 2.7V
#define AU_LDOH_2V9         3               //VDDAUD LDO voltage 2.9V
#define AU_LDOH_3V1         4               //VDDAUD LDO voltage 3.1V
#define AU_LDOH_3V2         5               //VDDAUD LDO voltage 3.2V

/*****************************************************************************
* Module    : 外接收音时钟选择
*****************************************************************************/
#define FM_USE_CRYSTAL_32K          0   //32.768k独立晶振
#define FM_USE_CRYSTAL_12M          1   //12M独立晶振
#define FM_USE_IO_12MHZ_CLK         2   //IO输出12M时钟
#define FM_SHARE_CRYSTAL_32KHz      3   //共用主按32K晶振，主控需要挂32K晶振

/*****************************************************************************
* Module    : AUX or MIC Left&Right channel list
* AUX: 可以任意左与右搭配，或只选择左，或只选择右
*****************************************************************************/
#define MIC0                        0x01        //MIC0
#define MIC1                        0x02        //MIC1

#define AUXR0                       0x06        //PE7
#define AUXL0                       0x07        //PE6
#define AUXR1                       0x08        //PA7
#define AUXL1                       0x09        //PA6

#define ADC0                        0x00        //ADC0, mic
#define ADC1                        0x01        //ADC1, mic

//ADC0 channel config
#define CH_MIC0                     (ADC0 << 4 | MIC0)      //MIC0          -> ADC0

//ADC1 channel config
#define CH_MIC1                     (ADC1 << 4 | MIC1)      //MIC1          -> ADC1

//ADC2 channel config
#define CH_MIC2                     0           //(不支持)

//ADC3 channel config
#define CH_MIC3                     0           //(不支持)

//ADC4 channel config
#define CH_MIC4                     0           //(不支持)

//AUX - ADC SEL
#define CH_AUXL0                    (ADC0 << 4 | AUXL0)     //PE6           -> ADC0
#define CH_AUXR0                    (ADC1 << 4 | AUXR0)     //PE7           -> ADC1
#define CH_AUXL1                    (ADC0 << 4 | AUXL1)     //PA6           -> ADC0
#define CH_AUXR1                    (ADC1 << 4 | AUXR1)     //PA7           -> ADC1

/*****************************************************************************
* Module    : AUX Analog gain level list, 与配置选项对应
*****************************************************************************/
#define AUX_P12DB           0       //AUX Analog +12DB
#define AUX_P9DB            1       //AUX Analog +9DB
#define AUX_P6DB            2       //AUX Analog +6DB
#define AUX_P0DB            3       //AUX Analog  0DB
#define AUX_N3DB            4       //AUX Analog -3DB
#define AUX_N6DB            5       //AUX Analog -6DB
#define AUX_N9DB            6       //AUX Analog -9DB
#define AUX_N12DB           7       //AUX Analog -12DB

/*****************************************************************************
* Module    : 电池低电电压列表
*****************************************************************************/
#define VBAT_2V8            0       //2.8v
#define VBAT_2V9            1       //2.9v
#define VBAT_3V0            2       //3.0v
#define VBAT_3V1            3       //3.1v
#define VBAT_3V2            4       //3.2v
#define VBAT_3V3            5       //3.3v
#define VBAT_3V4            6       //3.4v
#define VBAT_3V5            7       //3.5v
#define VBAT_3V6            8       //3.6v
#define VBAT_3V7            9       //3.7v
#define VBAT_3V8            10      //3.8v

/*****************************************************************************
* Module    : uart0 printf IO列表
*****************************************************************************/
#define PRINTF_NONE         0           //关闭UART0打印信息
#define PRINTF_PB2          1           //与SD0MAP_G2, SD0MAP_G4同一IO
#define PRINTF_PE7          2           //与SD0MAP_G1同一IO
#define PRINTF_PB3          3           //与USBDP, SD0MAP_G2同一IO
#define PRINTF_PB7          4           //与SD0MAP_G3同一IO
#define PRINTF_PE3          5
#define PRINTF_VUSB         6

/*****************************************************************************
* Module    : GPIO list
*****************************************************************************/
#define IO_NONE             0
#define IO_PB0              9
#define IO_PB1              10
#define IO_PB2              11
#define IO_PB3              12
#define IO_PB4              13
#define IO_PB5              14
#define IO_PB6              15
#define IO_PB7              16
#define IO_PE0              17
#define IO_PE1              18
#define IO_PE2              19
#define IO_PE3              20
#define IO_PE4              21
#define IO_PE5              22
#define IO_PE6              23
#define IO_PE7              24
#define IO_VUSB             25

#define IO_MIN_NUM          IO_PB0
#define IO_MAX_NUM          IO_PE7

#define IO_MUX_SDCLK        56
#define IO_MUX_SDCMD        57
#define IO_MUX_PWRKEY       58
#define IO_MUX_MICL         59


/*****************************************************************************
* Module    : 串口通信类型选择
*****************************************************************************/
#define INTF_NONE           0
#define INTF_UART1          1         //普通串口1
#define INTF_HUART          2         //高速串口(DMA模式)

/*****************************************************************************
* Module    : 充电仓类型选择
*****************************************************************************/
#define CBOX_NONE           0
#define CBOX_NOR            1         //普通充电仓
#define CBOX_SSW            2         //智能充电仓
#define CBOX_NWB            3         //没有电池的普通充电仓

/*****************************************************************************
 * Module    : I2C Mapping选择列表 FUNCMCON2[27:24]
 *****************************************************************************/

#define I2CMAP_PB1PB0         (1 << 24)        //G1 i2c clk: PB0 dat: PB1
#define I2CMAP_PB2PB1         (2 << 24)        //G2 i2c clk: PB1 dat: PB2
#define I2CMAP_PB4PB3         (3 << 24)        //G3 i2c clk: PB3 dat: PB4
#define I2CMAP_PB7PB6         (4 << 24)        //G4 i2c clk: PB6 dat: PB7
#define I2CMAP_PE1PE0         (5 << 24)        //G5 i2c clk: PE0 dat: PE1
#define I2CMAP_PE5PE6         (6 << 24)        //G6 i2c clk: PE6 dat: PE5
#define I2CMAP_PE7PE6         (7 << 24)        //G7 i2c clk: PB6 dat: PB7

/*****************************************************************************
 * Module    : codec选择列表
 *****************************************************************************/
#define WS_CODEC_MONO           0           //flag
#define WS_CODEC_STEREO         BIT(7)      //flag
#define WS_CODEC_ADPCM          0           //index
#define WS_CODEC_OPUS           1           //index
#define WS_CODEC_MSBC           2           //index
#define WS_CODEC_LC3            3           //index, lc3_7.5ms_10ms
#define WS_CODEC_LC3S           4           //index, lc3_2.5ms
#define WS_CODEC_LC3F           5           //index, lc3_5ms
#define WS_CODEC_USER1          6           //index
#define WS_CODEC_SBC            7           //index
#define WS_CODEC_LC3B           8           //index, lc3_1.25ms
#endif //CONFIG_DEFINE_H
