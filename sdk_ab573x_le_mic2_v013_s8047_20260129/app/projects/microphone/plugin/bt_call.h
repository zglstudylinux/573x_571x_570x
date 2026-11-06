#ifndef __BT_CALL
#define __BT_CALL

extern const int mic_gain_tbl[16];
void bt_sco_code_init(void);
void bt_sco_aec_init(u8 *sysclk, aec_cfg_t *aec);
void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_exit(void);
void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr);

void bt_sco_dmdnn_aiaec_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_aiaec_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dmns_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_near_nr_dft_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_near_nr_dft_exit(void);
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_user_init(u8 *sysclk, nr_cb_t *nr);

#if BT_SCO_DMIC_AI_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dmns_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dmns_exit()
#elif BT_SCO_SMIC_AI_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dnn_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dnn_exit()
#elif BT_SCO_AIAEC_DNN_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_aiaec_init(a, b)
    #define bt_sco_near_nr_exit(a)          bt_aiaec_exit()
#elif BT_SCO_DMIC_AIAEC_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dmdnn_aiaec_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dmdnn_aiaec_exit()
#elif BT_SCO_NR_USER_SMIC_EN || BT_SCO_NR_USER_DMIC_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_nr_user_init(a, b)
    #define bt_sco_near_nr_exit()           bt_sco_nr_user_exit()
#else
    #define bt_sco_near_nr_init(a, b)       bt_sco_near_nr_dft_init(a, b)
    #define bt_sco_near_nr_exit()           bt_sco_near_nr_dft_exit()
#endif

#endif
