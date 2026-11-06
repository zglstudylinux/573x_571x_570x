#include "include.h"
#if LE_AB_FOT_EN
//----------------------------------------------------------------------------
//app service
const uint8_t app_primay_uuid16[2]={0x12, 0xff};
static const gatts_uuid_base_st uuid_app_primay_base = {
    .type = GATTS_UUID_TYPE_16BIT,
    .uuid = app_primay_uuid16,
};

const uint8_t app_notify_uuid16[2]={0x14, 0xff};
static const gatts_uuid_base_st uuid_app_notify_base = {
    .props = ATT_NOTIFY,
    .type = GATTS_UUID_TYPE_16BIT,
    .uuid = app_notify_uuid16,
};
static gatts_service_base_st gatts_app_notify_base;

static gatt_characteristic_cb_info_t gatts_app_notify_cb_info = {
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

const uint8_t fota_uuid16[2]={0x15, 0xff};
static const gatts_uuid_base_st uuid_fota_base = {
    .props = ATT_READ|ATT_WRITE_WITHOUT_RESPONSE,
    .type  = GATTS_UUID_TYPE_16BIT,
    .uuid  = fota_uuid16,
};

static int gatt_callback_fota(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    if(fot_app_connect_auth(ptr, len, FOTA_CON_BLE)){
        fot_recv_proc(ptr,len);
    }
    return 0;
}

static gatt_characteristic_cb_info_t gatts_fota_cb_info = {
    .att_callback_func = gatt_callback_fota,
};

bool ab_fot_ble_send_packet(u8 *buf, u8 len)
{
    return ble_tx_notify(gatts_app_notify_base.handle, buf, len);
}

void ab_fot_gatts_service_init(void)
{
    int ret = 0;
    ret = gatts_service_add(GATTS_SRVC_TYPE_PRIMARY,
                                uuid_app_primay_base.uuid,
                                uuid_app_primay_base.type,
                                NULL);            //PRIMARY

    ret = gatts_characteristic_add(uuid_app_notify_base.uuid,
                                       uuid_app_notify_base.type,
                                       uuid_app_notify_base.props,
                                       &gatts_app_notify_base.handle,
                                       &gatts_app_notify_cb_info);      //characteristic

    ret = gatts_characteristic_add(uuid_fota_base.uuid,
                                       uuid_fota_base.type,
                                       uuid_fota_base.props,
                                       NULL,
                                       &gatts_fota_cb_info);      //characteristic

    if(ret != GATTS_SUCCESS){
        printf("ab fot gatt err: %d\n", ret);
        return;
    }
}


//----------------------------------------------------------------------------
//
void ab_fot_gatt_app_init(void)
{
    ab_fot_gatts_service_init();
}
#endif