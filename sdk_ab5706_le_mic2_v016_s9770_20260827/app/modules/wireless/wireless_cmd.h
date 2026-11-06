#ifndef _WIRELESS_CMD_API_H
#define _WIRELESS_CMD_API_H

#define CMD_MAX_NB          4
#define CMD_MAX_SIZE        9

#define CMD_MAX_EXT_SIZE    32
#if ADAPTER_SAVE_PARAM_EN
#define TX_MAX_BUF_SIZE     (CMD_MAX_EXT_SIZE - 1)
#else
#define TX_MAX_BUF_SIZE     (CMD_MAX_SIZE - 1)
#endif

//内部定义
typedef enum{
    PRIVATE_WS_MIC_CMD      = 0,
    PRIVATE_USB_CMD,
    PRIVATE_SYNC_CMD,           ///tx端发送变化信息给rx端，rx端再把信息发给另一个tx端实现两个tx同步
    PRIVATE_USER_DATA,
    PRIVATE_PWR_CTR_CMD,

#if ADAPTER_SAVE_PARAM_EN
    PRIVATE_CON_INTERACT_DATA = 0x20,
#endif
} cmd_t;

typedef struct wireless_cmd{
    u8 cmd;
    u8 buf[TX_MAX_BUF_SIZE];
} wireless_cmd_t;

//无线麦控制
typedef enum{
    ECHO_AUDIO_MUTE         = 0,
    ADPCM_CODER_MUTE,

    ADPCM_DECODER_MUTE,
    PLC_ADPCM_AUDIO_MUTE,
    SNDP_AUDIO_MUTE,
    AINS3_AUDIO_MUTE,
    SRC_AUDIO_MUTE,
    DAC_OUT_AUDIO_MUTE,
    USB_MIC_IN_AUDIO_MUTE,
    MAGIC_AUDIO_MUTE,
    HOWLING_AUDIO_MUTE,
    MIC_MUTE,
    BONDING_SYNC,

    ECHO_DELAY_LEVEL_UP     = 0x40,     //混响等级加
    ECHO_DELAY_LEVEL_DOWN,              //混响等级减
    MAGIC_LEVEL_CHANGE,                 //切换魔音音效

    MUSIC_CTL_1ST           = 0x60,
    MUSIC_CTL_VOL_UP = MUSIC_CTL_1ST,   //音乐音量加
    MUSIC_CTL_VOL_DOWN,                 //音乐音量减
    MUSIC_CTL_PREV,                     //上一曲
    MUSIC_CTL_NEXT,                     //下一曲
    MUSIC_CTL_PLAY_PAUSE,               //播放/暂停
    MUSIC_CTL_VOICE_REMOVE,             //人声消除切换

    DISCONNECT_NUM,                     //断开无线麦连接
} sub_ws_mic_cmd_t;

//USB播放控制
typedef enum{
    USB_SET_SPK_VOLUME      = 0,
    USB_CTL_MIC_STA,
    USB_CTL_PLAY_PAUSE,
    USB_CTL_VOLUME_UP,
    USB_CTL_VOLUME_DOWN,
    USB_CTL_PREVFILE,
    USB_CTL_NEXTFILE,
} sub_usb_cmd_t;

//私人控制消息
typedef enum{
    PRIVATE_SYNC_ECHO_DELAY_LEVEL      = 0,//k2与k3双击
    PRIVATE_SYNC_SOFT_GAIN_LEVEL,//k2与k3短按
    PRIVATE_SYNC_MAGIC_EFFECT_LEVEL,//K4长按
    PRIVATE_SYNC_MIC_MUTE_LEVEL,//k1短按
    PRIVATE_SYNC_VOICE_RM,//k4短按 4
} sub_private_ctl_cmd_t;

//私人控制消息
typedef enum{
    CON_INTERACT_GET_INFO,              //获取版本信息
    CON_INTERACT_XCFG_PARAM,
    CON_INTERACT_EQ_PARAM,
    CON_INTERACT_DRC_PARAM,
    CON_INTERACT_TX_COMPLETE,
    CON_INTERACT_SKIP_FIRST,

    CON_INTERACT_SET_RUN,               //运行某段地址指令
    CON_INTERACT_WR_RAM,                //写一段内存
    CON_INTERACT_RD_RAM,                //读一段内存
    CON_INTERACT_CMD_MAX,

    CON_INTERACT_RSP = 0x80,
} con_interact_cmd_t;


//device <==> adapter私有命令api
void wireless_cmd_init(void);
void wireless_rx_cmd(u8 index, u8 *ptr, u8 len);
void wireless_tx_usb_cmd(u8 msg, u8 param);
void wireless_tx_user_cmd(u8 index,u8 *ptr, u8 len);
uint8_t wireless_cmd_is_empty(u8 index);

//device <==> adpater USB播放控制api
#if !ADAPTER_USB_SPK_EN
    #define wireless_music_play_pause()
    #define wireless_music_volup()
    #define wireless_music_voldown()
    #define wireless_music_prev()
    #define wireless_music_next()
    #define wireless_set_usbspk_vol(vol)
#else
    #define wireless_music_play_pause()             wireless_tx_usb_cmd(USB_CTL_PLAY_PAUSE, 0)      //切换播放/暂停
    #define wireless_music_volup()                  wireless_tx_usb_cmd(USB_CTL_VOLUME_UP, 0)       //音量加
    #define wireless_music_voldown()                wireless_tx_usb_cmd(USB_CTL_VOLUME_DOWN, 0)     //音量减
    #define wireless_music_prev()                   wireless_tx_usb_cmd(USB_CTL_PREVFILE, 0)        //上一曲
    #define wireless_music_next()                   wireless_tx_usb_cmd(USB_CTL_NEXTFILE, 0)        //下一曲
    #define wireless_set_usbspk_vol(vol)            wireless_tx_usb_cmd(USB_SET_SPK_VOLUME, vol)    //spk_vol
#endif

#if (!(ADAPTER_USB_MIC_RX_EN || ADAPTER_USB_SPK_EN))
    #define wireless_set_usbmic_status(start)
#else
    #define wireless_set_usbmic_status(start)       wireless_tx_usb_cmd(USB_SET_SPK_VOLUME, start)  //mic_status
#endif

//device <==> adapter无线麦命令
#define wirless_echo_level_up()                     wireless_tx_ws_mic_cmd(ECHO_DELAY_LEVEL_UP, 1)
#define wirless_echo_level_down()                   wireless_tx_ws_mic_cmd(ECHO_DELAY_LEVEL_DOWN, 1)
#define wirless_magic_change()                      wireless_tx_ws_mic_cmd(MAGIC_LEVEL_CHANGE, 1)
#define wirless_music_vol_up()                      wireless_tx_ws_mic_cmd(MUSIC_CTL_VOL_UP, 1)
#define wirless_music_vol_down()                    wireless_tx_ws_mic_cmd(MUSIC_CTL_VOL_DOWN, 1)
#define wirless_music_prev()                        wireless_tx_ws_mic_cmd(MUSIC_CTL_PREV, 1)
#define wirless_music_next()                        wireless_tx_ws_mic_cmd(MUSIC_CTL_NEXT, 1)
#define wirless_music_play_pause()                  wireless_tx_ws_mic_cmd(MUSIC_CTL_PLAY_PAUSE, 1)
#define wirless_music_voice_remove()                wireless_tx_ws_mic_cmd(MUSIC_CTL_VOICE_REMOVE, 1)


//外部函数声明
void wireless_cmd_buf_init(void);
bool wireless_send_cmd(u8 index, u8 *cmd, u8 len);
void wireless_rx_user_cmd(u8 index, u8 *ptr, u8 len);
void wireless_tx_pwr_ctr_cmd(u8 index, u8 pwr_level);

void wireless_tx_bonding_sync(u8 index, u8 cmd_msg, u8 *bonding_addr, u8 len);
#endif
