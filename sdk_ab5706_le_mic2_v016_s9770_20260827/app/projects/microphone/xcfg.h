//该头文件由软件自动生成，请勿随意修改！
#ifndef _XCFG_H
#define _XCFG_H

#define XCFG_EN             1

typedef struct __attribute__((packed)) _xcfg_cb_t {
    u8 lang_id;                                 //提示音语言选择: 0:英文, 1:中文, 2:中英文(出厂默认英文), 3:中英文(出厂默认中文)
    u32 wireless_adapter_en              : 1;   //无线mic接收端使能
    u32 wireless_device_en               : 1;   //无线mic发射端使能
    u32 powkey_10s_reset                 : 1;   //POWKEY 10s复位系统
    u16 sys_sleep_time;                         //自动休眠时间: 不休眠: 0, 10秒钟后: 10, 20秒钟后: 20, 30秒钟后: 30, 45秒钟后: 45, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u16 sys_off_time;                           //自动关机时间: 不关机: 0, 30秒钟后: 30, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u32 lowpower_warning_en              : 1;   //低电提示
    u8 lpwr_warning_vbat;                       //低电语音提示电压: 0:2.8V, 1:2.9V, 2:3.0V, 3:3.1V, 4:3.2V, 5:3.3V, 6:3.4V, 7:3.5V, 8:3.6V, 9:3.7V
    u8 lpwr_off_vbat;                           //低电关机电压: 0:不关机, 1:2.8V, 2:2.9V, 3:3.0V, 4:3.1V, 5:3.2V, 6:3.3V, 7:3.4V, 8:3.5V, 9:3.6V, 10:3.7V
    u8 lpwr_warning_period;                     //低电语音播报周期(秒)
    u8 osc_both_cap;                            //OSC基础电容: 0:0PF, 1:6PF
    u8 uosci_cap;                               //自定义OSCI电容
    u8 uosco_cap;                               //自定义OSCO电容
    u32 ft_osc_cap_en                    : 1;   //优先使用产测电容值
    u32 huart_en                         : 1;   //HUART调试(EQ/FCC)
    u32 huart_sel                        : 4;   //HUART串口选择: PA6: 0, PA7: 1, PB1: 2, PB2: 3, PB3: 4, PB4: 5, PE6(MICBIAS): 6, PE7: 7, PF3: 8, VUSB: 9
    u32 uart1_en                         : 1;   //UART1调试(仓/IODM)
    u32 uart1_sel                        : 2;   //UART1串口选择: PA7: 0, PF0(MICP): 1, VUSB: 2
    u32 ntc_en                           : 1;   //是否使能NTC
    u32 ntc_user_inner_pu                : 1;   //NTC使用内部10K上拉
    u8 ntc_thd_val;                             //NTC阈值
    u32 dac_sel                          : 4;   //DAC声道选择: 单声道: 0, 双声道: 1, VCMBUF单声道: 2, VCMBUF双声道: 3, 差分单声道: 4, 差分双声道: 5
    u32 dac_spr_sel                      : 2;   //DAC输出采样率: 44.1KHz: 0, 48KHz: 1
    u32 dac_vcm_less_en                  : 1;   //DAC省VCM方案
    u32 spk_mute_en                      : 1;   //功放MUTE功能
    u32 spk_mute_io_sel                  : 5;   //功放MUTE控制IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5(WKO): 14, PE0: 17, PE5: 22, PE6(MICBIAS): 23, PE7: 24, PF0(MICP): 25, PF1(MICN): 26, PF2: 27, PF3: 28, VUSB: 29
    u32 high_mute                        : 1;   //高MUTE
    u8 loudspeaker_unmute_delay;                //功放MUTE延时(单位5ms)
    u32 ampabd_io_sel                    : 5;   //功放AB/D控制IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5(WKO): 14, PE0: 17, PE5: 22, PE6(MICBIAS): 23, PE7: 24, PF0(MICP): 25, PF1(MICN): 26, PF2: 27, PF3: 28, VUSB: 29
    u8 ampabd_type;                             //功放AB/D控制模式: 0:独立IO电平控制, 1:mute脉冲控制, 2:分压式控制
    u32 earphone_det_iosel               : 6;   //耳机检测IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5(WKO): 14, PE0: 17, PE5: 22, PE6(MICBIAS): 23, PE7: 24, PF0(MICP): 25, PF1(MICN): 26, PF2: 27, PF3: 28, 复用SDCLK检测: 60, 复用SDCMD检测: 61, 复用PWRKEY检测: 62
    u8 vol_max;                                 //音量级数: 0:16级音量, 1:30级音量, 2:32级音量, 3:50级音量
    u8 sys_init_vol_perc;                       //开机默认音量比例
    u8 sys_recover_initvol_perc;                //开机最小音量阈值
    u8 warning_volume_perc;                     //提示音音量比例
    u32 huart_audio_out_en               : 1;   //HUART AUDIO输出功能
    u32 huart_out_sel                    : 4;   //HUART AUDIO OUT: PA6: 0, PA7: 1, PB1: 2, PB2: 3, PB3: 4, PB4: 5, PE6: 6, PE7: 7, PF3: 8, VUSB: 9, NONE: 10
    u32 huart_audio_in_en                : 1;   //HUART AUDIO输入功能
    u32 huart_in_sel                     : 4;   //HUART AUDIO IN: PA6: 0, PA7: 1, PB1: 2, PB2: 3, PB3: 4, PB4: 5, PE6: 6, PE7: 7, PF3: 8, VUSB: 9, NONE: 10
    u32 vddbt_capless_en                 : 1;   //VDDBT省电容
    u32 vddio_sel                        : 4;   //VDDIO电压: None: 0, 2.9V: 5, 3.0V: 6, 3.1V: 7, 3.2V: 8, 3.3V: 9, 3.4V: 10, 3.5V: 11, 3.6V: 12
    u32 charge_en                        : 1;   //充电使能
    u32 charge_trick_en                  : 1;   //涓流充电使能
    u32 charge_dc_not_pwron              : 1;   //插入DC禁止软开机
    u8 charge_stop_curr;                        //充电截止电流: 0:0mA, 1:2.5mA, 2:5mA, 3:7.5mA, 4:10mA, 5:12.5mA, 6:15mA, 7:17.5mA, 8:20mA, 9:22.5mA, 10:25mA, 11:27.5mA, 12:30mA, 13:32.5mA, 14:35mA, 15:37.5mA
    u8 charge_constant_curr;                    //恒流充电电流: 0:5mA, 1:10mA, 2:15mA, 3:20mA, 4:25mA, 5:30mA, 6:35mA, 7:40mA, 8:45mA, 9:50mA, 10:55mA, 11:60mA, 12:65mA, 13:70mA, 14:75mA, 15:80mA, 16:90mA, 17:100mA, 18:110mA, 19:120mA, 20:130mA, 21:140mA, 22:150mA, 23:160mA, 24:170mA, 25:180mA, 26:190mA, 27:200mA, 28:210mA, 29:220mA, 30:230mA, 31:240mA, 32:250mA, 33:260mA, 34:270mA, 35:280mA, 36:290mA, 37:300mA, 38:310mA, 39:320mA, 40:330mA, 41:340mA, 42:350mA, 43:360mA
    u8 charge_trickle_curr;                     //涓流充电电流: 0:5mA, 1:10mA, 2:15mA, 3:20mA, 4:25mA, 5:30mA, 6:35mA, 7:40mA
    u32 charge_full_bled                 : 3;   //充满电蓝灯亮: 不亮蓝灯: 0, 亮10秒: 1, 亮20秒: 2, 亮30秒: 3, 1分钟: 4, 2分钟: 5, 3分钟: 6, 常亮: 7
    u32 ch_full_auto_pwrdwn_en           : 1;   //充满电自动关机
    char wl_name[32];                           //设备名称
    u8 bt_addr[6];                              //设备地址
    u8 bt_rf_pwrdec;                            //降低预置RF参数发射功率
    u32 ft_rf_param_en                   : 1;   //优先使用FT的RF参数
    u32 bt_rf_param_en                   : 1;   //自定义RF参数
    u8 rf_pa_gain;                              //GL_PA_GAIN
    u8 rf_mix_gain;                             //GL_MIX_GAIN
    u8 rf_dig_gain;                             //GL_DIG_GAIN
    u8 rf_pa_cap;                               //GL_PA_CAP
    u8 rf_mix_cap;                              //GL_MIX_CAP
    u8 rf_txdbm;                                //GL_TX_DBM
    u8 rf_udf0;                                 //GL_UDF0
    u8 rf_udf1;                                 //GL_UDF1
    u32 bt_rf_page_pwrdec                : 3;   //降低回连功率: 不降低: 0, 降低3dbm: 1, 降低6dbm: 2, 降低9dbm: 3
    u32 ble_rf_page_pwrdec               : 3;   //降低组队功率: 不降低: 0, 降低3dbm: 1, 降低6dbm: 2, 降低9dbm: 3
    u8 ble_page_rssi_thr;                       //设置组队范围RSSI(-dbm)
    u32 mic_bias_sel                     : 1;   //MIC供电IO选择: None: 0, PE6(MICBIAS): 1
    u32 mic_pwr_level                    : 3;   //MIC供电IO电压选择: 1.8v: 0, 2.4v: 1, 2.5v: 2, 2.6v: 3, 2.7v: 4, 2.8v: 5, 2.9v: 6, 3.0v: 7
    u32 mic_bias_method                  : 2;   //MIC偏置电路配置: 单端MIC外部电阻电容: 0, 单端MIC内部电阻电容: 1, 差分MIC: 2
    u8 mic_anl_gain;                            //MIC模拟增益
    u8 mic_dig_gain;                            //MIC数字增益
    u32 pwron_press_time                 : 3;   //软开机长按时间选择: 0.1秒: 0, 0.5秒: 1, 1秒: 2, 1.5秒: 3, 2秒: 4, 2.5秒: 5, 3秒: 6, 3.5秒: 7
    u32 pwroff_press_time                : 3;   //软关机长按时间选择: 1.5秒: 0, 2秒: 1, 2.5秒: 2, 3秒: 3, 3.5秒: 4, 4秒: 5, 4.5秒: 6, 5秒: 7
    u32 double_key_time                  : 3;   //双击响应时间选择: 200ms: 0, 300ms: 1, 400ms: 2, 500ms: 3, 600ms: 4, 700ms: 5, 800ms: 6, 900ms: 7
    u32 led_disp_en                      : 1;   //系统指示灯(蓝灯)
    u32 led_pwr_en                       : 1;   //电源状态灯(红灯)
    u32 port_2led_resless_en             : 1;   //省电阻1个IO推两个灯
    u32 rled_lowbat_en                   : 1;   //电池低电闪灯
    u32 bled_io_sel                      : 5;   //蓝灯IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5(WKO): 14, PE0: 17, PE5: 22, PE6(MICBIAS): 23, PE7: 24, PF0(MICP): 25, PF1(MICN): 26, PF2: 27, PF3: 28
    u32 rled_io_sel                      : 5;   //红灯IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5(WKO): 14, PE0: 17, PE5: 22, PE6(MICBIAS): 23, PE7: 24, PF0(MICP): 25, PF1(MICN): 26, PF2: 27, PF3: 28
    u32 led_sta_config_en                : 1;   //自定义LED闪灯状态
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_poweron;                              //开机闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_poweroff;                             //关机闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_btinit;                               //初始化闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_reconnect;                            //回连闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_pairing;                              //配对闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_connected;                            //已连接闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_playing;                              //播放闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_ring;                                 //来电闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_calling;                              //通话闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_lowbat;                               //低电闪灯控制
    u8 xm_keep_start[0];                        //For Keep Area Start
    u8 osci_cap;                                //产测OSCI电容
    u8 osco_cap;                                //产测OSCO电容
    u8 xm_keep_end[0];                          //For Keep Area End
} xcfg_cb_t;

extern xcfg_cb_t xcfg_cb;
#endif
