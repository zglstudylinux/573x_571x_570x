#include "include.h"

#if (CHARGE_BOX_TYPE == CBOX_NWB)
void charge_box_nwb_init(void)
{
    RTCCON8 &= ~BIT(19);                                //RI_LEAK_USB = 0
    RTCCON8 = (RTCCON8 & ~(3 << 29)) | (2 << 29);
    RTCCON13 |= BIT(29);                                //inbox high level wakeup
}
#endif