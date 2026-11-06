#ifndef _ADAPTER_SAVE_PARAM_H
#define _ADAPTER_SAVE_PARAM_H

#define MIC_VOL_DEFALUT_LEVEL   4
#define MIC_VOL_MAX_LEVEL       5

#define ECHO_DEFALUT_LEVEL      3
#define ECHO_MAX_LEVEL          5

#define MAGIC_DEFALUT_LEVEL     0
#define MAGIC_MAX_LEVEL         4

#define MUTE_DEFALUT_LEVEL      0

enum {
    MIC_VOL_LEVEL_TYPE,
    ECHO_LEVEL_TYPE,
    MAGIC_LEVEL_TYPE,
    MUTE_LEVEL_TYPE,
} ;

typedef struct {
    u8 mic_vol_level;
    u8 echo_level;
    u8 magic_level;
    u8 mute_level;
} wl_voice_param_str;

typedef struct {
    wl_voice_param_str wl_voice_param;
    uint8_t wl_addr[6];
    uint8_t addr_type;
} wl_adapter_save_str;

void wl_db_init(void);
wl_adapter_save_str *wl_device_db_get_for_addr(uint8_t *addr);
int wl_device_db_index_get_for_addr(uint8_t *addr);
int wl_device_db_is_exist(uint8_t *addr);
int wl_device_db_add(uint8_t *addr);
void wl_device_save_voice_param(uint8_t wl_db_index, uint8_t param_type, uint8_t param_value);

#endif // _ADAPTER_SAVE_PARAM_H
