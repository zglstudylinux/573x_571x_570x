#include "include.h"


#define param_read8(a, b)       b = cm_read8(PAGE0(a))
#define param_write8(a, b)      cm_write8(PAGE0(a), b)
#define param_read16(a, b)      b = cm_read16(PAGE0(a))
#define param_write16(a, b)     cm_write16(PAGE0(a), b)
#define param_read32(a, b)      b = cm_read32(PAGE0(a))
#define param_write32(a, b)     cm_write32(PAGE0(a), b)
#define param_read(a, b, c)     cm_read(a, PAGE0(b), c)
#define param_write(a, b, c)    cm_write(a, PAGE0(b), c)
#define param_sync_do()         cm_sync()


AT(.text.bsp.param)
void param_init(bool reset)
{
    //LANG ID初值
    if ((LANG_SELECT < LANG_EN_ZH) && xcfg_cb.lang_id >= LANG_EN_ZH) {
        printf("Language Select Error\n");
        xcfg_cb.lang_id = LANG_SELECT;
    }
    cm_init(MAX_CM_PAGE, CM_START, CM_SIZE);
    //printf("CM: %x\n", cm_read8(PAGE0(0)));
    //printf("CM: %x\n", cm_read8(PAGE1(0)));

    sys_cb.rand_seed = sys_get_rand_key();

    param_lang_id_read();
    if(sys_cb.lang_id >= LANG_EN_ZH) {
        if (xcfg_cb.lang_id == 2) {
            sys_cb.lang_id = 0;             //出厂默认英文
        } else if (xcfg_cb.lang_id == 3) {
            sys_cb.lang_id = 1;             //出厂默认中文
        } else {
            sys_cb.lang_id = xcfg_cb.lang_id;
        }
    }

    param_sys_vol_read();
    if ((SYS_LIMIT_VOLUME == 0) || (sys_cb.vol > VOL_MAX)) {
        sys_cb.vol = SYS_INIT_VOLUME;                   //上电还原到默认音量
    } else {
        if (sys_cb.vol < SYS_LIMIT_VOLUME) {
            sys_cb.vol = SYS_LIMIT_VOLUME;
        }
    }
    sys_cb.a2dp_vol = sys_cb.vol;

    param_hfp_vol_read();
    if (sys_cb.hfp_vol > 15) {
        sys_cb.hfp_vol = 15;
    }

    sys_cb.sw_rst_flag = sw_reset_source_get();
#if WIRELESS_CON_BONDING_EN
    param_read_bonding_addr(bongding_addr_get());
   // my_printf("%s %d\n",__func__,__LINE__);
   // my_print_r(bongding_addr_get(),6);
#endif
}

AT(.text.bsp.param)
void bsp_param_write(u8 *buf, u32 addr, uint len)
{
    param_write(buf, addr, len);
}

AT(.text.bsp.param)
void bsp_param_read(u8 *buf, u32 addr, uint len)
{
    param_read(buf, addr, len);
}

AT(.text.bsp.param)
void bsp_param_sync(void)
{
    param_sync_do();
}


AT(.text.bsp.param)
void param_sys_vol_write(void)
{
    param_write((u8 *)&sys_cb.vol, PARAM_SYS_VOL, 1);
}

AT(.text.bsp.param)
void param_sys_vol_read(void)
{
    param_read((u8 *)&sys_cb.vol, PARAM_SYS_VOL, 1);
}

AT(.text.bsp.param)
void param_hfp_vol_write(void)
{
    param_write((u8 *)&sys_cb.hfp_vol, PARAM_HSF_VOL, 1);
}

AT(.text.bsp.param)
void param_hfp_vol_read(void)
{
    param_read((u8 *)&sys_cb.hfp_vol, PARAM_HSF_VOL, 1);
}

AT(.text.bsp.param)
void param_lang_id_write(void)
{
    param_write((u8 *)&sys_cb.lang_id, PARAM_LANG_ID, 1);
}

AT(.text.bsp.param)
void param_lang_id_read(void)
{
    param_read((u8 *)&sys_cb.lang_id, PARAM_LANG_ID, 1);
}

AT(.text.bsp.param)
void param_sync(void)
{
    param_sync_do();
}

AT(.text.bsp.param)
u32 param_get_xosc_addr(void)
{
    return PARAM_BT_XOSC_CAP;
}

AT(.text.bsp.param.fota)
void param_fot_addr_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_ADDR, 4);
    param_sync();
}

AT(.text.bsp.param.fota)
void param_fot_addr_read(u8 *param)
{
    param_read(param, PARAM_FOT_ADDR, 4);
}

AT(.text.bsp.param.fota)
void param_fot_remote_ver_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_REMOTE_VER, 2);
    param_sync();
}

AT(.text.bsp.param.fota)
void param_fot_remote_ver_read(u8 *param)
{
    param_read(param, PARAM_FOT_REMOTE_VER, 2);
}

AT(.text.bsp.param.fota)
void param_fot_head_info_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_HEAD_INFO, 8);
    param_sync();
}

AT(.text.bsp.param.fota)
void param_fot_head_info_read(u8 *param)
{
    param_read(param, PARAM_FOT_HEAD_INFO, 8);
}

AT(.text.bsp.param.fota)
void param_fot_hash_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_HASH, 4);
    param_sync();
}

AT(.text.bsp.param.fota)
void param_fot_hash_read(u8 *param)
{
    param_read(param, PARAM_FOT_HASH, 4);
}

AT(.text.bsp.param.fota)
void param_fot_type_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_TYPE, 1);
    param_sync();
}

AT(.text.bsp.param.fota)
void param_fot_type_read(u8 *param)
{
    param_read(param, PARAM_FOT_TYPE, 1);
}

AT(.text.bsp.param)
void param_write_bonding_addr(u8 *param)
{
    param_write((u8 *)param, PARAM_BONDING_ADDR, 6);
    param_sync();
}

AT(.text.bsp.param)
void param_read_bonding_addr(u8 *param)
{
    param_read(param, PARAM_BONDING_ADDR, 6);
}

#if WIRELESS_CON_BONDING_EN
AT(.text.bsp.param)
void param_bond_addr_read(u8 num)
{
    if(num) {
        param_read((u8 *)&sys_cb.wl_bd_addr1, PARAM_BONDING_ADDR1, 6);
    } else {
        param_read((u8 *)&sys_cb.wl_bd_addr0, PARAM_BONDING_ADDR0, 6);
    }
}

AT(.text.bsp.param)
void param_bond_addr_write(u8 num)
{
    if(num) {
        param_write((u8 *)&sys_cb.wl_bd_addr1, PARAM_BONDING_ADDR1, 6);
    } else {
        param_write((u8 *)&sys_cb.wl_bd_addr0, PARAM_BONDING_ADDR0, 6);
    }
    param_sync();
}

AT(.text.bsp.param)
void param_bond_nb_read(void)
{
    param_read((u8 *)&sys_cb.wl_bond_nb, PARAM_BONDING_NB, 1);
}

AT(.text.bsp.param)
void param_bond_nb_write(void)
{
    param_write((u8 *)&sys_cb.wl_bond_nb, PARAM_BONDING_NB, 1);
    param_sync();
}
#endif
