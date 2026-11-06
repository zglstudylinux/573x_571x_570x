#ifndef _WIRELESS_PROC_H
#define _WIRELESS_PROC_H


#define WIRELESS_CON_STA_MASK               ((1<<WIRELESS_CON_LINK_NB)-1)

struct wireless_cb_tag {
    uint8_t change_flag;
    uint8_t change_sta[2];

    uint8_t connected_sta;
    uint8_t adv_sta;
    bool alg_en;
    uint8_t bypass_cnt;
#if WIRELESS_CON_PAIR_MODE
    uint8_t con_role_switch_flag;
    u32 con_id_switch_ticks;
    uint8_t con_temp;
#endif
};

extern struct wireless_cb_tag wireless_cb;

void wireless_var_init(void);
void wireless_sta_proc(void);
void wireless_audio_bypass(void);
void wireless_audio_enable(void);
u8 wireless_get_status(void);

void sfunc_adapter_exit(void);
void sfunc_adapter_init(void);

void wireless_cmd_reset(u8 index);

#endif // _WIRELESS_PROC_H
