#ifndef __LOAD_CODE_H_
#define __LOAD_CODE_H_


enum {
    LOCK_IDX_FOTA       = 0,
    LOCK_IDX_CHARGE,
    LOCK_IDX_SLEEP,
    LOCK_IDX_PWRSAVE,
    LOCK_IDX_SPIFLASH,
    LOCK_IDX_HUART,

    LOCK_IDX_A2DP,
    LOCK_IDX_SCO,
    LOCK_IDX_BT_SNIFF,
    LOCK_IDX_BB_TEST,
    LOCK_IDX_STREAM,

    //DEC
    LOCK_IDX_MP3DEC,
    LOCK_IDX_WAVDEC,
    LOCK_IDX_SBCDEC,
    LOCK_IDX_AACDEC,

    LOCK_IDX_OPUS,
#if FUNC_RECORD_EN
    LOCK_IDX_RECORD,
#endif
#if FUNC_USBDEV_EN
    LOCK_IDX_USBDEV,
#endif
    LOCK_IDX_WL,
    LOCK_IDX_MAX,
};

void lock_init(void);
bool lock_code(u8 idx);
bool unlock_code(u8 idx);
void unlock_code_without(u8 idx);

void unlock_dcode(void);
void unlock_code_btdec(void);
void lock_code_bt_voice(void);
void unlock_code_bt_voice(void);
void lock_code_pwrsave(void);
void unlock_code_pwrsave(void);
void lock_code_charge(void);
void unlock_code_charge(void);

void load_code_effect_comm(void);
void load_anc_tool_code(void);

#endif // __LOAD_CODE_H_
