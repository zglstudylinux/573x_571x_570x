#include "include.h"
#include "api.h"

// uint8_t cfg_bb_rf_gfsk_pwr = 127;     //微调GFSK TXPWR，范围：90~127

////获取配置中的RF参数
//const uint8_t *bt_rf_get_param(void)
//{
//    //优先使用FT参数，其次自定义参数，最后是库预置参数
//    if(xcfg_cb.ft_rf_param_en && bt_get_ft_trim_value(&xcfg_cb.rf_pa_gain)) {
//        return (const uint8_t *)&xcfg_cb.rf_pa_gain;
//    } else if(xcfg_cb.bt_rf_param_en) {
//        return (const uint8_t *)&xcfg_cb.rf_pa_gain;
//    }
//    return NULL;
//}

#if BT_RF_EXT_CTL_EN || BT_RF_PWR_BALANCE_EN
const uint8_t cfg_bb_rf_ctl = BIT(0);   //bit0=RF_EXT_CTL_EN


//此处添加外部PA/LNA控制IO，注意所有函数放公共区，不能加打印
AT(.com_text.isr.txrx)
void bb_rf_ext_ctl_cb(u32 rf_sta)
{
//	if(rf_sta & BIT(8)) {			//tx on, enable PA
//	} else if(rf_sta & BIT(9)){	    //tx down, disable PA
//	} else if(rf_sta & BIT(10)){	//rx on, enable LNA
//	} else if(rf_sta & BIT(11)){	//rx down, disable LNA
//	} else {						//idle
//	}
#if BT_RF_PWR_BALANCE_EN
    btrf_power_balance_cb(rf_sta);
#endif
}

//初始化蓝牙时，初始化外部PA/LNA控制IO
void bb_rf_ext_ctl_init(void)
{
#if BT_RF_PWR_BALANCE_EN
    btrf_power_balance_init();
#endif
}

//关闭蓝牙时，关闭外部PA/LNA
void bb_rf_ext_ctl_exit(void)
{
#if BT_RF_PWR_BALANCE_EN
    btrf_power_balance_exit();
#endif
}
#endif


#if BT_RF_PWR_BALANCE_EN
#define CONST_CAT(x, y)                          x ## y
#define SET_MACRO(x, y)                          CONST_CAT(x, y)
#define POWER_BALANCE_GPIO_INIT(x, y)           (SET_MACRO(GPIO, SET_MACRO(x, DE)) |= BIT(y), SET_MACRO(GPIO, SET_MACRO(x, FEN)) &= ~BIT(y), SET_MACRO(GPIO, SET_MACRO(x, DIR)) |= BIT(y))

#define POWER_BALANCE_GPIO_UPFDOWN_ON(x, y)     (SET_MACRO(GPIO, SET_MACRO(x, PU300)) |= BIT(y), SET_MACRO(GPIO, SET_MACRO(x, PD300)) |=BIT(y))
#define POWER_BALANCE_GPIO_UPFDOWN_OFF(x, y)    (SET_MACRO(GPIO, SET_MACRO(x, PU300))&= ~BIT(y), SET_MACRO(GPIO, SET_MACRO(x, PD300)) &= ~BIT(y))

#define BT_RF_PWR_TIMR_INIT()                   {CLKGAT0 |= BIT(4); asm("nop");asm("nop");asm("nop");asm("nop");}
#define BT_RF_PWR_TIMR_SET(us)                  {TMR4PR  = us*3 -1;	TMR4CNT = 0;}
#define BT_RF_PWR_TIMR_START()                  {TMR4CON = BIT(7)| (3<<4)| BIT(2)|BIT(0);}
#define BT_RF_PWR_TIMR_STOP()                   {TMR4CON = 0;}

volatile u8 btrf_power_level = 0;

AT(.com_text.btrf_pwrbalance)
void btrf_power_balance_level_set(u8 level)   //unit 3.3V/600 = 5.5mA
{
    POWER_BALANCE_GPIO_UPFDOWN_OFF(A, 3);
    POWER_BALANCE_GPIO_UPFDOWN_OFF(A, 4);
    POWER_BALANCE_GPIO_UPFDOWN_OFF(A, 5);
    POWER_BALANCE_GPIO_UPFDOWN_OFF(A, 6);

    POWER_BALANCE_GPIO_UPFDOWN_OFF(B, 0);
    POWER_BALANCE_GPIO_UPFDOWN_OFF(B, 1);

    POWER_BALANCE_GPIO_UPFDOWN_OFF(F, 1);
    POWER_BALANCE_GPIO_UPFDOWN_OFF(F, 2);

    if (0 == level ) {
        return;
    }
    if(level >= 1){
        POWER_BALANCE_GPIO_UPFDOWN_ON(A, 3);
    }
    if(level >= 2) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(A, 4);
    }
    if(level >= 3) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(A, 5);
    }
    if(level >= 4) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(A, 6);
    }
    if(level >= 5) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(B, 0);
    }
    if(level >= 6) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(B, 1);
    }
    if(level >= 7) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(F, 1);
    }
    if(level >= 8) {
        POWER_BALANCE_GPIO_UPFDOWN_ON(F, 2);
    }
}

AT(.com_text.isr.txrx)
void btrf_power_balance_cb(u32 rf_sta)
{
	if(rf_sta & BIT(8)) {			//tx on
        btrf_power_balance_level_delay_set(55,0);       //TX开始后,TX延时一段时间才真正耗电,这里延时一段时间后才降低功耗
	} else if(rf_sta & BIT(9)){	    //tx down
        btrf_power_balance_level_delay_set(12,7);       //TX结束,增加耗电
	} else if(rf_sta & BIT(10)){	//rx on
        btrf_power_balance_level_delay_set(45,3);       //RX开始后,RX延时一段时间才真正耗电,这里延时一段时间后才降低功耗
	} else if(rf_sta & BIT(11)){	//rx down
        btrf_power_balance_level_set(8);                //RX结束,增加耗电
	} else {						//idle
        btrf_power_balance_level_set(8);                //空闲状态,增加耗电

	}
}

AT(.com_text.isr.txrx)
void btrf_power_balance_level_delay_set(u32 delay_us, u8 level) //unit us
{
    btrf_power_level = level;
    BT_RF_PWR_TIMR_SET(delay_us);
    BT_RF_PWR_TIMR_START();
}

void btrf_power_balance_init(void)
{
    timer_isr_for_btrf_power_balance();

    POWER_BALANCE_GPIO_INIT(A, 3);
    POWER_BALANCE_GPIO_INIT(A, 4);
    POWER_BALANCE_GPIO_INIT(A, 5);
    POWER_BALANCE_GPIO_INIT(A, 6);
    POWER_BALANCE_GPIO_INIT(B, 0);
    POWER_BALANCE_GPIO_INIT(B, 1);
    POWER_BALANCE_GPIO_INIT(F, 1);
    POWER_BALANCE_GPIO_INIT(F, 2);
}

void btrf_power_balance_exit(void)
{
    btrf_power_balance_level_set(0);
    BT_RF_PWR_TIMR_STOP();
}


void timer_isr_for_btrf_power_balance(void)
{
    BT_RF_PWR_TIMR_INIT();
//    BT_RF_PWR_TIMR_SET(300);
//    BT_RF_PWR_TIMR_START();
    timer4_irq_init();
}
#endif // RF_PWR_BALANCE_EN
