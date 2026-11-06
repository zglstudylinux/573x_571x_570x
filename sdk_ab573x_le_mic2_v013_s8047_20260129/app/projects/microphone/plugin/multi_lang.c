#include "include.h"

#define RES_BUF(n)              RES_BUF_MP3(n)          //主要格式，个别可单独修改tbl
#define RES_LEN(n)              RES_LEN_MP3(n)          //主要格式，个别可单独修改tbl


#if (LANG_SELECT == LANG_EN || LANG_SELECT == LANG_EN_ZH)
const res_addr_t en_lang_tbl[RES_IDX_MAX] = {
#if WARNING_POWER_ON
    [RES_IDX_POWERON        ] = {(u32 *)RES_BUF(EN_POWERON),          (u32 *)RES_LEN(EN_POWERON)},
#endif
#if WARNING_POWER_OFF
    [RES_IDX_POWEROFF       ] = {(u32 *)RES_BUF(EN_POWEROFF),         (u32 *)RES_LEN(EN_POWEROFF)},
#endif
#if (LANG_SELECT == LANG_EN_ZH)
    [RES_IDX_LANGUAGE_EN    ] = {(u32 *)RES_BUF(EN_LANGUAGE),         (u32 *)RES_LEN(EN_LANGUAGE)},
    [RES_IDX_LANGUAGE_ZH    ] = {(u32 *)RES_BUF(ZH_LANGUAGE),         (u32 *)RES_LEN(ZH_LANGUAGE)},
#endif
#if WARNING_LOW_BATTERY
    [RES_IDX_LOW_BATTERY    ] = {(u32 *)RES_BUF(EN_LOW_BATTERY),      (u32 *)RES_LEN(EN_LOW_BATTERY)},
#endif

#if WARNING_MAX_VOLUME
#if WARING_MAXVOL_TYPE == RES_TYPE_PIANO
    [RES_IDX_MAX_VOL        ] = {(u32 *)&RES_BUF_PIANO(PIANO_MAX_VOL),  (u32 *)&RES_LEN_PIANO(PIANO_MAX_VOL)},
#elif WARING_MAXVOL_TYPE == RES_TYPE_TONE
    [RES_IDX_MAX_VOL        ] = {(u32 *)&RES_BUF_TONE(TONE_MAX_VOL),    (u32 *)&RES_LEN_TONE(TONE_MAX_VOL)},
//#elif WARING_MAXVOL_TYPE == RES_TYPE_WAV
#elif WARING_MAXVOL_TYPE == RES_TYPE_MP3
    [RES_IDX_MAX_VOL        ] = {(u32 *)RES_BUF_MP3(EN_MAX_VOL),      (u32 *)RES_LEN_MP3(EN_MAX_VOL)},
#elif WARING_MAXVOL_TYPE == RES_TYPE_WSBC
    [RES_IDX_MAX_VOL        ] = {(u32 *)RES_BUF_SBC(EN_MAX_VOL),      (u32 *)RES_LEN_SBC(EN_MAX_VOL)},
#else
    [RES_IDX_MAX_VOL        ] = {(u32 *)RES_BUF(EN_MAX_VOL),          (u32 *)RES_LEN(EN_MAX_VOL)},
#endif
#endif

#if WARNING_USB_SD
    [RES_IDX_SDCARD_MODE    ] = {(u32 *)RES_BUF(EN_SDCARD_MODE),      (u32 *)RES_LEN(EN_SDCARD_MODE)},
    [RES_IDX_USB_MODE       ] = {(u32 *)RES_BUF(EN_USB_MODE),         (u32 *)RES_LEN(EN_USB_MODE)},
#endif
#if WARNING_FUNC_AUX
    [RES_IDX_AUX_MODE       ] = {(u32 *)RES_BUF(EN_AUX_MODE),         (u32 *)RES_LEN(EN_AUX_MODE)},
#endif
#if WARNING_FUNC_CLOCK
    [RES_IDX_CLOCK_MODE     ] = {(u32 *)RES_BUF(EN_CLOCK_MODE),       (u32 *)RES_LEN(EN_CLOCK_MODE)},
#endif
#if WARNING_FUNC_FMRX
    [RES_IDX_FM_MODE        ] = {(u32 *)RES_BUF(EN_FM_MODE),          (u32 *)RES_LEN(EN_FM_MODE)},
#endif
#if WARNING_FUNC_SPEAKER
    [RES_IDX_SPK_MODE       ] = {(u32 *)RES_BUF(EN_SPK_MODE),         (u32 *)RES_LEN(EN_SPK_MODE)},
#endif
#if WARNING_FUNC_USBDEV
    [RES_IDX_PC_MODE        ] = {(u32 *)RES_BUF(EN_PC_MODE),          (u32 *)RES_LEN(EN_PC_MODE)},
#endif
#if WARNING_FUNC_BT
    [RES_IDX_BT_MODE        ] = {(u32 *)RES_BUF(EN_BT_MODE),          (u32 *)RES_LEN(EN_BT_MODE)},
#endif
#if WARNING_LEFT_RIGHT_CH
    [RES_IDX_LEFT_CH        ] = {(u32 *)RES_BUF(EN_LEFT_CH),          (u32 *)RES_LEN(EN_LEFT_CH)},
    [RES_IDX_RIGHT_CH       ] = {(u32 *)RES_BUF(EN_RIGHT_CH),         (u32 *)RES_LEN(EN_RIGHT_CH)},
#endif
#if WARNING_BT_PAIR
    [RES_IDX_PAIRING        ] = {(u32 *)RES_BUF(EN_PAIRING),          (u32 *)RES_LEN(EN_PAIRING)},
#endif
#if WARNING_BT_WAIT_CONNECT
    [RES_IDX_WAIT4CONN      ] = {(u32 *)RES_BUF(EN_WAIT4CONN),        (u32 *)RES_LEN(EN_WAIT4CONN)},
#endif
#if WARNING_BT_CONNECT
    [RES_IDX_CONNECTED      ] = {(u32 *)RES_BUF(EN_CONNECTED),        (u32 *)RES_LEN(EN_CONNECTED)},
#endif
#if WARNING_BT_DISCONNECT
    [RES_IDX_DISCONNECT     ] = {(u32 *)RES_BUF(EN_DISCONNECT),       (u32 *)RES_LEN(EN_DISCONNECT)},
#endif
#if WARNING_BT_RING_NUMBER
    [RES_IDX_NUM_0          ] = {(u32 *)RES_BUF(EN_NUM_0),            (u32 *)RES_LEN(EN_NUM_0)},
    [RES_IDX_NUM_1          ] = {(u32 *)RES_BUF(EN_NUM_1),            (u32 *)RES_LEN(EN_NUM_1)},
    [RES_IDX_NUM_2          ] = {(u32 *)RES_BUF(EN_NUM_2),            (u32 *)RES_LEN(EN_NUM_2)},
    [RES_IDX_NUM_3          ] = {(u32 *)RES_BUF(EN_NUM_3),            (u32 *)RES_LEN(EN_NUM_3)},
    [RES_IDX_NUM_4          ] = {(u32 *)RES_BUF(EN_NUM_4),            (u32 *)RES_LEN(EN_NUM_4)},
    [RES_IDX_NUM_5          ] = {(u32 *)RES_BUF(EN_NUM_5),            (u32 *)RES_LEN(EN_NUM_5)},
    [RES_IDX_NUM_6          ] = {(u32 *)RES_BUF(EN_NUM_6),            (u32 *)RES_LEN(EN_NUM_6)},
    [RES_IDX_NUM_7          ] = {(u32 *)RES_BUF(EN_NUM_7),            (u32 *)RES_LEN(EN_NUM_7)},
    [RES_IDX_NUM_8          ] = {(u32 *)RES_BUF(EN_NUM_8),            (u32 *)RES_LEN(EN_NUM_8)},
    [RES_IDX_NUM_9          ] = {(u32 *)RES_BUF(EN_NUM_9),            (u32 *)RES_LEN(EN_NUM_9)},
#endif
#if WARNING_BT_LOW_LATENCY
    [RES_IDX_MUSIC_MODE     ] = {(u32 *)RES_BUF(EN_MUSIC_MODE),       (u32 *)RES_LEN(EN_MUSIC_MODE)},
    [RES_IDX_GAME_MODE      ] = {(u32 *)RES_BUF(EN_GAME_MODE),        (u32 *)RES_LEN(EN_GAME_MODE)},
#endif
#if WARNING_BT_CALL_CTRL
    [RES_IDX_CALL_HANGUP    ] = {(u32 *)RES_BUF(EN_CALL_HANGUP),      (u32 *)RES_LEN(EN_CALL_HANGUP)},
    [RES_IDX_CALL_REJECT    ] = {(u32 *)RES_BUF(EN_CALL_REJECT),      (u32 *)RES_LEN(EN_CALL_REJECT)},
    [RES_IDX_REDIALING      ] = {(u32 *)RES_BUF(EN_REDIALING),        (u32 *)RES_LEN(EN_REDIALING)},
#endif
#if WARNING_BT_HID_MENU
    [RES_IDX_CAMERA_ON      ] = {(u32 *)RES_BUF(EN_CAMERA_ON),        (u32 *)RES_LEN(EN_CAMERA_ON)},
    [RES_IDX_CAMERA_OFF     ] = {(u32 *)RES_BUF(EN_CAMERA_OFF),       (u32 *)RES_LEN(EN_CAMERA_OFF)},
#endif
    [RES_IDX_FINDMY_SOUND   ] = {(u32 *)&RES_BUF_PIANO(PIANO_FINEMY_SOUND), (u32 *)&RES_LEN_PIANO(PIANO_FINEMY_SOUND)},
    [RES_IDX_RING           ] = {(u32 *)RES_BUF_MP3(RING),            (u32 *)RES_LEN_MP3(RING)},
#if WARNING_TAKE_PHOTO
    [RES_IDX_TAKE_PHOTO     ] = {(u32 *)RES_BUF_MP3(TAKE_PHOTO),      (u32 *)RES_LEN_MP3(TAKE_PHOTO)},
#endif
    [RES_IDX_UPDATE         ] = {(u32 *)RES_BUF_MP3(UPDATE),          (u32 *)RES_LEN_MP3(UPDATE)},
    [RES_IDX_UPDATE_DONE    ] = {(u32 *)RES_BUF_MP3(UPDATE_DONE),     (u32 *)RES_LEN_MP3(UPDATE_DONE)},
#if WARNING_BT_CONNECT
    [RES_IDX_TWS_CONNECTED  ] = {(u32 *)RES_BUF(EN_CONNECTED),        (u32 *)RES_LEN(EN_CONNECTED)},
#endif
#if ANC_EN
    [RES_IDX_ANC            ] = {(u32 *)RES_BUF_WAV(ANC_ON),          (u32 *)RES_LEN_WAV(ANC_ON)},
    [RES_IDX_NR_DISABLE     ] = {(u32 *)RES_BUF_WAV(NR_DISABLE),      (u32 *)RES_LEN_WAV(NR_DISABLE)},
    [RES_IDX_TRANSPARENCY   ] = {(u32 *)RES_BUF_WAV(TRANSPARENCY),    (u32 *)RES_LEN_WAV(TRANSPARENCY)},
#endif
#if ANC_ALG_AEM_RT_FF_FB_EN
    [RES_IDX_AEM_RT_ON      ] = {(u32 *)RES_BUF_WAV(AEM_ON),          (u32 *)RES_LEN_WAV(AEM_ON)},
#endif
};
#endif

#if (LANG_SELECT == LANG_ZH || LANG_SELECT == LANG_EN_ZH)
const res_addr_t zh_lang_tbl[RES_IDX_MAX] = {
#if WARNING_POWER_ON
    [RES_IDX_POWERON        ] = {(u32 *)RES_BUF(ZH_POWERON),          (u32 *)RES_LEN(ZH_POWERON)},
#endif
#if WARNING_POWER_OFF
    [RES_IDX_POWEROFF       ] = {(u32 *)RES_BUF(ZH_POWEROFF),         (u32 *)RES_LEN(ZH_POWEROFF)},
#endif
#if (LANG_SELECT == LANG_EN_ZH)
    [RES_IDX_LANGUAGE_EN    ] = {(u32 *)RES_BUF(EN_LANGUAGE),         (u32 *)RES_LEN(EN_LANGUAGE)},
    [RES_IDX_LANGUAGE_ZH    ] = {(u32 *)RES_BUF(ZH_LANGUAGE),         (u32 *)RES_LEN(ZH_LANGUAGE)},
#endif
#if WARNING_LOW_BATTERY
    [RES_IDX_LOW_BATTERY    ] = {(u32 *)RES_BUF(ZH_LOW_BATTERY),      (u32 *)RES_LEN(ZH_LOW_BATTERY)},
#endif

#if WARNING_MAX_VOLUME
#if WARING_MAXVOL_TYPE == RES_TYPE_PIANO
    [RES_IDX_MAX_VOL        ] = {(u32 *)&RES_BUF_PIANO(PIANO_MAX_VOL),  (u32 *)&RES_LEN_PIANO(PIANO_MAX_VOL)},
#elif WARING_MAXVOL_TYPE == RES_TYPE_TONE
    [RES_IDX_MAX_VOL        ] = {(u32 *)&RES_BUF_TONE(TONE_MAX_VOL),    (u32 *)&RES_LEN_TONE(TONE_MAX_VOL)},
//#elif WARING_MAXVOL_TYPE == RES_TYPE_WAV
#elif WARING_MAXVOL_TYPE == RES_TYPE_MP3
    [RES_IDX_MAX_VOL        ] = {(u32 *)RES_BUF_MP3(EN_MAX_VOL),      (u32 *)RES_LEN_MP3(EN_MAX_VOL)},
#elif WARING_MAXVOL_TYPE == RES_TYPE_WSBC
    [RES_IDX_MAX_VOL        ] = {(u32 *)RES_BUF_SBC(EN_MAX_VOL),      (u32 *)RES_LEN_SBC(EN_MAX_VOL)},
#else
    [RES_IDX_MAX_VOL        ] = {(u32 *)RES_BUF(ZH_MAX_VOL),          (u32 *)RES_LEN(ZH_MAX_VOL)},
#endif
#endif

#if WARNING_USB_SD
    [RES_IDX_SDCARD_MODE    ] = {(u32 *)RES_BUF(ZH_SDCARD_MODE),      (u32 *)RES_LEN(ZH_SDCARD_MODE)},
    [RES_IDX_USB_MODE       ] = {(u32 *)RES_BUF(ZH_USB_MODE),         (u32 *)RES_LEN(ZH_USB_MODE)},
#endif
#if WARNING_FUNC_AUX
    [RES_IDX_AUX_MODE       ] = {(u32 *)RES_BUF(ZH_AUX_MODE),         (u32 *)RES_LEN(ZH_AUX_MODE)},
#endif
#if WARNING_FUNC_CLOCK
    [RES_IDX_CLOCK_MODE     ] = {(u32 *)RES_BUF(ZH_CLOCK_MODE),       (u32 *)RES_LEN(ZH_CLOCK_MODE)},
#endif
#if WARNING_FUNC_FMRX
    [RES_IDX_FM_MODE        ] = {(u32 *)RES_BUF(ZH_FM_MODE),          (u32 *)RES_LEN(ZH_FM_MODE)},
#endif
#if WARNING_FUNC_SPEAKER
    [RES_IDX_SPK_MODE       ] = {(u32 *)RES_BUF(ZH_SPK_MODE),         (u32 *)RES_LEN(ZH_SPK_MODE)},
#endif
#if WARNING_FUNC_USBDEV
    [RES_IDX_PC_MODE        ] = {(u32 *)RES_BUF(ZH_PC_MODE),          (u32 *)RES_LEN(ZH_PC_MODE)},
#endif
#if WARNING_FUNC_BT
    [RES_IDX_BT_MODE        ] = {(u32 *)RES_BUF(ZH_BT_MODE),          (u32 *)RES_LEN(ZH_BT_MODE)},
#endif
#if WARNING_LEFT_RIGHT_CH
    [RES_IDX_LEFT_CH        ] = {(u32 *)RES_BUF(ZH_LEFT_CH),          (u32 *)RES_LEN(ZH_LEFT_CH)},
    [RES_IDX_RIGHT_CH       ] = {(u32 *)RES_BUF(ZH_RIGHT_CH),         (u32 *)RES_LEN(ZH_RIGHT_CH)},
#endif
#if WARNING_BT_PAIR
    [RES_IDX_PAIRING        ] = {(u32 *)RES_BUF(ZH_PAIRING),          (u32 *)RES_LEN(ZH_PAIRING)},
#endif
#if WARNING_BT_WAIT_CONNECT
    [RES_IDX_WAIT4CONN      ] = {(u32 *)RES_BUF(ZH_WAIT4CONN),        (u32 *)RES_LEN(ZH_WAIT4CONN)},
#endif
#if WARNING_BT_CONNECT
    [RES_IDX_CONNECTED      ] = {(u32 *)RES_BUF(ZH_CONNECTED),        (u32 *)RES_LEN(ZH_CONNECTED)},
#endif
#if WARNING_BT_DISCONNECT
    [RES_IDX_DISCONNECT     ] = {(u32 *)RES_BUF(ZH_DISCONNECT),       (u32 *)RES_LEN(ZH_DISCONNECT)},
#endif
#if WARNING_BT_RING_NUMBER
    [RES_IDX_NUM_0          ] = {(u32 *)RES_BUF(ZH_NUM_0),            (u32 *)RES_LEN(ZH_NUM_0)},
    [RES_IDX_NUM_1          ] = {(u32 *)RES_BUF(ZH_NUM_1),            (u32 *)RES_LEN(ZH_NUM_1)},
    [RES_IDX_NUM_2          ] = {(u32 *)RES_BUF(ZH_NUM_2),            (u32 *)RES_LEN(ZH_NUM_2)},
    [RES_IDX_NUM_3          ] = {(u32 *)RES_BUF(ZH_NUM_3),            (u32 *)RES_LEN(ZH_NUM_3)},
    [RES_IDX_NUM_4          ] = {(u32 *)RES_BUF(ZH_NUM_4),            (u32 *)RES_LEN(ZH_NUM_4)},
    [RES_IDX_NUM_5          ] = {(u32 *)RES_BUF(ZH_NUM_5),            (u32 *)RES_LEN(ZH_NUM_5)},
    [RES_IDX_NUM_6          ] = {(u32 *)RES_BUF(ZH_NUM_6),            (u32 *)RES_LEN(ZH_NUM_6)},
    [RES_IDX_NUM_7          ] = {(u32 *)RES_BUF(ZH_NUM_7),            (u32 *)RES_LEN(ZH_NUM_7)},
    [RES_IDX_NUM_8          ] = {(u32 *)RES_BUF(ZH_NUM_8),            (u32 *)RES_LEN(ZH_NUM_8)},
    [RES_IDX_NUM_9          ] = {(u32 *)RES_BUF(ZH_NUM_9),            (u32 *)RES_LEN(ZH_NUM_9)},
#endif
#if WARNING_BT_LOW_LATENCY
    [RES_IDX_MUSIC_MODE     ] = {(u32 *)RES_BUF(ZH_MUSIC_MODE),       (u32 *)RES_LEN(ZH_MUSIC_MODE)},
    [RES_IDX_GAME_MODE      ] = {(u32 *)RES_BUF(ZH_GAME_MODE),        (u32 *)RES_LEN(ZH_GAME_MODE)},
#endif
#if WARNING_BT_CALL_CTRL
    [RES_IDX_CALL_HANGUP    ] = {(u32 *)RES_BUF(ZH_CALL_HANGUP),      (u32 *)RES_LEN(ZH_CALL_HANGUP)},
    [RES_IDX_CALL_REJECT    ] = {(u32 *)RES_BUF(ZH_CALL_REJECT),      (u32 *)RES_LEN(ZH_CALL_REJECT)},
    [RES_IDX_REDIALING      ] = {(u32 *)RES_BUF(ZH_REDIALING),        (u32 *)RES_LEN(ZH_REDIALING)},
#endif
#if WARNING_BT_HID_MENU
    [RES_IDX_CAMERA_ON      ] = {(u32 *)RES_BUF(ZH_CAMERA_ON),        (u32 *)RES_LEN(ZH_CAMERA_ON)},
    [RES_IDX_CAMERA_OFF     ] = {(u32 *)RES_BUF(ZH_CAMERA_OFF),       (u32 *)RES_LEN(ZH_CAMERA_OFF)},
#endif
    [RES_IDX_FINDMY_SOUND   ] = {(u32 *)&RES_BUF_PIANO(PIANO_FINEMY_SOUND), (u32 *)&RES_LEN_PIANO(PIANO_FINEMY_SOUND)},
    [RES_IDX_RING           ] = {(u32 *)RES_BUF_MP3(RING),            (u32 *)RES_LEN_MP3(RING)},
#if WARNING_TAKE_PHOTO
    [RES_IDX_TAKE_PHOTO     ] = {(u32 *)RES_BUF_MP3(TAKE_PHOTO),      (u32 *)RES_LEN_MP3(TAKE_PHOTO)},
#endif
    [RES_IDX_UPDATE         ] = {(u32 *)RES_BUF_MP3(UPDATE),          (u32 *)RES_LEN_MP3(UPDATE)},
    [RES_IDX_UPDATE_DONE    ] = {(u32 *)RES_BUF_MP3(UPDATE_DONE),     (u32 *)RES_LEN_MP3(UPDATE_DONE)},
#if WARNING_BT_CONNECT
    [RES_IDX_TWS_CONNECTED  ] = {(u32 *)RES_BUF(ZH_CONNECTED),        (u32 *)RES_LEN(ZH_CONNECTED)},
#endif
#if ANC_EN
    [RES_IDX_ANC            ] = {(u32 *)RES_BUF_WAV(ANC_ON),          (u32 *)RES_LEN_WAV(ANC_ON)},
    [RES_IDX_NR_DISABLE     ] = {(u32 *)RES_BUF_WAV(NR_DISABLE),      (u32 *)RES_LEN_WAV(NR_DISABLE)},
    [RES_IDX_TRANSPARENCY   ] = {(u32 *)RES_BUF_WAV(TRANSPARENCY),    (u32 *)RES_LEN_WAV(TRANSPARENCY)},
#endif
#if ANC_ALG_AEM_RT_FF_FB_EN
    [RES_IDX_AEM_RT_ON      ] = {(u32 *)RES_BUF_WAV(AEM_ON),          (u32 *)RES_LEN_WAV(AEM_ON)},
#endif
};
#endif

#if LANG_SELECT == LANG_EN_ZH
void multi_lang_init(uint lang_id)
{
}
#endif
