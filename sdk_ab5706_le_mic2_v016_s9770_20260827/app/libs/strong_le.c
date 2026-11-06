/**********************************************************************
*
*   strong_le.c
*   定义库里面le部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


void strong_le_include(void) {}

#if !LE_PAIR_EN
#if (LE_ADDRESS_TYPE == GAP_RANDOM_ADDRESS_TYPE_OFF)
void le_sm_init(void) {}
void gap_random_address_set(void) {}
void gap_random_address_set_mode(void) {}
void le_device_db_init(void){}
#endif
void sm_just_works_confirm(void) {}
void sm_request_pairing(void) {}
void sm_send_security_request(void) {}
int sm_authorization_state(void) { return 0; }
int sm_authenticated(void) { return 0; }
int sm_encryption_key_size(void) { return 0; }
void sm_add_event_handler(void) {}
void sm_cmac_signed_write_start(void){ }
int sm_cmac_ready(void) { return 0; }
uint16_t sm_set_setup_context(uint8_t *ptr){ return 0;}
uint16_t sm_get_setup_context(uint8_t *ptr){ return 0;}
uint16_t tws_get_sm_db_info(uint8_t *ptr){ return 0;}
uint16_t tws_set_sm_db_info(uint8_t *ptr, uint16_t len){ return 0;}
#endif

///llcp
void ll_con_move_cb(uint16_t id) {}
void ll_ping_restart(uint8_t id) {}
uint8_t ll_ping_set(uint8_t id, uint16_t au_to) {return 0;}

#if WIRELESS_EN && !ADAPTER_SAVE_PARAM_EN
int wl_hci_le_con_upd_cmd_func(uint8_t link_id, void *param, uint16_t opcode) {return 0;}
uint8_t ll_con_update_ind_func(uint8_t link_id, void *param, uint16_t opcode) {return 0;}
int ll_op_con_upd_ind_func(uint8_t aid, uint *ret, uint8_t rid, uint8_t index) {return 0;}
int hci_le_con_upd_cmd_func(uint8_t link_id, void *param, uint16_t opcode) {return 0;}
int ble_con_param_upd_cfm_func(uint8_t aid, uint *ret, uint8_t rid, uint8_t index) {return 0;}
int ble_con_offset_upd_ind_func(uint8_t aid, uint *ret, uint8_t rid, uint8_t index) {return 0;}
#endif

#if WIRELESS_CON_VERS == 0
//WEAK
uint8_t ble_con_channel_get(uint8_t link, uint16_t evt_cnt) {return 0;}

//STRONG
void ble_con_acl_tx_done_cb_v1(uint8_t link, uint8_t tx_idx);
void ble_con_acl_rx_done_cb_v1(uint8_t link, uint8_t rx_idx);
void ble_con_evt_start_do_cb_v1(uint8_t link);
void ble_con_end_cb_v1(uint8_t link);
void ble_con_established_cb_v1(uint8_t link);

AT(.com_text.ble.isr.con)
void ble_con_acl_tx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_tx_done_cb_v1(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_acl_rx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_rx_done_cb_v1(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_evt_start_do_cb(uint8_t link)
{
    ble_con_evt_start_do_cb_v1(link);
}

void ble_con_established_cb(uint8_t link)
{
    ble_con_established_cb_v1(link);
}

AT(.com_text.ble.isr.con)
void ble_con_end_cb(uint8_t link)
{
    ble_con_end_cb_v1(link);
}

AT(.com_text.ble.isr.con)
void ble_audio_ctr_cb(uint8_t enable)
{
}
#else
//WEAK
void ble_con_acl_tx_done_cb_v1(uint8_t link, uint8_t tx_idx) {}
void ble_con_acl_rx_done_cb_v1(uint8_t link, uint8_t rx_idx) {}
void ble_con_evt_start_do_cb_v1(uint8_t link) {}
void ble_con_established_cb_v1(uint8_t link) {}
#endif

#if WIRELESS_CON_VERS == 2
void ble_con_acl_tx_done(uint8_t link, uint8_t tx_idx);
void ble_con_acl_rx_done(uint8_t link, uint8_t rx_idx);
void ble_con_evt_start_do(uint8_t link);
void ble_con_end_cb_v3(uint8_t link);
void ble_t2r_v3_anchor_ts_update(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt);
void ble_t2r_v3_init(uint8_t init_type);
uint8_t ble_t2r_v3_start(uint16_t con_handle, uint8_t test_rx);
uint8_t ble_t2r_v3_stop(uint16_t con_handle);
void ble_t2r_v3_link_all_ctr(uint8_t enable);
uint16_t ble_t2r_v3_link_duration_get(void);
void ble_t2r_v3_pwr_ctr(uint8_t index, uint8_t pwr_level);
void ble_t2r_v3_skip_first_set_enable(uint8_t index, bool enable);

AT(.com_text.ble.isr.con)
void ble_con_acl_tx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_tx_done(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_acl_rx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_rx_done(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_evt_start_do_cb(uint8_t link)
{
    ble_con_evt_start_do(link);
}

void ble_con_established_cb(uint8_t link)
{
//    ble_con_established_cb_v3(link);
}

AT(.com_text.ble.isr.con)
void ble_con_end_cb(uint8_t link)
{
    ble_con_end_cb_v3(link);
}

AT(.com_text.ble.isr.con)
void ble_con_anchor_ts_update_cb(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt)
{
    ble_t2r_v3_anchor_ts_update(link, anchor_ts, anchor_hus, evt_cnt);
}

void ble_audio_init_cb(uint8_t init_type)
{
    return ble_t2r_v3_init(init_type);
}

uint8_t ble_audio_start_cb(uint16_t con_handle, uint8_t test_rx)
{
    return ble_t2r_v3_start(con_handle, test_rx);
}

uint8_t ble_audio_stop_cb(uint16_t con_handle)
{
    return ble_t2r_v3_stop(con_handle);
}

AT(.com_text.ble.isr.con)
void ble_audio_ctr_cb(uint8_t enable)
{
    ble_t2r_v3_link_all_ctr(enable);
}

uint16_t ble_single_link_duration_get(void)
{
    return ble_t2r_v3_link_duration_get();
}

void ble_audio_pwr_ctr_set(uint8_t idx, uint8_t pwr_level)
{
    printf("ble_audio_pwr_ctr_set, %d, %d\n", idx, pwr_level);
    ble_t2r_v3_pwr_ctr(idx, pwr_level);
}
void wl_skip_first_set(uint8_t index, uint8_t enable)
{
    ble_t2r_v3_skip_first_set_enable(index, enable);
}
#else
void ble_t2r_v3_anchor_ts_update(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt) {}
void ble_t2r_v3_init(uint8_t init_type) {}
uint8_t ble_t2r_v3_start(uint16_t con_handle, uint8_t test_rx) {return 0;}
uint8_t ble_t2r_v3_stop(uint16_t con_handle) {return 0;}
#endif

#if WIRELESS_CON_VERS == 3
void ble_con_acl_tx_done(uint8_t link, uint8_t tx_idx);
void ble_con_acl_rx_done(uint8_t link, uint8_t rx_idx);
void ble_con_evt_start_do(uint8_t link);
void ble_con_end_cb_v4(uint8_t link);
void ble_t2r_v4_anchor_ts_update(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt);
void ble_t2r_v4_calc_anchor_ts_diff(uint8_t link);
void ble_t2r_v4_init(uint8_t init_type);
uint8_t ble_t2r_v4_start(uint16_t con_handle, uint8_t test_rx, void *pdu);
uint8_t ble_t2r_v4_stop(uint16_t con_handle);
bool ble_t2r_v4_evt_cnt_is_free(void);
bool ble_t2r_v4_evt_cnt_alloc(uint8_t link, uint16_t instant, uint16_t *evt_cnt_ret);
void ble_t2r_v4_evt_cnt_free(uint8_t link);
void ble_t2r_v4_ch_map_update(uint8_t link, void *map, uint16_t instant);
bool ble_t2r_v4_param_get(uint16_t *sw, void *map);

AT(.com_text.ble.isr.con)
void ble_con_acl_tx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_tx_done(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_acl_rx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_rx_done(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_evt_start_do_cb(uint8_t link)
{
    ble_con_evt_start_do(link);
}

AT(.com_text.ble.isr.con)
void ble_con_end_cb(uint8_t link)
{
    ble_con_end_cb_v4(link);
}

void ble_con_established_cb(uint8_t link)
{
//    ble_con_established_cb_v4(link);
}

AT(.com_text.ble.isr.con)
void ble_con_anchor_ts_update_cb(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt)
{
    ble_t2r_v4_anchor_ts_update(link, anchor_ts, anchor_hus, evt_cnt);
}

AT(.com_text.ble.isr.con)
void ble_con_calc_anchor_ts_diff_cb(uint8_t link)
{
    ble_t2r_v4_calc_anchor_ts_diff(link);
}

void ble_con_send_audio_setup(uint8_t index, uint8_t test_rx)
{
    void ll_tis_req(uint8_t index, uint8_t test_rx);
    ll_tis_req(index, test_rx);
}

void ble_audio_init_cb(uint8_t init_type)
{
    return ble_t2r_v4_init(init_type);
}

uint8_t ble_audio_start_cb(uint16_t con_handle, uint8_t test_rx, void *param)
{
    return ble_t2r_v4_start(con_handle, test_rx, param);
}

uint8_t ble_audio_stop_cb(uint16_t con_handle)
{
    return ble_t2r_v4_stop(con_handle);
}

AT(.com_text.ble.isr.con)
void ble_audio_ctr_cb(uint8_t enable)
{
}

bool ble_tis_req_param_get(uint16_t *sw, void *map)
{
    return ble_t2r_v4_param_get(sw, map);
}

AT(.text.ble.chmap)
void ble_tis_ch_map_update(uint8_t link, void *map, uint16_t instant)
{
    ble_t2r_v4_ch_map_update(link, map, instant);
}

bool ble_tis_evt_cnt_is_free(void)
{
    return ble_t2r_v4_evt_cnt_is_free();
}

bool ble_tis_evt_cnt_alloc(uint8_t link, uint16_t instant, uint16_t *evt_cnt_ret)
{
    return ble_t2r_v4_evt_cnt_alloc(link, instant, evt_cnt_ret);
}

void ble_tis_evt_cnt_free(uint8_t link)
{
    ble_t2r_v4_evt_cnt_free(link);
}

void ble_audio_pwr_ctr_set(uint8_t idx, uint8_t pwr_level)
{

}
#else
void ble_t2r_v4_anchor_ts_update(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt) {}
void ble_t2r_v4_calc_anchor_ts_diff(uint8_t link) {}
void ble_t2r_v4_init(uint8_t init_type) {}
uint8_t ble_t2r_v4_start(uint16_t con_handle, uint8_t test_rx, void *param) {return 0;}
uint8_t ble_t2r_v4_stop(uint16_t con_handle) {return 0;}
void mgr_ch_class_set_bad(void* ch_map) {}
void mgr_ch_class_update(void) {}
int mgr_ch_class_update_ind_func(uint16_t msg, void *param, uint16_t par0, uint16_t par1) {return 0;}
#endif

#if (WIRELESS_CON_VERS == 5 || WIRELESS_CON_VERS == 6)
void ble_con_acl_tx_done(uint8_t link, uint8_t tx_idx);
void ble_con_acl_rx_done(uint8_t link, uint8_t rx_idx);
void ble_t2r_v6_init(uint8_t init_type);
uint8_t ble_t2r_v6_start(uint16_t con_handle, bool test_rx);
uint8_t ble_t2r_v6_stop(uint16_t con_handle);
void ble_con_end_cb_v6(uint8_t link);
void ble_con_evt_start_do(uint8_t link);
void ble_t2r_v6_anchor_ts_update(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt);
uint16_t ble_t2r_v6_link_duration_get(void);
void ble_t2r_v6_pwr_ctr(uint8_t index, uint8_t pwr_level);

AT(.com_text.ble.isr.con)
void ble_con_acl_tx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_tx_done(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_acl_rx_done_cb(uint8_t link, uint8_t rx_idx)
{
    ble_con_acl_rx_done(link, rx_idx);
}

AT(.com_text.ble.isr.con)
void ble_con_evt_start_do_cb(uint8_t link)
{
    ble_con_evt_start_do(link);
}

AT(.com_text.ble.isr.con)
void ble_con_end_cb(uint8_t link)
{
    ble_con_end_cb_v6(link);
}

void ble_con_established_cb(uint8_t link)
{
}

AT(.com_text.ble.isr.con)
void ble_con_anchor_ts_update_cb(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt)
{
    ble_t2r_v6_anchor_ts_update(link, anchor_ts, anchor_hus, evt_cnt);
}

void ble_audio_init_cb(uint8_t init_type)
{
    ble_t2r_v6_init(init_type);
}

uint8_t ble_audio_start_cb(uint16_t con_handle, uint8_t test_rx, void *param)
{
    return ble_t2r_v6_start(con_handle, test_rx);
}

uint8_t ble_audio_stop_cb(uint16_t con_handle)
{
    return ble_t2r_v6_stop(con_handle);
}

uint16_t ble_single_link_duration_get(void)
{
    return ble_t2r_v6_link_duration_get();
}

void ble_audio_pwr_ctr_set(uint8_t idx, uint8_t pwr_level)
{
    printf("ble_audio_pwr_ctr_set, %d, %d\n", idx, pwr_level);
    ble_t2r_v6_pwr_ctr(idx, pwr_level);
}
#else
void ble_t2r_v6_anchor_ts_update(uint8_t link, uint32_t anchor_ts, uint16_t anchor_hus, uint evt_cnt) {}
void ble_t2r_v6_init(uint8_t init_type) {}
uint8_t ble_t2r_v6_start(uint16_t con_handle, uint8_t test_rx) {return 0;}
uint8_t ble_t2r_v6_stop(uint16_t con_handle) {return 0;}
AT(.com_text.ble.isr.con)
void ble_t2r_v6_hop_accel_isr(void) {}
AT(.com_text.ble.isr.con)
void ble_t2r_v6_ch_map_update(uint8_t link_id, void *p_map) {}
AT(.com_text.ble.isr.con)
bool ble_t2r_v6_need_hold(uint8_t link_id) {return false;}
#endif

#if (WIRELESS_MIC_ROLE == 0)
#elif (WIRELESS_MIC_ROLE == 1)
AT(.com_text.ble.isr.con)
bool ble_scan0_rx_ind(uint8_t scan_id, uint8_t rxdesc_idx) {return 0;}
#else
#endif

#if !ADAPTER_NORMAL_LE_EN
void btstack_init(uint8_t init_type) {}
void hci_recv_packet(uint8_t *buf, int len) {}
#endif

#if WIRELESS_MIC_BROADCAST_EN
void ble_audio_pwr_ctr_set(uint8_t idx, uint8_t pwr_level) {}
uint16_t ble_single_link_duration_get(void) {return 0;}
#else
void ble_ws_per_adv_init(void) {}
bool ble_ws_per_adv_end_ind(void *param) {return false;}
bool ble_ws_per_scan_rx_ind(uint8_t id, uint8_t idx) {return false;}
void ble_ws_per_scan_set_param(uint intv, uint window) {}
void ble_ws_per_scan_init(void) {}
void ble_ws_per_scan_init_hook(void *params, uint8_t act_id) {}
void ble_ws_per_adv_ctrl(uint8_t enable) {}
void ble_ws_per_sync_ctrl(uint8_t enable) {}
void ble_ws_per_scan_ctrl(bool enable) {}
int ble_ws_per_sync_disc_ind_func(uint16_t mid, void *param, uint16_t did, uint16_t sid) {return false;}
int ble_ws_per_sync_user_data_ind_func(uint16_t mid, void *param, uint16_t did, uint16_t sid) {return false;}
int ble_ws_per_adv_tx_req_func(uint16_t mid, void *param, uint16_t did, uint16_t sid) {return false;}
int ble_ws_per_adv_tx_cfm_func(uint16_t mid, void *param, uint16_t did, uint16_t sid) {return false;}
int ble_ws_per_ch_map_tx_ind_func(uint16_t mid, void *param, uint16_t did, uint16_t sid) {return false;}
int ble_ws_per_ch_map_rx_ind_func(uint16_t mid, void *param, uint16_t did, uint16_t sid) {return false;}
void ble_ws_per_ch_map_update(void){};
void ble_ws_per_adv_cmpid_set(uint8_t cmp_id){}
const uint8_t *ble_t2r_broadcast_channel_map_get(void) {return NULL;}
void broadcast_source_ch_map_tx_req(uint8_t index, uint8_t *map) {}
uint8_t ble_t2r_broadcast_stop(uint16_t con_handle){ return 0;}
void ble_t2r_broadcast_init(uint8_t init_type){}
#endif

#if !ADAPTER_UART_COMMAND_EN               //不支持外部LEA芯片共存
AT(.com_text.ble.isr.adv)
void ble_pscan_adv_offset_update(void *evt) {}
AT(.com_text.ret.bt21.isr.adj)
void wireless_con_evt_time_update(void *elt) {}
AT(.com_text.wireless)
void external_iso_anchor_point_get(uint32_t *hs, uint32_t *hus) {}
AT(.com_text.ret.bt21.isr)
void ble_sched_prio_time_adj(void *elt) {}
AT(.com_text.ret.bt21.isr)
uint bb_sch_arb_hook(uint index, void *elt, void *param) {return 0;}
AT(.com_text.ble_pscan)
void wireless_con_offset_record(uint8_t index, bool flag, uint32_t clk_offset, uint32_t bit_offset) {}
AT(.com_text.ble.wireless.iso)
bool wireless_iso_check_overlap(uint8_t index, uint32_t iso_start_clk, uint32_t iso_intv, uint32_t iso_win) {return false;}
#endif


#if WIRELESS_CON_BONDING_EN
bool ble_con_check_bonding_cb(uint8_t *addr, uint8_t addr_type){return true;}
#endif
