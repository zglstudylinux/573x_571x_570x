#include "include.h"

#define CAR_TOOL_SEED                       0xffff

const effect_update_callback_t effect_update_callback_tbl[] = {
	[CFG_MIC_EQ]   = {local_mic_eq_effect_update_callback},
	[CFG_MIC_DRC]  = {local_mic_drc_effect_update_callback},
};

const u8 effect_offset_tbl[EFFECT_IDX_MAX_NB] = {
	[EFFECT_IDX_LOC_MIC_EQ]     = CFG_MIC_EQ,
	[EFFECT_IDX_LOC_MIC_DRC]    = CFG_MIC_DRC,
};

u8 effect_info_offset_get(uint effect_idx)
{
    if(effect_idx >= EFFECT_IDX_MAX_NB) {
        return CFG_MAX;
    }

    return effect_offset_tbl[effect_idx];
}

AT(.text.effect_res)
u8 *effect_res_addr_get(uint effect_idx)
{
    u8 res_offset;
    if(effect_idx >= EFFECT_IDX_MAX_NB) {
        return NULL;
    }

    res_offset = effect_offset_tbl[effect_idx];
    if(res_offset >= CFG_MAX) {
        return NULL;
    }

    u32 effect_res_offset = effect_info[res_offset].effect_res_offset;
    u32 *res_inbin_addr = (u32*)((u8*) (RES_BUF_EFFECT_EFFECT_BIN) + effect_res_offset);
    u8 *res_real_addr  = (u8*)RES_BUF_EFFECT_EFFECT_BIN + *res_inbin_addr;

    return res_real_addr;
}

AT(.text.effect_res)
u32 effect_res_len_get(uint effect_idx)
{
    u8 res_offset;
    if(effect_idx >= EFFECT_IDX_MAX_NB) {
        return 0;
    }

    res_offset = effect_offset_tbl[effect_idx];
    if(res_offset >= CFG_MAX) {
        return 0;
    }

    u32 effect_len_offset = effect_info[res_offset].effect_len_offset;
    u32  res_inbin_len  = *(u32*)((u8*) (RES_BUF_EFFECT_EFFECT_BIN) + effect_len_offset);

    return res_inbin_len;
}

bool bsp_set_effect_by_idx(u8 effect_idx)
{
    if (effect_idx > EFFECT_IDX_MAX_NB) {
        return false;
    }

    u32 addr = RES_BUF_EFFECT_EFFECT_BIN;
    u8 cfg_idx = effect_offset_tbl[effect_idx];
    u32* res_inbin_addr = (u32*)((u8*)(addr) + effect_info[cfg_idx].effect_res_offset);
    u32  res_inbin_len = *(u32*)((u8*)(addr) + effect_info[cfg_idx].effect_len_offset);
    u8*  res_real_addr = (u8*)addr + *res_inbin_addr;
    u32 cal_crc = calc_crc(res_real_addr, res_inbin_len-2, CAR_TOOL_SEED);
    u32 res_crc = little_endian_read_16(res_real_addr, res_inbin_len-2);

    if (cal_crc != res_crc) {
        printf("effect.bin %s CRC ERROR %x %x\n", effect_info[cfg_idx].effect_cfg_name, cal_crc, res_crc);
        return false;
    }
    if (effect_update_callback_tbl[cfg_idx].effect_update_callback){
        effect_update_callback_tbl[cfg_idx].effect_update_callback(res_real_addr, res_inbin_len, 0);
        return true;
    }
    return false;
}
