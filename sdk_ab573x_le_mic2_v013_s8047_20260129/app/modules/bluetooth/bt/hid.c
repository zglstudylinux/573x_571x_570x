#include "include.h"
#include "api.h"

#if BT_HID_EN


#if BT_HID_MANU_EN
void bsp_bt_hid_tog_conn(void)
{
}
#endif

bool bsp_bt_hid_photo(u16 keycode)
{
    return false;
}

#if BT_HID_VOL_CTRL_EN
bool bsp_bt_hid_vol_change(u16 keycode)
{
    return false;
}
#endif

//点击左边屏幕中心位置
void bsp_bt_hid_screen_left(void)
{
    bt_hid_touch_screen(0);
    delay_5ms(10);
    bt_hid_touch_screen(1);
}

//点击右边屏幕中心位置
void bsp_bt_hid_screen_right(void)
{
    bt_hid_touch_screen(0x11);
    delay_5ms(10);
    bt_hid_touch_screen(0x10);
}
#endif

