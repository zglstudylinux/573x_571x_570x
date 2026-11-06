#ifndef __FUNC_ADAPTER_H
#define __FUNC_ADAPTER_H

void func_adapter(void);
void func_adapter_init(void);
void func_adapter_message(u16 msg);

void func_adapter_set_create_con(void);

#if ADAPTER_SECOND_CON_VERS_EN
void adapter_con_vers_set_adv(uint8_t pscan, uint8_t iscan);
#define adapter_adv_set_enable(pscan,iscan)     adapter_con_vers_set_adv(pscan,iscan)
#else
#define adapter_adv_set_enable(pscan,iscan)     wireless_adv_set_enable(pscan,iscan)
#endif
#endif
