#include "include.h"
#include "wireless.h"

#if WIRELESS_EN && ADAPTER_SAVE_PARAM_EN

#if ADAPTER_EN

AT(.rodata.soft_gain_tbl)
const u32 soft_gain_tbl_64[128] = {
    //负增益
    SOFT_GAIN_N110DB,SOFT_GAIN_N110DB,SOFT_GAIN_N65DB,SOFT_GAIN_N60DB,SOFT_GAIN_N59DB,
    SOFT_GAIN_N58DB,SOFT_GAIN_N57DB,SOFT_GAIN_N56DB,SOFT_GAIN_N55DB,SOFT_GAIN_N54DB,
    SOFT_GAIN_N53DB,SOFT_GAIN_N52DB,SOFT_GAIN_N51DB,SOFT_GAIN_N50DB,SOFT_GAIN_N49DB,
    SOFT_GAIN_N48DB,SOFT_GAIN_N47DB,SOFT_GAIN_N46DB,SOFT_GAIN_N45DB,SOFT_GAIN_N44DB,
    SOFT_GAIN_N43DB,SOFT_GAIN_N42DB,SOFT_GAIN_N41DB,SOFT_GAIN_N40DB,SOFT_GAIN_N39DB,
    SOFT_GAIN_N38DB,SOFT_GAIN_N37DB,SOFT_GAIN_N36DB,SOFT_GAIN_N35DB,SOFT_GAIN_N34DB,
    SOFT_GAIN_N33DB,SOFT_GAIN_N32DB,SOFT_GAIN_N31DB,SOFT_GAIN_N30DB,SOFT_GAIN_N29DB,
    SOFT_GAIN_N28DB,SOFT_GAIN_N27DB,SOFT_GAIN_N26DB,SOFT_GAIN_N25DB,SOFT_GAIN_N24DB,
    SOFT_GAIN_N23DB,SOFT_GAIN_N22DB,SOFT_GAIN_N21DB,SOFT_GAIN_N20DB,SOFT_GAIN_N19DB,
    SOFT_GAIN_N18DB,SOFT_GAIN_N17DB,SOFT_GAIN_N16DB,SOFT_GAIN_N15DB,SOFT_GAIN_N14DB,
    SOFT_GAIN_N13DB,SOFT_GAIN_N12DB,SOFT_GAIN_N11DB,SOFT_GAIN_N10DB,SOFT_GAIN_N9DB,
    SOFT_GAIN_N8DB,SOFT_GAIN_N7DB,SOFT_GAIN_N6DB,SOFT_GAIN_N5DB,SOFT_GAIN_N4DB,
    SOFT_GAIN_N3DB,SOFT_GAIN_N2DB,SOFT_GAIN_N1DB,SOFT_GAIN_N0DB,

    SOFT_GAIN_P0_0DB, SOFT_GAIN_P0_5DB, SOFT_GAIN_P1_0DB, SOFT_GAIN_P1_5DB, SOFT_GAIN_P2_0DB,
    SOFT_GAIN_P2_5DB, SOFT_GAIN_P3_0DB, SOFT_GAIN_P3_5DB, SOFT_GAIN_P4_0DB, SOFT_GAIN_P4_5DB,
    SOFT_GAIN_P5_0DB, SOFT_GAIN_P5_5DB, SOFT_GAIN_P6_0DB, SOFT_GAIN_P6_5DB, SOFT_GAIN_P7_0DB,
    SOFT_GAIN_P7_5DB, SOFT_GAIN_P8_0DB, SOFT_GAIN_P8_5DB, SOFT_GAIN_P9_0DB, SOFT_GAIN_P9_5DB,
    SOFT_GAIN_P10_0DB, SOFT_GAIN_P10_5DB, SOFT_GAIN_P11_0DB, SOFT_GAIN_P11_5DB, SOFT_GAIN_P12_0DB,
    SOFT_GAIN_P12_5DB, SOFT_GAIN_P13_0DB, SOFT_GAIN_P13_5DB, SOFT_GAIN_P14_0DB, SOFT_GAIN_P14_5DB,
    SOFT_GAIN_P15_0DB, SOFT_GAIN_P15_5DB, SOFT_GAIN_P16_0DB, SOFT_GAIN_P16_5DB, SOFT_GAIN_P17_0DB,
    SOFT_GAIN_P17_5DB, SOFT_GAIN_P18_0DB, SOFT_GAIN_P18_5DB, SOFT_GAIN_P19_0DB, SOFT_GAIN_P19_5DB,
    SOFT_GAIN_P20_0DB, SOFT_GAIN_P20_5DB, SOFT_GAIN_P21_0DB, SOFT_GAIN_P21_5DB, SOFT_GAIN_P22_0DB,
    SOFT_GAIN_P22_5DB, SOFT_GAIN_P23_0DB, SOFT_GAIN_P23_5DB, SOFT_GAIN_P24_0DB, SOFT_GAIN_P24_5DB,
    SOFT_GAIN_P25_0DB, SOFT_GAIN_P25_5DB, SOFT_GAIN_P26_0DB, SOFT_GAIN_P26_5DB, SOFT_GAIN_P27_0DB,
    SOFT_GAIN_P27_5DB, SOFT_GAIN_P28_0DB, SOFT_GAIN_P28_5DB, SOFT_GAIN_P29_0DB, SOFT_GAIN_P29_5DB,
    SOFT_GAIN_P30_0DB, SOFT_GAIN_P30_5DB, SOFT_GAIN_P31_0DB, SOFT_GAIN_P31_5DB,
};

void wl_device_save_mic_vol_level(uint8_t *addr, uint8_t mic_vol_level){
    u8 wl_db_index = wl_device_db_index_get_for_addr(addr);

    wl_device_save_voice_param(wl_db_index, MIC_VOL_LEVEL_TYPE, mic_vol_level);
}

void wl_device_save_echo_level(uint8_t *addr, uint8_t echo_level){
    u8 wl_db_index = wl_device_db_index_get_for_addr(addr);

    wl_device_save_voice_param(wl_db_index, ECHO_LEVEL_TYPE, echo_level);
}

void wl_device_save_magic_level(uint8_t *addr, uint8_t magic_level){
    u8 wl_db_index = wl_device_db_index_get_for_addr(addr);

    wl_device_save_voice_param(wl_db_index, MAGIC_LEVEL_TYPE, magic_level);
}

void wl_device_save_mute_level(uint8_t *addr, uint8_t mute_level){
    u8 wl_db_index = wl_device_db_index_get_for_addr(addr);

    wl_device_save_voice_param(wl_db_index, MUTE_LEVEL_TYPE, mute_level);
}

#else

void wl_voice_param_set(wl_xcfg_str_t * wl_xcfg_ptr)
{
    if (wl_xcfg_ptr->soft_gain_replace_en) {
        u8 soft_gain_max_level = wl_xcfg_ptr->soft_gain_max_level+1;
        soft_gain_max_level_set(soft_gain_max_level);
        soft_gain_tbl_set((u8 *)&wl_xcfg_ptr->soft_gain_level_0, soft_gain_max_level*4);
        soft_gain_level_set(wl_xcfg_ptr->soft_gain_default_level);
    }
    if (wl_xcfg_ptr->magic_replace_en) {
        s32 magic_tbl[5] = {wl_xcfg_ptr->magic_level_0, wl_xcfg_ptr->magic_level_1, wl_xcfg_ptr->magic_level_2, wl_xcfg_ptr->magic_level_3, wl_xcfg_ptr->magic_level_4};
        magic_effect_tbl_set((u8 *)magic_tbl, 5*4);
        magic_effect_level_set(wl_xcfg_ptr->magic_default_level);
    }
    if (wl_xcfg_ptr->echo_replace_en) {
        u16 echo_tbl[8] = {wl_xcfg_ptr->echo_level_0, wl_xcfg_ptr->echo_level_1, wl_xcfg_ptr->echo_level_2, wl_xcfg_ptr->echo_level_3, \
                           wl_xcfg_ptr->echo_level_4, wl_xcfg_ptr->echo_level_5, wl_xcfg_ptr->echo_level_6, wl_xcfg_ptr->echo_level_7};
        u8 echo_max_level = wl_xcfg_ptr->echo_max_level+1;
        echo_max_level_set(echo_max_level);
        echo_env_set(wl_xcfg_ptr->echo_level, wl_xcfg_ptr->echo_dry_user, wl_xcfg_ptr->echo_wet_user);
        echo_tbl_set((u8 *)echo_tbl, echo_max_level*2);
        echo_delay_level_set(wl_xcfg_ptr->echo_default_level);
    }

    if (wl_xcfg_ptr->mic_mute_replace_en) {
        mic_enc_mute_en_set(wl_xcfg_ptr->mic_mute_default_level);
    }

    if (wl_xcfg_ptr->save_voice_param_en) {
        soft_gain_level_set(wl_xcfg_ptr->soft_gain_save_level);
        echo_delay_level_set(wl_xcfg_ptr->echo_save_level);
        magic_effect_level_set(wl_xcfg_ptr->magic_save_level);
        mic_enc_mute_en_set(wl_xcfg_ptr->mic_mute_save_level);
    }
}

#endif

#endif
