/**********************************************************************
*
*   strong_bt.c
*   定义库里面bt部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


#if (!BT_FCC_TEST_EN && !FUNC_BT_FCC_EN) || !LE_EN
uint8_t vs_ble_test(void const *cmd) {
    return 0x11;
}

#if !LE_BQB_RF_EN
uint8_t ble_test_start(void* params) {
    return 0x0c;
}
uint8_t ble_test_stop(void) {
    return 0x0c;
}
#endif
#endif

#if BT_FCC_TEST_EN || FUNC_BT_FCC_EN
#if LE_EN
uint8_t vs_ble_test_do(void const *cmd);
uint8_t vs_ble_test(void const *cmd) {
    return vs_ble_test_do(cmd);
}
#endif
#else
AT(.com_text.weak.bt21.isr.test.fcc)
bool bt_acl_test_rx_end(uint8_t index, void *par) {
    return false;
}
#if !IODM_TEST_EN
uint8_t vs_fcc_test_cmd(void const *param) {
    return 0x11;
}
#endif

#if !LE_BQB_RF_EN
void hci_h3c_init(void) {}
void bt_uart_init(void){}
AT(.com_text.weak.stack.uart_isr)
bool bt_uart_isr(void) {
    return false;
}
#endif
#endif

#if !BT_HID_EN
void hid_device_init(void) {}
int bt_hid_is_connected(void) { return 0;}
void hid_establish_service_level_connection(void* bd_addr) {}
void hid_release_service_level_connection(void* bd_addr) {}
void *get_hid_device_context_for_bd_addr(void *bd_addr) {return NULL;}
void remove_hid_device_context(void * hid_device) {}
void btstack_hid_api(uint param) {}
const void *btstack_hid_tbl[0];
void btstack_hid_send(void) {}
bool bt_hid_send(void *buf, uint len, bool auto_release) { return false;}
bool bt_hid_send_key(uint type, uint keycode) {return false;}
bool bt_hid_is_send_complete(void) { return true;}
void hid_report_set(void *buf, uint len, bool auto_release) {}
bool bsp_bt_hid_vol_change(u16 keycode) {return 0;}
bool bsp_bt_hid_photo(u16 keycode) {return 0;}
void bsp_bt_hid_tog_conn(void) {}
#endif

#if !BT_HID_DOUYIN_EN
void btstack_hid_douyin(uint keycode){}
#endif

#if !BT_HID_EN || !BT_TWS_MS_SWITCH_EN
uint8_t tws_get_hid_service_report(uint8_t *data_ptr) {return 0;}
uint8_t tws_set_hid_service_report(uint8_t *data_ptr, uint8_t len) {return 0;}
uint8_t tws_get_hid_service(uint8_t *data_ptr, uint16_t conhdl){return 0;}
uint8_t tws_set_hid_service(uint8_t *data_ptr, uint16_t conhdl, u8 address[], uint8_t len){return 0;}
#endif


#if !BT_2ACL_EN
void avdtp_ready_switch_play(void) {};
#endif

#if !BT_AVDTP_DYN_LATENCY_EN
void a2dp_set_latency_ms(uint32_t a2dp_latency_ms) {}
void a2dp_latency_check(void) {}
uint16_t a2dp_calc_latency(int8_t rssi, uint16_t rxpkt_ok, uint8_t rxpkt_fail, uint8_t rxpkt_total) {return 0;}
AT(.com_text.weak.sbc.play)
void a2dp_switch_latency(void) {}
#endif

#if !BT_A2DP_EXCEPT_RESTORE_PLAY_EN
void noload_set_play_state(uint8_t play_state){}
uint16_t noload_get_play_state(uint8_t play_state){return 0;}
void noload_clear_play_state(uint8_t play_state){}
void noload_reset_play_state() {}
#endif

WEAK uint8_t bt_get_connected_num(void) {return 0;}

#if !BT_HFP_REC_EN
AT(.com_text.weak.bt_rec)
void bt_sco_rec_mix_do(u8 *buf, u32 samples) {}
void bt_sco_rec_fill_far_buf(u16 *buf, u16 samples) {}
#endif


#if !BT_SPP_EN
void spp_establish_service_level_connection(void *bd_addr) {}
void spp_release_service_level_connection(void *bd_addr) {}
void remove_spp_connection_context(void * spp_connection) {}
void * provide_spp_connection_context_for_bd_addr(void *bd_addr) { return NULL; }
void spp_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {}
void spp_service_flush(void) {}
void * get_spp_connection_context_for_bd_addr(void *bd_addr) { return NULL; }
void spp_init_var(void) {}
#endif

#if !BT_SPP_EN
void spp_txpkt_init(void) {}
AT(.com_text.weak.spp.send_req)
void btstack_spp_send_req(void) {}
int spp_send_packet_channel(void *context, void *buf) {return -1;}
AT(.com_text.weak.spp.event_send)
void spp_event_send(uint16_t cid) {}
#endif

#if !(BT_PBAP_EN || BT_MAP_EN)
void goep_client_init() {}
#endif

#if !BT_MAP_EN
void map_client_init(void) {}
void btstack_map(uint param) {}
void bt_get_time(char *ptr){}
#endif

#if !BT_PBAP_EN
void pbap_client_init(void) {}
void pbap_establish_service_level_connection(void* addr) {}
const void *btstack_pbap_tbl[0];
void btstack_pbap_api(uint param) {}
bool bt_pbap_is_connected(void) {return false;}
#endif

#if !BT_HFP_EN
void hfp_hf_init(void) {}
void hfp_hf_call_switch_for_index(uint8_t index) {}
void hfp_hf_call_setup_other_sync(uint8_t index) {}
void hfp_hf_call_switch(uint8_t flag) {}
bool hfp_hf_get_siri_status(void) {return false;}
void hfp_hf_release_service_level_connection(u8 *bd_addr) {}
void hfp_hf_establish_service_level_connection(u8 *bd_addr) {}
void hfp_hf_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {}
bool hfp_is_connect(uint8_t index) {return false;}
void hfp_hf_switch_siri(uint8_t index) {}
void hfp_hf_answer_incoming_call(uint8_t index) {}
void hfp_hf_terminate_call(uint8_t index) {}
void hfp_hf_terminate_held_call(uint8_t index) {}
void hfp_hf_end_active_and_accept_other(uint8_t index) {}
void hfp_hf_call_hold_switch(uint8_t index) {}
void hfp_hf_call_hold(uint8_t index) {}
void hfp_hf_dial_number(void) {}
void hfp_hf_at_cmd(void) {}
void hfp_hf_custom_at_cmd(void) {}
void hfp_hf_redial_last_number(uint8_t index) {}
void hfp_hf_set_speaker_gain(int gain) {}
void hfp_hf_set_microphone_gain(int gain) {}
void hfp_hf_report_bat_state(int bat) {}
void hfp_init(void) {}
void hfp_var_init(void) {}
void *hfp_get_connections(void) {return NULL;}
void *hfp_get_connection_for_index(uint8_t index) {return NULL;}
uint8_t hfp_rfcomm_service_supported(u8 *bd_addr) {return 0;}
void hfp_vol_delay_store(u8 *hfp_connection) {}
#endif

#if !BT_A2DP_EN
void a2dp_init_var(void) {}
bool avdtp_device_media_is_start_by_index(uint8_t index) {return false;}
void a2dp_notice_music_change_dev(void *ptr) {}
uint8_t a2dp_sbc_codec_setconf_verify(void *codec_info_p) {return false;}
uint8_t a2dp_aac_codec_setconf_verify(void *codec_info_p) {return false;}
void a2dp_set_codec_param(void *media_codec) {}
bool a2dp_is_avdtp_start(void) {return false;}
bool a2dp_is_playing(void) {return false;}
bool a2dp_is_playing_do(void *ptr) {return false;}
bool a2dp_is_avctp_ctrl(void *ptr) {return false;}
void a2dp_search_os_capacity(void *ptr) {}
void a2dp_release_service_level_connection(u8 *bd_addr) {}
void a2dp_run_for_release(void *ptr) {}
void a2dp_establish_service_level_connection(u8 *bd_addr) {}
void * provide_a2dp_connection_context_for_bd_addr(u8 *bd_addr) {return NULL;}
void remove_a2dp_connection_context(void *ptr, uint16_t profile) {}
void * get_a2dp_connection_context_for_bd_addr(u8 *bd_addr) {return NULL;}
void * get_a2dp_connection_context_for_handle(u16 handle) {return NULL;}
AT(.com_text.a2dp)
void * get_avctp_connection_context_for_l2cap_cid(uint16_t cid) {return NULL;}
void * get_avdtp_signal_connection_context_for_l2cap_cid(uint16_t cid) {return NULL;}
void * get_avdtp_media_connection_context_for_l2cap_cid(uint16_t cid) {return NULL;}
void * a2dp_get_connections_for_index(uint8_t index) {return NULL;}
void * a2dp_get_playing_connection(void) {return NULL;}
AT(.com_text.sbc)
void * a2dp_get_connections(void) {return NULL;}
void avdtp_play_pause_check(void) {}
void avdtp_play_pause(void *ptr, bool is_play) {}
void avdtp_send_delay_report(bool force) {}
uint avdtp_fill_get_offset(u8 *ptr, uint len) {return false;}
AT(.btdec.a2dp.rx)
uint8_t avdtp_fill_sbc_buffer(u8 *ptr, uint len) {return false;}
void avdtp_fill_reset(void) {}
AT(.com_text.bb.btisr)
uint8_t avdtp_rx_media_parse(uint8_t *ptr, uint16_t len) {return false;}
void avdtp_clear_sbc(u8 reset) {}
AT(.com_text.bb.btisr)
bool a2dp_media_is_pend(void) {return false;}
AT(.com_text.bb.btisr)
void a2dp_media_fill_cb(void) {}
void avrcp_get_element_attributes_request(void) {}
void avctp_play_pause_for_conn(void *ptr) {}
void avctp_player_ctrl_for_conn(void *ptr, u8 operand) {}
void avctp_volume_change_for_conn(void *ptr) {}
void avctp_vol_delay_store(void *ptr) {}
#endif
