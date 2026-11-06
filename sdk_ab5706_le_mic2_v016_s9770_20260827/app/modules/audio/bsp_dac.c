#include "include.h"
#include "bsp_dac.h"

uint8_t cfg_dac_out_spr = 0;
const u8 *dac_dvol_table;

//数字音量DB音量表
AT(.rodata.dac)
const u16 dac_dvol_tbl_db[61] = {
    DIG_N0DB,   DIG_N1DB,   DIG_N2DB,   DIG_N3DB,   DIG_N4DB,   DIG_N5DB,   DIG_N6DB,   DIG_N7DB,
    DIG_N8DB,   DIG_N9DB,   DIG_N10DB,  DIG_N11DB,  DIG_N12DB,  DIG_N13DB,  DIG_N14DB,  DIG_N15DB,
    DIG_N16DB,  DIG_N17DB,  DIG_N18DB,  DIG_N19DB,  DIG_N20DB,  DIG_N21DB,  DIG_N22DB,  DIG_N23DB,
    DIG_N24DB,  DIG_N25DB,  DIG_N26DB,  DIG_N27DB,  DIG_N28DB,  DIG_N29DB,  DIG_N30DB,  DIG_N31DB,
    DIG_N32DB,  DIG_N33DB,  DIG_N34DB,  DIG_N35DB,  DIG_N36DB,  DIG_N37DB,  DIG_N38DB,  DIG_N39DB,
    DIG_N40DB,  DIG_N41DB,  DIG_N42DB,  DIG_N43DB,  DIG_N44DB,  DIG_N45DB,  DIG_N46DB,  DIG_N47DB,
    DIG_N48DB,  DIG_N49DB,  DIG_N50DB,  DIG_N51DB,  DIG_N52DB,  DIG_N53DB,  DIG_N54DB,  DIG_N55DB,
    DIG_N56DB,  DIG_N57DB,  DIG_N58DB,  DIG_N59DB,  DIG_N60DB,
};

//软件数字增益
AT(.rodata.dac)
const int sdadc_soft_gain_tbl_64[64] = {
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
    SOFT_GAIN_P30_0DB, SOFT_GAIN_P30_5DB, SOFT_GAIN_P31_0DB, SOFT_GAIN_P31_5DB
};

AT(.rodata.dac)
const u8 dac_dvol_tbl_16[16 + 1] = {
    60,  43,  32,  26,  24,  22,  20,  18, 16,
    14,  12,  10,  8,   6,   4,   2,   0,
};

AT(.rodata.dac)
const u8 dac_dvol_tbl_32[32 + 1] = {
    60,  50,  43,  38,  35,  30,  28,  26,
    24,  23,  22,  21,  20,  19,  18,  17,
    16,  15,  14,  13,  12,  11,  10,  9,
    8,   7,   6,   5,   4,   3,   2,   1,   0,
};

AT(.text.bsp.dac)
u32 bsp_dac_get_gain_level(u8 vol)
{
    u32 level;
    if (vol > VOL_MAX) {
        vol = VOL_MAX;
    }
    level = dac_dvol_table[vol];
    return 60 - level;
}

AT(.text.bsp.dac)
void bsp_change_volume_db(u8 level)
{
    s32 db_num;
    db_num = 60 - level;
    if (db_num > 60) {
        db_num = 60;
    }
    if (db_num < 0) {
        db_num = 0;
    }

    dac_vol_set(dac_dvol_tbl_db[db_num]);
}

AT(.text.bsp.dac)
void bsp_change_volume_db_l_r(u8 l_level, u8 r_level)
{
    u16 dvol_l = dac_dvol_tbl_db[60 - l_level];
    u16 dvol_r = dac_dvol_tbl_db[60 - r_level];
    dac_balance_set(dvol_l, dvol_r);
}

AT(.text.vol.convert)
u16 bsp_volume_convert(u8 vol)
{
    u16 vol_set = 0;
    u8 level = 0;
    if (vol <= VOL_MAX) {
        level = dac_dvol_table[vol];
        if (level > 60) {
            level = 60;
        }
        vol_set = dac_dvol_tbl_db[level];
    }
    return vol_set;
}

AT(.text.bsp.dac)
void bsp_change_volume(u8 vol)
{
    u8 level = 0;
    if (vol <= VOL_MAX) {
        level = dac_dvol_table[vol];
        if (level > 60) {
            level = 60;
        }

        printf("vol_change: %d, (%d, %d)\n", vol, sys_cb.vol, sys_cb.a2dp_vol);
        dac_vol_set(dac_dvol_tbl_db[level]);
    }
}

AT(.text.bsp.dac)
bool bsp_set_volume(u8 vol)
{
    printf("vol_set: %d\n", sys_cb.vol);

    if (!bsp_res_is_vol_busy()) {       //未播放提示音，修改音量
        bsp_change_volume(vol);
    }
    if (vol == sys_cb.vol) {
        gui_box_show_vol();
        return false;
    }

    if (vol <= VOL_MAX) {
        sys_cb.vol = vol;
        gui_box_show_vol();

        sys_cb.cm_times = 0;
        sys_cb.cm_vol_change = 1;
    }
    return true;
}

AT(.text.bsp.dac)
u8 bsp_volume_inc(u8 vol)
{
    vol++;
    if(vol > VOL_MAX)
        vol = VOL_MAX;
    return vol;
}

AT(.text.bsp.dac)
u8 bsp_volume_dec(u8 vol)
{
    if(vol > 0)
        vol--;
    return vol;
}

//vcmbuf及差分
AT(.text.bsp.dac)
bool bsp_dac_off_for_bt_conn(void)
{
    if ((DAC_OFF_FOR_BT_CONN_EN) && (DAC_CH_SEL >= DAC_VCMBUF_MONO)) {
        return true;
    }
    return false;
}

AT(.text.bsp.dac)
void dac_set_anl_offset(u8 bt_call_flag)
{

}

AT(.text.bsp.dac)
void dac_set_vol_table(u8 vol_max)
{
    if (vol_max == 16) {
        dac_dvol_table = dac_dvol_tbl_16;
    } else {
        dac_dvol_table = dac_dvol_tbl_32;
    }
    dac_set_anl_offset(0);
}

//开机控制DAC电容放电等待时间
AT(.text.dac.pull)
void dac_pull_down_delay(void)
{
    delay_5ms(DAC_PULL_DOWN_DELAY);
}

#if LOUDSPEAKER_MUTE_EN
AT(.com_text.dac.mute)
void dac_set_mute_callback(u8 mute_flag)
{
    if (mute_flag) {
        bsp_loudspeaker_mute();
    } else {
        if (!sys_cb.mute) {
            bsp_loudspeaker_unmute();
        }
    }
}
#endif

AT(.text.bsp.dac)
void dac_init(void)
{
    cfg_dac_out_spr = DAC_OUT_SPR;

    dac_set_vol_table(xcfg_cb.vol_max);
    printf("[%s] vol_max:%d\n", __func__, xcfg_cb.vol_max);

    dac_aubuf_init();

    dac_power_on();

    dac_ang_gain_set(ANL_N0DB);

    plugin_music_eq();

#if DAC_DRC_EN
    music_dac_drc_effect_update_by_num(0);
#endif

#if DAC_DNR_EN
    dac_dnr_init(2, 0x10, 80, 0x10);
#endif

    mic_leave_out_rc_init();
}


