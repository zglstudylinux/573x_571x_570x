#include "include.h"
#include "toolkit_effect.h"

/*
 * 文件名称: effect_adjust_by_res.c
 * 功能描述: 使用上位机修改音效相关处理
 ****************************************************************************************
    code : 待测
    buf  : 待测
    time : 待测
设计流程: 上位机工具会根据链路一键生成 effect_info[]
          根据收到的数据比对 effect_info[] 中的EFFECT_MODE_IDX，匹配则进入callback中进行相应的处理，否则返回

 */

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#define TRACE_R32(...)            print_r32(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#define TRACE_R32(...)
#endif // TRACE_EN


#if EFFECT_DBG_ADJUST_EN

void local_mic_eq_effect_update_callback(u8 *buf, u32 len, u8 params)
{
#if WIRELESS_MIC_EQ_DRC_EN
    tool_eq_coef_t *tool_eq_coef = (tool_eq_coef_t *)(buf + 14);
    if(params == 1) {//update
        if (wireless_role_is_adapter()) {
            toolkit_ack_fail();
            return;
        }
        loc_mic_pacc_set_eq_by_param(tool_eq_coef->max_bandnum, (const u32 *)&tool_eq_coef->gain);
    }
#else
    toolkit_ack_fail();
#endif
}

void local_mic_drc_effect_update_callback(u8 *buf, u32 len, u8 params)
{
#if WIRELESS_MIC_EQ_DRC_EN
    tool_drc_coef_t *tool_drc_coef = (tool_drc_coef_t *)(buf + 14);
    if(params == 1) {//update
        if (wireless_role_is_adapter()) {
            toolkit_ack_fail();
            return;
        }
        loc_mic_pacc_set_drc_by_param(tool_drc_coef->coeff_cnt, (const u32 *)&tool_drc_coef->in_attack_coeff);
    }
#else
    toolkit_ack_fail();
#endif
}

void mix_mic_eq_effect_update_callback(u8 *buf, u32 len, u8 params)
{
#if ADAPTER_MIX_DRC_EN
    tool_eq_coef_t *tool_eq_coef = (tool_eq_coef_t *)(buf + 14);
    if(params == 1 && wireless_get_status()) {//update
        if (!wireless_role_is_adapter()) {
            toolkit_ack_fail();
            return;
        }
        mix_pacc_set_eq_by_param(tool_eq_coef->max_bandnum, (const u32 *)&tool_eq_coef->gain);
    }
#else
    toolkit_ack_fail();
#endif //
}

void mix_mic_drc_effect_update_callback(u8 *buf, u32 len, u8 params)
{
#if ADAPTER_MIX_DRC_EN
    tool_drc_coef_t *tool_drc_coef = (tool_drc_coef_t *)(buf + 14);

    if(params == 1 && wireless_get_status()) {//update
        if (!wireless_role_is_adapter()) {
            toolkit_ack_fail();
            return;
        }
        mix_pacc_set_drc_by_param(tool_drc_coef->coeff_cnt, (const u32 *)&tool_drc_coef->in_attack_coeff);
    }
#else
    toolkit_ack_fail();
#endif
}

#if BT_SCO_EQ_DRC_EN
void msbc_eq_effect_update_callback(u8 *buf, u32 len, u8 params)
{
    tool_eq_coef_t *tool_eq_coef = (tool_eq_coef_t *)(buf + 14);
    if(bt_sco_is_msbc()) {

    } else {
        TRACE("ERROR !! NO MATCH\n");
        toolkit_ack_fail();
        return;
    }
    if(params == 1) {//update
        sco_pacc_set_eq_by_param(tool_eq_coef->max_bandnum, (const u32 *)&tool_eq_coef->gain);
    }
}
void msbc_drc_effect_update_callback(u8 *buf, u32 len, u8 params)
{
    tool_drc_coef_t *tool_drc_coef = (tool_drc_coef_t *)(buf + 14);
    if(bt_sco_is_msbc()) {

    } else {
        TRACE("ERROR !! NO MATCH\n");
        toolkit_ack_fail();
        return;
    }
    if(params == 1) {//update
        sco_pacc_set_drc_by_param(tool_drc_coef->coeff_cnt, (const u32 *)&tool_drc_coef->in_attack_coeff);
    }
}
void csvd_eq_effect_update_callback(u8 *buf, u32 len, u8 params)
{
    tool_eq_coef_t *tool_eq_coef = (tool_eq_coef_t *)(buf + 14);
    if(bt_sco_is_msbc()) {
        TRACE("ERROR !! NO MATCH\n");
        toolkit_ack_fail();
        return;
    } else {

    }
    if(params == 1) {//update
        sco_pacc_set_eq_by_param(tool_eq_coef->max_bandnum, (const u32 *)&tool_eq_coef->gain);
    }
}
void csvd_drc_effect_update_callback(u8 *buf, u32 len, u8 params)
{
    tool_drc_coef_t *tool_drc_coef = (tool_drc_coef_t *)(buf + 14);
    if(bt_sco_is_msbc()) {
        TRACE("ERROR !! NO MATCH\n");
        toolkit_ack_fail();
        return;
    } else {

    }
    if(params == 1) {//update
        sco_pacc_set_drc_by_param(tool_drc_coef->coeff_cnt, (const u32 *)&tool_drc_coef->in_attack_coeff);
    }
}
#else
void msbc_eq_effect_update_callback(u8 *buf, u32 len, u8 params){}
void msbc_drc_effect_update_callback(u8 *buf, u32 len, u8 params){}
void csvd_eq_effect_update_callback(u8 *buf, u32 len, u8 params){}
void csvd_drc_effect_update_callback(u8 *buf, u32 len, u8 params){}
#endif
#endif
