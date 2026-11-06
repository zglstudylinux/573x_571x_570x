#include "include.h"
#include "wireless.h"

#if ADAPTER_EN && ADAPTER_SAVE_PARAM_EN

#define WL_INVALID_ADDR_TYPE    0xff
#define WL_VALID_ADDR_TYPE      0x01
#define WL_MEMORY_NUM           4
#define WL_MEMORY_SIZE          (sizeof(wl_adapter_save_str))

static wl_adapter_save_str wl_adapter_save[WL_MEMORY_NUM];


static void wl_get_link_info(void *buf, u16 addr, u16 size)
{
//    printf("wl_read: %04x,%04x, %08lx\n", addr, size, WL_CM_PAGE(addr));

    cm_read(buf, WL_CM_PAGE(addr), size);

//    print_r(buf, size);
}

static void wl_put_link_info(void *buf, u16 addr, u16 size)
{
//    printf("wl_write: %04x,%04x, %08lx\n", addr, size, WL_CM_PAGE(addr));
//    print_r(buf, size);

    cm_write(buf, WL_CM_PAGE(addr), size);
}

static void wl_sync_link_info(void)
{
    cm_sync();
}

AT(.text.le.db.write)
static void wl_info_save_for_index(u8 index)
{
    wl_put_link_info(&wl_adapter_save[index], index * WL_MEMORY_SIZE, WL_MEMORY_SIZE);
}

AT(.text.le.db.read)
static void wl_info_load_for_index(u8 index)
{
    wl_get_link_info(&wl_adapter_save[index], index * WL_MEMORY_SIZE, WL_MEMORY_SIZE);
}

AT(.text.le.db.read)
static int wl_addr_is_null(uint8_t * data, int size){
    int i;
    for (i=0; i < size ; i++){
        if (data[i]) return 0;
    }
    return 1;
}

// free device
AT(.text.le.db.remove)
static void wl_device_db_remove(int index){
    wl_adapter_save[index].addr_type = WL_INVALID_ADDR_TYPE;
}

void wl_device_save_voice_param(uint8_t wl_db_index, uint8_t param_type, uint8_t param_value){

    if (wl_db_index < WL_MEMORY_NUM) {
        switch (param_type) {
            case MIC_VOL_LEVEL_TYPE:
                wl_adapter_save[wl_db_index].wl_voice_param.mic_vol_level = param_value;
                break;

            case ECHO_LEVEL_TYPE:
                wl_adapter_save[wl_db_index].wl_voice_param.echo_level = param_value;
                break;

            case MAGIC_LEVEL_TYPE:
                wl_adapter_save[wl_db_index].wl_voice_param.magic_level = param_value;
                break;

            case MUTE_LEVEL_TYPE:
                wl_adapter_save[wl_db_index].wl_voice_param.mute_level = param_value;
                break;
        }

        ///直接保存信息并更新
        wl_info_save_for_index(wl_db_index);
        wl_sync_link_info();
    }
}

AT(.text.le.db)
int wl_device_db_is_exist(uint8_t *addr){
    int i;
    for (i=0;i<WL_MEMORY_NUM;i++){
        if(!memcmp(wl_adapter_save[i].wl_addr, addr, 6) && (wl_adapter_save[i].addr_type == WL_VALID_ADDR_TYPE)){
            return true;
        }
    }

    return false;
}

AT(.text.le.db)
int wl_device_db_index_get_for_addr(uint8_t *addr){
    int i;
    for (i=0;i<WL_MEMORY_NUM;i++){
        if(!memcmp(wl_adapter_save[i].wl_addr, addr, 6) && (wl_adapter_save[i].addr_type == WL_VALID_ADDR_TYPE)){
            return i;
        }
    }

    return (-1);
}

AT(.text.le.db)
wl_adapter_save_str *wl_device_db_get_for_addr(uint8_t *addr){
    int i;
    for (i=0;i<WL_MEMORY_NUM;i++){
        if(!memcmp(wl_adapter_save[i].wl_addr, addr, 6) && (wl_adapter_save[i].addr_type == WL_VALID_ADDR_TYPE)){
            return &wl_adapter_save[i];
        }
    }

    return NULL;
}

AT(.text.le.db)
int wl_device_db_add(uint8_t *addr){
    int i;
    int index = -1;
    for (i=0;i<WL_MEMORY_NUM;i++){
         if (wl_adapter_save[i].addr_type == WL_INVALID_ADDR_TYPE){
            index = i;
            break;
         }
    }

    if (index < 0){
        for(i=0; i < (WL_MEMORY_NUM - 1); i++){
            memcpy(&wl_adapter_save[i], &wl_adapter_save[i + 1], sizeof(wl_adapter_save_str));
            wl_info_save_for_index(i);
        }
        index = WL_MEMORY_NUM - 1;
    }

    wl_adapter_save[index].addr_type = WL_VALID_ADDR_TYPE;
    memcpy(wl_adapter_save[index].wl_addr, addr, 6);
    wl_adapter_save[index].wl_voice_param.mic_vol_level = MIC_VOL_DEFALUT_LEVEL;
    wl_adapter_save[index].wl_voice_param.echo_level = ECHO_DEFALUT_LEVEL;
    wl_adapter_save[index].wl_voice_param.magic_level = MAGIC_DEFALUT_LEVEL;
    wl_adapter_save[index].wl_voice_param.mute_level = MUTE_DEFALUT_LEVEL;

    ///直接保存信息并更新
    wl_info_save_for_index(index);
    wl_sync_link_info();

    return index;
}

AT(.text.le.db.init) WEAK
void wl_db_init(void){
    int i;
    for (i=0;i<WL_MEMORY_NUM;i++){
        memset(&wl_adapter_save[i], 0, sizeof(wl_adapter_save_str));
        wl_info_load_for_index(i);
        if(wl_addr_is_null((uint8_t *)wl_adapter_save[i].wl_addr, 6)){
            wl_device_db_remove(i);
        }
    }
}

#endif
