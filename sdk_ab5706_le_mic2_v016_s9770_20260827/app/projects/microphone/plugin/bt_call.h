#ifndef __BT_CALL
#define __BT_CALL

extern const int mic_gain_tbl[16];
void bt_sco_code_init(void);
void bt_sco_aec_init(u8 *sysclk, aec_cb_t *aec);
void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_exit(void);
void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr);

void bt_sco_ains3_init(u8 *sysclk, nr_cb_t *nr);
void bt_ains3_exit(void);
void bt_sco_ains4_init(u8 *sysclk, nr_cb_t *nr);
void bt_ains4_exit(void);
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_dnn_exit(void);
void bt_sco_nr_user_init(u8 *sysclk, nr_cb_t *nr);


#if BT_SCO_TX_NS_SEL == NS_AINS3
    #define bt_sco_near_nr_init(a, b)       bt_sco_ains3_init(a, b)
    #define bt_sco_near_nr_exit()           bt_ains3_exit()
#elif BT_SCO_TX_NS_SEL == NS_AINS4
    #define bt_sco_near_nr_init(a, b)       bt_sco_ains4_init(a, b)
    #define bt_sco_near_nr_exit()           bt_ains4_exit()
#elif BT_SCO_NR_USER_SMIC_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_nr_user_init(a, b)
    #define bt_sco_near_nr_exit()
#else
    #define bt_sco_near_nr_init(a, b)
    #define bt_sco_near_nr_exit()
#endif

#endif
