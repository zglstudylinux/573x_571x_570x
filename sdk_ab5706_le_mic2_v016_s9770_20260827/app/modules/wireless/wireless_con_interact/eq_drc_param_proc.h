#ifndef _EQ_DRC_PARAM_PROC_H
#define _EQ_DRC_PARAM_PROC_H


uint8_t *wl_eq_param_get(uint16_t *buf_len);
uint8_t *wl_drc_param_get(uint16_t *buf_len);
void wl_eq_drc_replace_set(wl_xcfg_str_t * wl_xcfg_ptr);

void wl_eq_param_set(void);
void wl_drc_param_set(void);

#endif // _EQ_DRC_PARAM_PROC_H
