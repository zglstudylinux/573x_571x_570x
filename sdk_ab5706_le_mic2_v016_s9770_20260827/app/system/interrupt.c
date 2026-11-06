#include "include.h"


AT(.com_text.timer)
void timer4_isr(void)
{
#if BT_RF_PWR_BALANCE_EN
    if (TMR4CON & BIT(9)) {             //overflow Pending
        GPIOECLR = BIT(7);
        TMR4CPND = BIT(9);
        TMR4CON = 0;
        btrf_power_balance_level_set(btrf_power_level);
    }
#endif // BT_RF_PWR_BALANCE_EN

#if ((GUI_SELECT == GUI_LEDSEG_7P7S)||(GUI_SELECT == GUI_LEDSEG_6C6S))
    if (TMR4CON & BIT(9)) {
        TMR4CPND = BIT(9);              //Clear Pending
        TMR4CON = 0;
#if (GUI_SELECT == GUI_LEDSEG_7P7S)
        ledseg_7p7s_clr();              //close display
#elif (GUI_SELECT == GUI_LEDSEG_6C6S)
        ledseg_6c6s_clr();
#endif
    }
#endif
}

