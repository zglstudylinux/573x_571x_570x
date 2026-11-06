depend(0x01020100);

config(SUB, "系统配置", "系统的相关配置");
config(LIST, "提示音语言选择", "选择系统的语言", LANG_ID, 4, "英文", "中文", "中英文(出厂默认英文)", "中英文(出厂默认中文)", 0);
config(LEVEL, 0x03);
config(CHECK, "POWKEY 10s复位系统", "是否长按POWKEY 10s复位系统，用于防止系统死机", POWKEY_10S_RESET, 1);
config(LISTVAL, "自动休眠时间", "设置自动休眠时间", SYS_SLEEP_TIME, SHORT, 30, 21, ("不休眠", 0), ("10秒钟后", 10), ("20秒钟后", 20), ("30秒钟后", 30), ("45秒钟后", 45), ("1分钟后", 60), ("2分钟后", 120), ("3分钟后", 180), ("4分钟后", 240), ("5分钟后", 300), ("6分钟后", 360), ("7分钟后", 420), ("8分钟后", 480), ("9分钟后", 540), ("10分钟后", 600), ("15分钟后", 900), ("20分钟后", 1200), ("25分钟后", 1500), ("30分钟后", 1800), ("45分钟后", 2700), ("1小时后", 3600));
config(LISTVAL, "自动关机时间", "设置自动关机时间", SYS_OFF_TIME, 	SHORT, 300, 18, ("不关机", 0), ("30秒钟后", 30), ("1分钟后", 60),  ("2分钟后", 120), ("3分钟后", 180), ("4分钟后", 240), ("5分钟后", 300), ("6分钟后", 360), ("7分钟后", 420), ("8分钟后", 480), ("9分钟后", 540), ("10分钟后", 600), ("15分钟后", 900), ("20分钟后", 1200), ("25分钟后", 1500), ("30分钟后", 1800), ("45分钟后", 2700), ("1小时后", 3600));
config(CHECK, "低电提示", "当电压比较低的时候，会有提示音警告", LOWPOWER_WARNING_EN, 1);
config(LIST, "低电语音提示电压", "选择低电语音提示电压", LPWR_WARNING_VBAT, 10, "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 4, LOWPOWER_WARNING_EN);
config(LIST, "低电关机电压", "选择低电关机电压", LPWR_OFF_VBAT, 11, "不关机", "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 2, LOWPOWER_WARNING_EN);
config(BYTE, "低电语音播报周期(秒)", "设置低电语音播报周期(秒)", LPWR_WARNING_PERIOD, 1, 240, 30, LOWPOWER_WARNING_EN);
config(LIST, "OSC基础电容", "配置24M晶振OSCI与OSCO基础电容（6PF）", OSC_BOTH_CAP, 2, "0PF", "6PF", 0);
config(BYTE, "自定义OSCI电容", "配置24M晶振OSCI负载电容（0.25PF）。电容大小：n * 0.25PF + 基础电容", UOSCI_CAP, 0, 63, 45);
config(BYTE, "自定义OSCO电容", "配置24M晶振OSCO负载电容（0.25PF）。电容大小：n * 0.25PF + 基础电容", UOSCO_CAP, 0, 63, 45);
config(CHECK,  "优先使用产测电容值",  "是否优先使用产测校准的OSC电容值，没过产测或者关闭时使用<自定义OSCI电容>、<自定义OSCO电容>",  FT_OSC_CAP_EN,  1);

config(CHECK, "EQ调试（蓝牙串口）", "是否使用SPP调试EQ功能", EQ_DGB_SPP_EN, 0);
config(CHECK, "HUART调试(EQ/FCC)", "是否使用HUART调试功能", HUART_EN, 0);
config(LISTVAL, "HUART串口选择", "选择HUART的IO", HUART_SEL, BIT, 4, 0, 11, ("PB2", 0), ("PB4", 1), ("PB3", 2), ("PB6", 3), ("PB7", 4), ("PE2", 5), ("PE3", 6), ("PE5", 7), ("PE6", 8), ("PE7", 9), ("VUSB", 10), HUART_EN);
config(CHECK, "UART1调试(仓/IODM)", "是否使用UART1调试功能", UART1_EN, 0);
config(LISTVAL, "UART1串口选择", "选择UART1的IO", UART1_SEL, BIT, 2, 2, 4, ("PB1", 0), ("PB7", 1), ("VUSB", 2), ("PE5", 3), UART1_EN);
config(CHECK,  "是否使能NTC",  "是否使能NTC",  NTC_EN,  0);
config(CHECK, "NTC使用内部10K上拉", "NTC是否使用内部10K上拉,使能程序中的宏USER_NTC后才有效", NTC_USER_INNER_PU,0, NTC_EN);
config(BYTE,  "NTC阈值",  "NTC阈值",  NTC_THD_VAL,  0, 255, 100,  NTC_EN);
config(CHECK, "QTEST", "是否使用QTEST快测功能", QTEST_EN, 0);

config(SUB, "DAC配置", "DAC的相关配置");
config(LISTVAL, "DAC输出采样率", "DAC选择输出的采样率", DAC_SPR_SEL, BIT, 2, 0, 2, ("44.1KHz", 0), ("48KHz", 1));
config(CHECK, "DAC扩展模式", "提高DAC输出功率，功耗会增加", DAC_MAXOUT_EN, 0);
config(CHECK, "省VCM方案", "是否使用内部VCM，AB5736A\D、AB5736E\F\H使用内部VCM", DAC_VCM_LESS_EN, 1);
config(LEVEL, 0x00);
config(CHECK, "蓝牙连接时关闭DAC", "蓝牙连接过程关闭DAC，仅对差分/VCMVUF输出有效", DAC_OFF_FOR_CONN, 0);
config(LEVEL, 0x03);
config(CHECK, "功放MUTE功能", "是否使能功放MUTE功能", SPK_MUTE_EN, 0);
config(LISTVAL, "功放MUTE控制IO选择", "功放MUTE控制GPIO选择", SPK_MUTE_IO_SEL, BIT, 5, 0, 17, ("None", 0), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PB6", 15), ("PB7", 16), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE3", 20), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), SPK_MUTE_EN);
config(CHECK, "高MUTE", "是否为高MUTE，否则为低MUTE", HIGH_MUTE, 1, SPK_MUTE_EN);
config(BYTE, "功放MUTE延时(单位5ms)", "功放MUTE的延时控制，防止解MUTE时间不够导致声音不全。", LOUDSPEAKER_UNMUTE_DELAY, 0, 255, 6, SPK_MUTE_EN);
config(LEVEL, 0x00);
config(LIST, "功放AB/D控制模式", "功放AB/D控制模式选择", AMPABD_TYPE, 2, "独立IO电平控制", "mute脉冲控制", 0);
config(LISTVAL, "功放AB/D控制IO选择", "功放AB/D控制GPIO选择", AMPABD_IO_SEL, BIT, 5, 0, 17, ("None", 0), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PB6", 15), ("PB7", 16), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE3", 20), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24));
config(LISTVAL, "耳机检测IO选择", "选择耳机检测IO选择", EARPHONE_DET_IOSEL,  BIT, 6, 0, 20, ("None", 0), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PB6", 15), ("PB7", 16), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE3", 20), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("复用SDCLK检测", 60), ("复用SDCMD检测", 61), ("复用PWRKEY检测", 62));
config(LEVEL, 0x03);

config(SUB, "音量配置", "音量的相关配置");
config(LIST, "音乐衰减增益", "配置音乐音量衰减，调节音量表均衡音乐和通话音量。", DAC_MAX_GAIN, 7, "0DB", "-1DB", "-2DB", "-3DB", "-4DB", "-5DB", "-6DB", 0);
config(LIST, "通话衰减增益", "配置通话音量衰减，调节音量表均衡音乐和通话音量。", BT_CALL_MAX_GAIN, 7, "0DB", "-1DB", "-2DB", "-3DB", "-4DB", "-5DB", "-6DB", 0);
config(LIST, "音量级数", "选择系统音量级数", VOL_MAX, 2, "16级音量", "32级音量", 1);
config(BYTE, "开机默认音量比例", "开机默认音量比例（0~100%）", SYS_INIT_VOL_PERC, 0, 100, 100);
config(BYTE, "开机最小音量阈值", "开机音量小于该比例（0~100%）时，恢复成阈值音量，避免开机无声", SYS_RECOVER_INITVOL_PERC, 0, 100, 15);
config(BYTE, "提示音音量比例", "提示音音量比例（0~100%）", WARNING_VOLUME_PERC, 0, 100, 65);

config(SUB, "电源配置", "电源的相关配置");
config(CHECK, "BUCK MODE", "是否设置为BUCK MODE", BUCK_MODE_EN, 0);
config(RELATE, BUCK_MODE_EN, 1);
config(CHECK, "SIDO MODE", "是否打开BUCK SIDO MODE", SIDO_MODE_EN, 0);
config(RELATE, BUCK_MODE_EN, 0);
config(CHECK, "VDDBT省电容", "是否VDDBT省电容，只能LDO模式才能省电容", VDDBT_CAPLESS_EN, 0);
config(RELATE);
config(LISTVAL, "VDDIO电压", "选择VDDIO电压, VDDAUD与VDDIO短接可能需要调VDDIO电压。", VDDIO_SEL, BIT, 4, 7, 9, ("None", 0), ("2.9V", 6), ("3.0V", 7), ("3.1V", 8), ("3.2V", 9), ("3.3V", 10), ("3.4V", 11), ("3.5V", 12), ("3.6V", 13));

config(SUB, "充电配置", "充电功能的相关配置");
config(CHECK, "充电使能", "是否打开充电功能", CHARGE_EN, 1);
config(CHECK, "涓流充电使能", "是否使能涓流充电", CHARGE_TRICK_EN, 1, CHARGE_EN);
config(CHECK, "插入DC禁止软开机", "PWRKEY软开机，DC Online时，禁止软开机", CHARGE_DC_NOT_PWRON, 1, CHARGE_EN);
config(CHECK, "充电电压跟随模式", "是否打开充电电压跟随模式，需支持快充的仓", CHARGE_VOLTAGE_FOLLOW, 0, CHARGE_EN);
config(LIST, "充电截止电流", "选择充电截止电流", CHARGE_STOP_CURR, 16, "0mA", "2.5mA", "5mA", "7.5mA", "10mA", "12.5mA", "15mA", "17.5mA", "20mA", "22.5mA", "25mA", "27.5mA", "30mA", "32.5mA", "35mA", "37.5mA", 4, CHARGE_EN);
config(LIST, "恒流充电电流", "恒流充电（电池电压大于2.9v）电流", CHARGE_CONSTANT_CURR, 33, "5mA", "10mA", "15mA", "20mA", "25mA", "30mA", "35mA", "40mA", "45mA", "50mA", "55mA", "60mA", "65mA", "70mA", "75mA", "80mA", "90mA", "100mA", "110mA", "120mA", "130mA", "140mA", "150mA", "160mA", "170mA", "180mA", "190mA", "200mA",, "210mA", "220mA", "230mA", "240mA", "250mA" 7, CHARGE_EN);
config(LIST, "涓流充电电流", "涓流充电（电池电压小于2.9v）电流", CHARGE_TRICKLE_CURR, 8, "5mA", "10mA", "15mA", "20mA", "25mA", "30mA", "35mA", "40mA", 3, CHARGE_TRICK_EN);
config(LISTVAL, "充满电蓝灯亮", "设置充满电蓝灯亮时间", CHARGE_FULL_BLED, BIT, 3, 3, 8,  ("不亮蓝灯", 0), ("亮10秒", 1), ("亮20秒", 2), ("亮30秒", 3), ("1分钟", 4), ("2分钟", 5), ("3分钟", 6), ("常亮", 7), CHARGE_EN);
config(LISTVAL, "充电截止计时", "电压满足后开始计数", CHARGE_STOP_TIME, SHORT, 3, 13, ("不计时", 0), ("10min", 1), ("20min", 2),("30min", 3),("40min", 4),("50min", 5),("60min", 6),("70min", 7),("80min", 8),("90min", 9),("120min", 12),("150min", 15),("180min", 18);
config(CHECK, "充满电自动关机", "电池充满自动关机功能，主要用于放入电池仓充电的选项", CH_FULL_AUTO_PWRDWN_EN, 1, CHARGE_EN);
config(CHECK, "充电仓功能", "是否使能充电仓功能", CHBOX_EN, 1, CHARGE_EN);
config(LISTVAL, "充电仓类型选择", "选择正确类型充电仓，确保充满关机和拿起开机功能正常", CH_BOX_TYPE_SEL, BIT, 3, 0, 5, ("兼容5V短暂掉0V后维持电压", 0), ("5V不掉电的充电仓", 1), ("5V掉电但有维持电压", 2), ("5V完全掉电无维持电压", 3), ("无电池充电仓(需同时打开宏)", 4), CHBOX_EN);
config(LISTVAL, "仓允许最低维持电压", "inbox信号电压，即充满维持电压的充电仓允许的最低维持电压", CH_INBOX_SEL, BIT, 1, 0, 2, ("1.1V", 0), ("1.7V", 1), CHBOX_EN);
config(CHECK, "从充电仓拿出自动开机", "耳机从充电仓拿出自动开机，注意充满后电池仓自动断电的不能开此选项", CH_OUT_AUTO_PWRON_EN, 1 CHBOX_EN);
config(LISTVAL, "出仓或入仓VUSB漏电配置", "加速耳机出仓时自动开机或入仓时唤醒充电仓", CH_LEAKAGE_SEL, BIT, 2, 2, 4, ("None", 0), ("漏电电流X1档", 1), ("漏电电流X2档", 2), ("漏电电流X3档", 3), CHARGE_EN);
config(CHECK, "入仓耳机关机", "有些维持电压的电池仓，耳机入仓后电池仓的5V不会自动起来进入充电, 这时耳机是否进入关机。", CHG_INBOX_PWRDWN_EN, 1, CHBOX_EN);
config(BIT, "短暂掉0V的仓稳定检测时间", "短暂掉0V的仓稳定检测时间: 500ms + n*100ms", CHBOX_OUT_DELAY,  4, 0, 15, 0, CHBOX_EN);

config(SUB, "蓝牙配置", "蓝牙的相关配置");
config(TEXT, "蓝牙名称", "手机上可以看到的蓝牙名称", BT_NAME, 32, "Bluetrum-TWS");
config(MAC, "蓝牙地址", "蓝牙的MAC地址", BT_ADDR, 6, 41:42:00:00:00:00, 41:42:FF:FF:FF:FF, 41:42:00:00:00:01);
config(LISTVAL, "长按几秒开机进配对", "是否支持长按N秒开机直接进入配对状态", BT_PWRKEY_NSEC_DISCOVER, BIT, 3, 0, 8, ("不支持", 0), ("2秒", 1), ("3秒", 2), ("4秒", 3), ("5秒", 4), ("6秒", 5), ("7秒", 6), ("8秒", 7));
config(CHECK, "配对提示音", "是否播放配对提示音", WARNING_BT_PAIR, 1);
config(LEVEL, 0x03);

config(LEVEL, 0x03);
config(SUB, "无线配置", "无线麦的相关配置");
config(CHECK, "无线mic接收端使能", "是否打开无线mic接收端功能", WIRELESS_ADAPTER_EN, 0);
config(CHECK, "无线mic发射端使能", "是否打开无线mic发射端功能", WIRELESS_DEVICE_EN, 0);
config(TEXT, "设备名称", "无线设备的配对名称", WL_NAME, 32, "LE-MIC2");

config(SUB, "RF参数", "蓝牙RF参数");
config(S8, "降低预置RF参数发射功率", "微调<预置RF参数>或<FT的RF参数>的发射功率，每级可降低0.5dbm发射功率（负数则增加功率）", BT_RF_PWRDEC, -6, 32, 0);
config(CHECK,  "优先使用FT的RF参数",  "是否优先使用FT校准的RF参数，没过FT或关闭时使用<预置RF参数>或<自定义RF参数>",  FT_RF_PARAM_EN,  1);
config(CHECK, "自定义RF参数", "打开后，有FT的芯片优先使用<FT的RF参数>，否则使用<自定义RF参数>", BT_RF_PARAM_EN, 0);
config(LEVEL, 0x03);
config(BYTE, "GL_PA_GAIN",    "PA_GAIN, 参考值3",         RF_PA_GAIN,    0,  7,  7, BT_RF_PARAM_EN);
config(LEVEL, 0x03);
config(BYTE, "GL_MIX_GAIN",   "MIX_GAIN, 参考值3~5",      RF_MIX_GAIN,   0,  7,  6, BT_RF_PARAM_EN);
config(BYTE, "GL_DIG_GAIN",   "DIG_GAIN, 参考值30~59",    RF_DIG_GAIN,   24, 59, 55, BT_RF_PARAM_EN);
config(LEVEL, 0x00);
config(BYTE, "GL_PA_CAP",     "PA_CAP, 参考值12~14",      RF_PA_CAP,     0, 15, 4, BT_RF_PARAM_EN);
config(BYTE, "GL_MIX_CAP",    "MIX_CAP, 参考值7~9",       RF_MIX_CAP,    0, 15,  8, BT_RF_PARAM_EN);
config(BYTE, "GL_TX_DBM",     "Cable实测dbm值",           RF_TXDBM,      0, 12,  8, BT_RF_PARAM_EN);
config(BYTE, "GL_UDF0",       "UDF(保留使用)",            RF_UDF0,       0,  0,  0, BT_RF_PARAM_EN);
config(BYTE, "GL_UDF1",       "UDF(保留使用)",            RF_UDF1,       0,  0,  0, BT_RF_PARAM_EN);
config(LEVEL, 0x03);
config(LISTVAL, "降低回连功率", "是否降低回连手机或TWS的功率", BT_RF_PAGE_PWRDEC, BIT, 3, 1, 4, ("不降低", 0), ("降低3dbm", 1), ("降低6dbm", 2), ("降低9dbm", 3));
config(LISTVAL, "降低组队功率", "是否降低TWS搜索组队的功率", BLE_RF_PAGE_PWRDEC, BIT, 3, 0, 4, ("不降低", 0), ("降低3dbm", 1), ("降低6dbm", 2), ("降低9dbm", 3));
config(BYTE, "设置组队范围RSSI(-dbm)", "该值越小(-90dbm最小),组队范围约大", BLE_PAGE_RSSI_THR, 20, 90, 90);
config(CHECK, "通话RF参数", "是否单独配置通话RF参数", BT_RF_PARAM_SCO_EN, 0, BT_RF_PARAM_EN);
config(BYTE, "SCO_PA_GAIN",   "通话PA_GAIN, 参考值7",     RF_PA_GAIN_SCO,    0,  5,  4, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_MIX_GAIN",  "通话MIX_GAIN, 参考值3~5",  RF_MIX_GAIN_SCO,   0,  7,  6, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_DIG_GAIN",  "通话DIG_GAIN, 参考值30~59",RF_DIG_GAIN_SCO,   24,61, 55, BT_RF_PARAM_SCO_EN);
config(LEVEL, 0x00);
config(BYTE, "SCO_PA_CAP",    "通话PA_CAP, 参考值7~9",    RF_PA_CAP_SCO,     0, 15,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_MIX_CAP",   "通话MIX_CAP, 参考值7~9",   RF_MIX_CAP_SCO,    0, 15,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_TX_DBM",    "通话Cable实测dbm值",       RF_TXDBM_SCO,      0, 12,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_UDF0",      "通话UDF(保留使用)",        RF_UDF0_SCO,       0,  0,  0, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_UDF1",      "通话UDF(保留使用)",        RF_UDF1_SCO,       0,  0,  0, BT_RF_PARAM_SCO_EN);
config(LEVEL, 0x03);


config(SUB, "MIC参数", "设备端MIC参数");

config(LISTVAL, "设备端MIC配置", "选择MIC，注意配置对应MIC通路参数", CALL_TALK_MIC, BIT, 3, 0, 3,  ("None", 5), ("MIC0", 0), ("MIC1", 1), WIRELESS_DEVICE_EN);
config(BIT, "后置数字增益", "调节算法后置数字增益", BT_SCO_POST_GAIN, 4, 0, 15, 2, WIRELESS_DEVICE_EN);
config(LISTVAL, "MIC供电IO电压选择", "MIC供电IO电压选择", MIC_PWR_LEVEL, BIT, 3, 6, 8, ("2.6v", 0), ("2.65v", 1), ("2.7v", 2), ("2.75v", 3), ("2.8v", 4), ("2.85v", 5), ("2.9v", 6), ("2.5v", 7));

config(CHECK, "MIC0", "是否打开MIC0", MIC0_EN, 0);
config(LISTVAL, "MIC0偏置电路配置", "MIC0电路BIAS配置", MIC0_BIAS_METHOD, BIT, 1, 1, 2, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), MIC0_EN);
config(BYTE, "MIC0数字增益", "MIC0数字增益配置(0~39DB), Step 1DB", BT_MIC0_DIG_GAIN, 0, 36, 20, MIC0_EN);

config(CHECK, "MIC1", "是否打开MIC1", MIC1_EN, 0);
config(LISTVAL, "MIC1偏置电路配置", "MIC1电路BIAS配置，省电容、省电阻配置", MIC1_BIAS_METHOD, BIT, 2, 1, 3, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), ("单端MIC内部电阻电容", 2), MIC1_EN);
config(BYTE, "MIC1数字增益", "MIC1数字增益配置(0~39DB), Step 1DB", BT_MIC1_DIG_GAIN, 0, 36, 20, MIC1_EN);

config(LEVEL, 0x03);

config(SUB, "按键配置", "选择按键及按键功能配置");
config(LISTVAL, "软开机长按时间选择", "长按PWRKEY多长时间后软开机", PWRON_PRESS_TIME,  BIT, 3, 3, 8, ("0.1秒", 0), ("0.5秒", 1), ("1秒", 2), ("1.5秒", 3), ("2秒", 4), ("2.5秒", 5), ("3秒", 6), ("3.5秒", 7));
config(LISTVAL, "软关机长按时间选择", "长按PWRKEY多长时间后软关机", PWROFF_PRESS_TIME, BIT, 3, 3, 8, ("1.5秒", 0), ("2秒", 1), ("2.5秒", 2), ("3秒", 3), ("3.5秒", 4), ("4秒", 5), ("4.5秒", 6), ("5秒", 7));
config(LISTVAL, "双击响应时间选择", "选择双击按键响应间隔时间", DOUBLE_KEY_TIME, BIT, 3, 1, 8, ("200ms", 0), ("300ms", 1), ("400ms", 2), ("500ms", 3), ("600ms", 4), ("700ms", 5), ("800ms", 6), ("900ms", 7));
config(CHECK, "音乐PP键配置", "音乐场景PP键配置，一般是KEY_1作为PP键", KEY1_MSC_CFG_EN, 0);
config(RELATE, KEY1_MSC_CFG_EN, 1);
config(LISTVAL, "音乐PP键短按", "音乐场景PP键短按的功能", KEY1_MSC_KS_SEL,    BIT, 4, 1,  15, ("无", 0), ("PLAY/PAUSE", 1), ("PREV", 2), ("NEXT", 3), ("VOL+", 4), ("VOL-", 5), ("回拨", 6), ("SIRI", 7), ("低延时", 8), ("降噪控制", 9), ("中英文切换", 10), ("MODE", 11), ("HID拍照", 12), ("IOS HOME", 13), ("CBT_TEST", 14));
config(LISTVAL, "音乐PP键长按", "音乐场景PP键长按的功能", KEY1_MSC_KL_SEL,    BIT, 4, 7,  15, ("无", 0), ("PLAY/PAUSE", 1), ("PREV", 2), ("NEXT", 3), ("VOL+", 4), ("VOL-", 5), ("回拨", 6), ("SIRI", 7), ("低延时", 8), ("降噪控制", 9), ("中英文切换", 10), ("MODE", 11), ("HID拍照", 12), ("IOS HOME", 13), ("CBT_TEST", 14));
config(LISTVAL, "音乐PP键双击", "音乐场景PP键双击的功能", KEY1_MSC_KD_SEL,    BIT, 4, 6,  15, ("无", 0), ("PLAY/PAUSE", 1), ("PREV", 2), ("NEXT", 3), ("VOL+", 4), ("VOL-", 5), ("回拨", 6), ("SIRI", 7), ("低延时", 8), ("降噪控制", 9), ("中英文切换", 10), ("MODE", 11), ("HID拍照", 12), ("IOS HOME", 13), ("CBT_TEST", 14));
config(LISTVAL, "音乐PP键三击", "音乐场景PP键三击的功能", KEY1_MSC_KTH_SEL,	  BIT, 4, 0,  15, ("无", 0), ("PLAY/PAUSE", 1), ("PREV", 2), ("NEXT", 3), ("VOL+", 4), ("VOL-", 5), ("回拨", 6), ("SIRI", 7), ("低延时", 8), ("降噪控制", 9), ("中英文切换", 10), ("MODE", 11), ("HID拍照", 12), ("IOS HOME", 13), ("CBT_TEST", 14));
config(LISTVAL, "音乐PP键四击", "音乐场景PP键四击的功能", KEY1_MSC_KFOUR_SEL, BIT, 4, 0,  15, ("无", 0), ("PLAY/PAUSE", 1), ("PREV", 2), ("NEXT", 3), ("VOL+", 4), ("VOL-", 5), ("回拨", 6), ("SIRI", 7), ("低延时", 8), ("降噪控制", 9), ("中英文切换", 10), ("MODE", 11), ("HID拍照", 12), ("IOS HOME", 13), ("CBT_TEST", 14));
config(LISTVAL, "音乐PP键五击", "音乐场景PP键五击的功能", KEY1_MSC_KFIVE_SEL, BIT, 4, 14, 15, ("无", 0), ("PLAY/PAUSE", 1), ("PREV", 2), ("NEXT", 3), ("VOL+", 4), ("VOL-", 5), ("回拨", 6), ("SIRI", 7), ("低延时", 8), ("降噪控制", 9), ("中英文切换", 10), ("MODE", 11), ("HID拍照", 12), ("IOS HOME", 13), ("CBT_TEST", 14));
config(RELATE);
config(CHECK, "通话PP键配置", "通话场景PP键配置，一般是KEY_1作为PP键", KEY1_CALL_CFG_EN, 0);
config(RELATE, KEY1_CALL_CFG_EN, 1);
config(LISTVAL, "通话PP键短按", "通话场景PP键短按的功能", KEY1_CALL_KS_SEL,    BIT, 4, 1, 8, ("无", 0), ("接听(挂起当前)/挂断", 1), ("接听(挂断当前)/挂断", 2), ("拒接", 3), ("切换私密接听", 4), ("切换三方通话", 5), ("VOL+", 6), ("VOL-", 7));
config(LISTVAL, "通话PP键长按", "通话场景PP键长按的功能", KEY1_CALL_KL_SEL,    BIT, 4, 3, 8, ("无", 0), ("接听(挂起当前)/挂断", 1), ("接听(挂断当前)/挂断", 2), ("拒接", 3), ("切换私密接听", 4), ("切换三方通话", 5), ("VOL+", 6), ("VOL-", 7));
config(LISTVAL, "通话PP键双击", "通话场景PP键双击的功能", KEY1_CALL_KD_SEL,    BIT, 4, 4, 8, ("无", 0), ("接听(挂起当前)/挂断", 1), ("接听(挂断当前)/挂断", 2), ("拒接", 3), ("切换私密接听", 4), ("切换三方通话", 5), ("VOL+", 6), ("VOL-", 7));
config(LISTVAL, "通话PP键三击", "通话场景PP键三击的功能", KEY1_CALL_KTH_SEL,   BIT, 4, 5, 8, ("无", 0), ("接听(挂起当前)/挂断", 1), ("接听(挂断当前)/挂断", 2), ("拒接", 3), ("切换私密接听", 4), ("切换三方通话", 5), ("VOL+", 6), ("VOL-", 7));
config(LISTVAL, "通话PP键四击", "通话场景PP键四击的功能", KEY1_CALL_KFOUR_SEL, BIT, 4, 0, 8, ("无", 0), ("接听(挂起当前)/挂断", 1), ("接听(挂断当前)/挂断", 2), ("拒接", 3), ("切换私密接听", 4), ("切换三方通话", 5), ("VOL+", 6), ("VOL-", 7));
config(LISTVAL, "通话PP键五击", "通话场景PP键五击的功能", KEY1_CALL_KFIVE_SEL, BIT, 4, 0, 8, ("无", 0), ("接听(挂起当前)/挂断", 1), ("接听(挂断当前)/挂断", 2), ("拒接", 3), ("切换私密接听", 4), ("切换三方通话", 5), ("VOL+", 6), ("VOL-", 7));
config(RELATE);

config(SUB, "LED灯配置", "选择及配置LED状态指示灯");
config(CHECK, "系统指示灯(蓝灯)", "是否使用系统状态指示灯", LED_DISP_EN, 1);
config(CHECK, "电源状态灯(红灯)", "是否使用充电/电源指示灯", LED_PWR_EN, 1);
config(CHECK, "省电阻1个IO推两个灯", "省电阻1个IO推两个灯，不能兼容升级功能", PORT_2LED_RESLESS_EN, 1, LED_DISP_EN);
config(CHECK, "电池低电闪灯", "电池低电时，红灯或蓝灯是否闪烁？", RLED_LOWBAT_EN, 0, LED_DISP_EN);
config(LISTVAL, "蓝灯IO选择", "蓝灯的GPIO口选择", BLED_IO_SEL, BIT, 5, 0, 17, ("None", 0), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PB6", 15), ("PB7", 16), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE3", 20), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), LED_DISP_EN);
config(LISTVAL, "红灯IO选择", "红灯的GPIO口选择", RLED_IO_SEL, BIT, 5, 0, 17, ("None", 0), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PB6", 15), ("PB7", 16), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE3", 20), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), LED_PWR_EN);

config(CHECK, "自定义LED闪灯状态", "是否需要自定义LED闪灯状态", LED_STA_CONFIG_EN, 0);
config(LED, "开机", "配置开机状态下的闪灯状态", LED_POWERON, 0x02, 0x01, 10, 255, 		LED_STA_CONFIG_EN);
config(LED, "关机", "配置关机状态下的闪灯状态", LED_POWEROFF, 0x40, 0x15, 4, 255, 		LED_STA_CONFIG_EN);
config(LED, "初始化", "配置蓝牙初始化状态下的闪灯状态", LED_BTINIT, 0x00, 0xaa, 10, 0, 	LED_STA_CONFIG_EN);
config(LED, "回连", "配置回连状态下的闪灯状态", LED_RECONNECT, 0x00, 0xaa, 6, 0, 		LED_STA_CONFIG_EN);
config(LED, "配对", "配置配对状态下的闪灯状态", LED_PAIRING, 0x60, 0x03, 3, 6, 			LED_STA_CONFIG_EN);
config(LED, "已连接", "配置已连接状态下的闪灯状态", LED_CONNECTED, 0x00, 0x02, 2, 86, 	LED_STA_CONFIG_EN);
config(LED, "播放", "配置播放状态下的闪灯状态", LED_PLAYING, 0x00, 0x02, 2, 86, 		LED_STA_CONFIG_EN);
config(LED, "来电", "配置来电状态下的闪灯状态", LED_RING, 0x00, 0x02, 2, 86, 			LED_STA_CONFIG_EN);
config(LED, "通话", "配置通话状态下的闪灯状态", LED_CALLING, 0x00, 0x02, 2, 86, 		LED_STA_CONFIG_EN);
config(LED, "低电", "配置低电状态下的闪灯状态", LED_LOWBAT, 0xaa, 0x00, 6, 0, 			LED_STA_CONFIG_EN);

config(LEVEL, 0x0E);
config(SUB, "产测参数", "产测校准的一些参数，不要改动！");
config(KEEP);
config(BYTE, "产测OSCI电容", "产测校准的24M晶振OSCI负载电容，单位0.25PF。不要改动默认值0。", OSCI_CAP, 0, 63, 0);
config(BYTE, "产测OSCO电容", "产测校准的24M晶振OSCO负载电容，单位0.25PF。不要改动默认值0。", OSCO_CAP, 0, 63, 0);
config(BUF, "授权密钥", "程序运行需要判断的合法授权密钥", SOFT_KEY, 20);
config(KEEPEND)

config(LEVEL, 0x100);
makecfgfile(xcfg.bin);
makecfgdef(xcfg.h);
xcopy(xcfg.h, ../../xcfg.h);
