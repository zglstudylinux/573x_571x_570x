#ifndef _WL_XCFG_PARAM_PROC_H
#define _WL_XCFG_PARAM_PROC_H

///跟配置工具一一对应
typedef struct __attribute__((packed))  _wl_xcfg_cb_t {
    u8 wl_save_version;                         //配置版本（暂时只支持0）
    u32 save_voice_param_en              : 1;   //音效参数保存功能
    u32 soft_gain_save_level             : 3;   //soft_gain保存等级
    u32 magic_save_level                 : 3;   //magic保存等级
    u32 echo_save_level                  : 3;   //echo保存等级
    u32 mic_mute_save_level              : 1;   //mic mute保存等级
    u32 soft_gain_replace_en             : 1;   //soft_gain重配置
    u32 soft_gain_max_level              : 3;   //soft_gain最大等级
    u32 soft_gain_default_level          : 3;   //soft_gain默认等级
    u32 soft_gain_level_0;                      //soft_gain等级0
    u32 soft_gain_level_1;                      //soft_gain等级1
    u32 soft_gain_level_2;                      //soft_gain等级2
    u32 soft_gain_level_3;                      //soft_gain等级3
    u32 soft_gain_level_4;                      //soft_gain等级4
    u32 soft_gain_level_5;                      //soft_gain等级5
    u32 soft_gain_level_6;                      //soft_gain等级6
    u32 soft_gain_level_7;                      //soft_gain等级7
    u32 magic_replace_en                 : 1;   //magic表重配置
    u32 magic_default_level              : 3;   //magic默认等级
    s16 magic_level_0;                          //magic等级0
    s16 magic_level_1;                          //magic等级1
    s16 magic_level_2;                          //magic等级2
    s16 magic_level_3;                          //magic等级3
    s16 magic_level_4;                          //magic等级4
    u32 echo_replace_en                  : 1;   //echo重配置
    u32 echo_level                       : 7;   //echo attenuation
    u16 echo_dry_user;                          //echo干度
    u16 echo_wet_user;                          //echo湿度
    u32 echo_max_level                   : 3;   //echo最大等级
    u32 echo_default_level               : 3;   //echo默认等级
    u32 echo_level_0                     : 9;   //echo等级0
    u32 echo_level_1                     : 9;   //echo等级1
    u32 echo_level_2                     : 9;   //echo等级2
    u32 echo_level_3                     : 9;   //echo等级3
    u32 echo_level_4                     : 9;   //echo等级4
    u32 echo_level_5                     : 9;   //echo等级5
    u32 echo_level_6                     : 9;   //echo等级6
    u32 echo_level_7                     : 9;   //echo等级7
    u32 mic_mute_replace_en              : 1;   //mic mute重配置
    u32 mic_mute_default_level           : 1;   //mic mute默认值
    u32 wl_key1_msc_cfg_en               : 1;   //无线麦PP键配置
    u32 wl_key1_msc_ks_sel               : 5;   //PP键短按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key1_msc_kl_sel               : 5;   //PP键长按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key1_msc_kl_up_sel            : 5;   //PP键长按抬起: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key1_msc_kl_hold_sel          : 5;   //PP键长按保持: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key1_msc_kd_sel               : 5;   //PP键双击: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key2_msc_cfg_en               : 1;   //无线麦+键配置
    u32 wl_key2_msc_ks_sel               : 5;   //+键短按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key2_msc_kl_sel               : 5;   //+键长按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key2_msc_kl_up_sel            : 5;   //+键长按抬起: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key2_msc_kl_hold_sel          : 5;   //+键长按保持: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key2_msc_kd_sel               : 5;   //+键双击: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key3_msc_cfg_en               : 1;   //无线麦-键配置
    u32 wl_key3_msc_ks_sel               : 5;   //-键短按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key3_msc_kl_sel               : 5;   //-键长按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key3_msc_kl_up_sel            : 5;   //-键长按抬起: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key3_msc_kl_hold_sel          : 5;   //-键长按保持: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key3_msc_kd_sel               : 5;   //-键双击: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key4_msc_cfg_en               : 1;   //无线麦功能键配置
    u32 wl_key4_msc_ks_sel               : 5;   //功能键短按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key4_msc_kl_sel               : 5;   //功能键长按: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key4_msc_kl_up_sel            : 5;   //功能键长按抬起: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key4_msc_kl_hold_sel          : 5;   //功能键长按保持: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 wl_key4_msc_kd_sel               : 5;   //功能键双击: 无: 0, MUTE/UNMUTE: 1, PWROFF: 2, VOL+: 3, VOL-: 4, ECHO+: 5, ECHO-: 6, MAGIC切换: 7, RM_VOICE切换: 8, MUSIC_PP: 9, MUSIC_NEXT: 10, MUSIC_PREV: 11, MUSIC_VOL+: 12, MUSIC_VOL-: 13, USER+: 14, USER-: 15, USER改变: 16, PWROFF_START: 17, PWROFF_HOLD: 18, PWROFF_RELEASE: 19
    u32 eq_replace_en                    : 1;   //eq参数重配置
    u32 drc_replace_en                   : 1;   //drc参数重配置
    u32 patch_en                         : 1;   //patch使能
    u32 dnr_replace_en                   : 1;   //dnr重配置
    u16 dnr_max_pow;                            //dnr阈值
} wl_xcfg_str_t;

void wl_xcfg_param_tx_proc_init(uint8_t index);
uint8_t *wl_xcfg_param_get(uint8_t index, uint16_t *buf_len);

void wl_xcfg_param_rx_proc(uint8_t *buf, uint8_t len);
void wl_xcfg_param_set(void);
void mic_audio_dnr_max_pow_set(u16 max_pow);

extern const u32 soft_gain_tbl_64[128];

#endif // _WL_XCFG_PARAM_PROC_H
