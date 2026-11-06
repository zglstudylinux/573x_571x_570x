#ifndef _VUSB_TEST_H
#define _VUSB_TEST_H

#define PRODUCT_TEST_BAUD          115200
#define PRODUCT_TEST_START_TO      10          ///单位5ms
#define PRODUCT_SET_CONFIG_TO      200         ///单位5ms

#define PRODUCT_TEST_WL_NAME_LEN   32
#define PRODUCT_TEST_MAX_LEN       sizeof(struct vusb_set_config_tag)
#define PRODUCT_TEST_HEAD          0x4567

enum{
    PRODUCT_TEST_START_REQ,
    PRODUCT_TEST_START_RSP,

    PRODUCT_GET_INFO_REQ,
    PRODUCT_GET_INFO_RSP,

    PRODUCT_SET_CONFIG_REQ,
    PRODUCT_SET_CONFIG_RSP,
};

typedef struct vusb_set_config_tag{
    ///无线麦interval;
    uint16_t tx_intv;
    uint16_t con_intv;
    uint16_t ws_feat;
    uint8_t codec[2];
    ///无线麦频段
    uint8_t freq_band;
    uint8_t retry;
    uint8_t discon_auto_pwroff;
	uint8_t config_rssi;
	uint8_t reserve[3];
    uint8_t wl_name_len;
    uint8_t wl_name[PRODUCT_TEST_WL_NAME_LEN];
} vusb_set_config_t;

typedef struct {
    uint16_t vusb_head;
    uint8_t reserve1;
    uint8_t crc8;
    uint8_t cmd;
    uint8_t len;
    uint8_t reserve2[2];
    uint8_t buf[PRODUCT_TEST_MAX_LEN];
} product_test_cmd_t;

typedef struct {
    product_test_cmd_t huart_test_cmd;
    uint8_t rx_kick_flag;
    uint8_t wl_name_set_flag;
    uint8_t wl_name_len;
    uint8_t wl_name[PRODUCT_TEST_WL_NAME_LEN];
    uint8_t wl_config_sucess_flag;
    uint8_t config_rssi;
    uint8_t vusb_start_test_flag;
} product_test_str_t;

void product_test_init(void);
void product_test_exit(void);
uint8_t product_test_rssi_get(void);
void product_test_rx_process(void);
uint8_t product_test_is_sucess(void);
uint8_t product_test_name_get(u8 *le_name);
void bsp_product_test_process(void);
bool product_test_huart_done(void);

void product_test_huart_init(void);
#endif
