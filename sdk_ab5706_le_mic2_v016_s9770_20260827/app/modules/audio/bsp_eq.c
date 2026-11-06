#include "include.h"
#include "bsp_eq.h"

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


#define EQ_CRC_SEED                             0xffff
#define EQ_BAND_NUM                             12
#define CAL_FIX(x)		                        ((int)(x * (1 << 27)))

u8 eq_rx_buf[EQ_BUFFER_LEN];
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
eq_dbg_cb_t eq_dbg_cb;
static u8 eq_tx_buf[12];
const char tbl_eq_version[10] = {'E', 'Q', '1', '*', 6, 0, 12, 4, 5, 0};
const char tbl_drc_version[10] = {'D', 'R', '0', '*', 6, 0, 4, 0, 0, 0};
AT(.com_rodata.anc)
const char tbl_anc_header[3] = "ANC";
#if TOOLKIT_AUDIO_DUMP_EN
AT(.com_rodata.btmic_dump)
const char toolkit_btmic_dump_header[7] = "BTMIC";
#endif
void music_set_pt_eq(u8 channel, u8 band_cnt, const u32 *param);
void music_dbb_eq_index_init(u32* coef_l, u32* coef_r);

typedef enum{
    EQ_RES,
    DRC_RES,
}RES_TYPE_SET;

u8 check_sum(u8 *buf, u16 size)
{
    u32 i, sum = 0;
    for (i = 0; i < size; i++) {
        sum += buf[i];
    }
    return (u8)(-sum);
}

void tx_ack(uint8_t *packet, uint16_t len)
{
    delay_5ms(1);   //延时一段时间再ack
    if (eq_dbg_cb.rx_type) {
#if EQ_DBG_IN_UART
        huart_tx(packet, len);
#endif
    } else {
#if EQ_DBG_IN_SPP && BT_SPP_EN
        if (xcfg_cb.eq_dgb_spp_en && (bt_get_status() >= BT_STA_CONNECTED)) {
            bt_spp_tx(SPP_SERVICE_CH0, packet, len);
        }
#endif
    }
    eq_dbg_cb.rx_type = 0;
}

void eq_tx_ack(u8 bank_num, u8 ack, u8 type)
{
    u8 ack_buf[4];
    if (type) {
        ack_buf[0] = 'D';
    } else {
        ack_buf[0] = 'A';
    }
    ack_buf[1] = bank_num;
    ack_buf[2] = ack;
    ack_buf[3] = check_sum(ack_buf, 3);
//    ack_buf[3] = -(ack_buf[0] + ack_buf[1] + ack_buf[2]);
    tx_ack(ack_buf, 4);
}

void eq_tx_version(u8 type)
{
    if (type) {
        memcpy(eq_tx_buf, tbl_drc_version, 10);
    } else {
        memcpy(eq_tx_buf, tbl_eq_version, 10);
    }

    u16 crc = calc_crc(eq_tx_buf, 10, EQ_CRC_SEED);
    eq_tx_buf[10] = crc;
    eq_tx_buf[11] = crc >> 8;
    tx_ack(eq_tx_buf, 12);
}


void bsp_eq_parse_cmd(void)
{
//    RES_TYPE_SET set_type = 0;
//#if TRACE_EN
////    print_r(eq_rx_buf, EQ_BUFFER_LEN);
//#endif
//    if (eq_rx_buf[0] == 'E' && eq_rx_buf[1] == 'Q') {
//        set_type = EQ_RES;
//    } else if (eq_rx_buf[0] == 'D' && eq_rx_buf[1] == 'R') {
//        set_type = DRC_RES;
//    } else {
//        return;
//    }
//    if (eq_rx_buf[2] == '?' && eq_rx_buf[3] == '#') {
//        eq_tx_version(set_type);
//        return;
//    } else if (eq_rx_buf[2] == '0' && eq_rx_buf[3] == ':') {
//        eq_tx_ack(0, 0, set_type);
//        return;
//    } else if (eq_rx_buf[2] == 'S' && (eq_rx_buf[3] == 'P' || eq_rx_buf[3] == 'F')) {
//        return ;
//    } else if (eq_rx_buf[2] == 'S' && (eq_rx_buf[3] == 'M')) {
//#if DAC_PT_EN
//    } else if (eq_rx_buf[2] == 'S' && (eq_rx_buf[3] == 'D') && (eq_rx_buf[4] == 'A') && (eq_rx_buf[5] == 'C')) {
//        u8 band_num = eq_rx_buf[7];                                                 //eq 滤波器数量
//        u8 channel  = eq_rx_buf[8];                                                 //左右声道标志位：1->左声道;2->右声道;3->验双声道效果
//        u32 crc_rx  = calc_crc(eq_rx_buf, (9 + band_num*4*5), EQ_CRC_SEED);
//        TRACE("dac pt rx_buf, band_num [%d], ch [%d], crc [%4x]\n", band_num, channel, crc_rx);
//        if (crc_rx == little_endian_read_16(eq_rx_buf, (9 + band_num*4*5))) {      //crc16校验
//            eq_tx_ack(band_num, 1, set_type);                                      //校验成功，发送ack1
//            TRACE("dac pt rx_buf crc check successfully\n");
//        } else {
//            eq_tx_ack(band_num, 0, set_type);                                      //校验失败，发送ack0
//            TRACE("dac pt rx_buf crc err, correct code :%4x\n", little_endian_read_16(eq_rx_buf, (12 + band_num*4*5)));
//            return;
//        }
//        if (eq_rx_buf[6] == 'T') {                                                  //设置eq参数听效果
//            if (channel == 1 || channel == 2) {
//                eq_tx_ack(band_num, channel, set_type);
//                TRACE("dac pt SDACT cmd ch check successfully\n");
//            } else {
//                eq_tx_ack(band_num, 0, set_type);
//                TRACE("dac pt SDACT cmd ch check err, %d\n", channel);
//                return;
//            }
//            music_set_pt_eq(channel-1, band_num, (u32 *)&eq_rx_buf[9]);
//        } else if (eq_rx_buf[6] == 'R') {                                           //烧录eq参数
//            u8 error_code = 0;
//            if (channel == 1 || channel == 2) {
//                u8 dac_ch = channel - 1;
//                cm_write8(PARAM_DAC_PT_NUM + dac_ch, band_num);
//                cm_write(&eq_rx_buf[9], DAC_PT_PAGE(dac_ch * 120), band_num * 4 * 5);
//                cm_sync();
//                error_code = channel;
//                TRACE("dac pt SDACR cmd cm_write successfully\n");
//            } else if (channel == 3) {
//                plugin_music_eq();
//                error_code = channel;
//                TRACE("dac pt SDACR cmd plugin_music_eq\n");
//            } else {
//                error_code = 4;
//                TRACE("dac pt SDACR cmd err\n");
//            }
//            eq_tx_ack(band_num, error_code, set_type);
//        }
//        return;
//#endif
//    }
//
//    u8 band_num = eq_rx_buf[6];
//    u32 size = little_endian_read_16(eq_rx_buf, 4);
//    u32 crc = calc_crc(eq_rx_buf, size+4, EQ_CRC_SEED);
//    if (crc == little_endian_read_16(eq_rx_buf, 4+size)) {
//        eq_tx_ack(band_num, 0, set_type);
//    } else {
//        eq_tx_ack(band_num, 1, set_type);
//        return;
//    }
//    u8 band_cnt = eq_rx_buf[7];
//
//    {
//        if (set_type) {
////            music_drc_set_by_param(band_cnt, (u32 *)&eq_rx_buf[10]);
//        } else {
//#if BT_TWS_EN
//            if (!bt_tws_is_slave()) {
//                eq_rx_buf[0] = 'E';
//                eq_rx_buf[1] = 'Q';
//                bt_tws_sync_eq_param();
//            }
//#endif
//            music_eq_set_by_param(band_cnt, (u32 *)&eq_rx_buf[14]);
//        }
//    }
//
//#if (UART0_PRINTF_SEL != PRINTF_NONE)
//    u8 k;
//    u32 *ptr = (u32 *)&eq_rx_buf[10];
//    if (set_type) {
//        for (k = 0; k < 10; k++) {
//            printf("%08x", *ptr++);
//            if (k % 5 == 4) {
//                printf("\n");
//            } else {
//                printf(" ");
//            }
//        }
//    } else {
//        printf("%08x\n", little_endian_read_32(eq_rx_buf, 14));
//        for (k = 0; k < band_cnt*5; k++) {
//            printf("%08x", *ptr++);
//            if (k % 5 == 4) {
//                printf("\n");
//            } else {
//                printf(" ");
//            }
//        }
//    }
//#endif
//    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
}

#if BT_TWS_EN
uint16_t tws_get_spp_eq_info(uint8_t *buf)
{
    if (buf != NULL) {
        memcpy(buf, eq_rx_buf, EQ_BUFFER_LEN);
    }
//    printf("get_spp_eq: %d\n", EQ_BUFFER_LEN);
    return EQ_BUFFER_LEN;
}

void tws_set_spp_eq_info(uint8_t *buf, uint16_t len)
{
    if (buf != NULL) {
        memcpy(eq_rx_buf, buf, len);
        if (memcmp(eq_rx_buf, tbl_anc_header, 3) == 0) {
            msg_enqueue(EVT_ONLINE_SET_ANC);
        } else {
            msg_enqueue(EVT_ONLINE_SET_EQ);
        }
    }
 //   printf("set_spp_eq: %d\n", len);
}
#endif

AT(.com_huart.text)
u8 bsp_eq_rx_done(u8* rx_buf)
{
    if (memcmp(rx_buf, tbl_anc_header, 3) == 0) {
//        msg_enqueue(EVT_ONLINE_SET_ANC);
        eq_dbg_cb.rx_type = 1;
    } else if (((rx_buf[0]=='E') && (rx_buf[1]=='Q')) || (rx_buf[0] == 'D' && rx_buf[1] == 'R')) {
//        msg_enqueue(EVT_ONLINE_SET_EQ);
        eq_dbg_cb.rx_type = 1;
    } else if (((rx_buf[0]=='C') && (rx_buf[1]=='F') && (rx_buf[2]=='G'))) {
        msg_enqueue(EVT_ONLINE_SET_EFFECT);
        eq_dbg_cb.rx_type = 1;
    }
    #if TOOLKIT_AUDIO_DUMP_EN
else if(0 == memcmp(rx_buf, toolkit_btmic_dump_header, 5)){
        msg_enqueue(EVT_ONLINE_SET_BTMIC_DUMP);
        eq_dbg_cb.rx_type = 1;
    }
#endif
    return eq_dbg_cb.rx_type;
}

void eq_dbg_init(void)
{
    memset(&eq_dbg_cb, 0, sizeof(eq_dbg_cb_t));
}
#endif

AT(.text.music)
void music_eq_set_by_num(u8 num)
{
    if (num > (MUSIC_EQ_TBL_LEN - 1)) {
        return;
    }

#if EQ_APP_EN
    eq_msc_index_init(0);
#endif

    uint effect_idx = eq_effect_idx_tbl[num];
    u32 res_addr = (u32)effect_res_addr_get(effect_idx);
    u32 res_len  = effect_res_len_get(effect_idx);

    music_eq_set_by_res(res_addr, res_len);
}

#if EQ_MODE_EN
AT(.text.music)
void sys_set_eq(void)
{
    sys_cb.eq_mode++;
    if (sys_cb.eq_mode >= MUSIC_EQ_TBL_LEN) {
        sys_cb.eq_mode = 0;
    }
    music_eq_set_by_num(sys_cb.eq_mode);
    gui_box_show_eq();
}
#endif // EQ_MODE_EN

struct eq_coef_t {
    u32 param0;                         //频点参数，由工具计算
    u32 param1;
    u32 *coef;                          //频段参数，由eq_coef_cal计算
};

#if EQ_APP_EN || DAC_PT_EN

struct eq_div_t {
    uint32_t total_gain;
    u32 coef[DAC_EQ_NUM_TOTAL][5];              //顺序：APP、产测、动态低音
};

#if EQ_APP_EN
static struct eq_coef_t app_coef[EQ_APP_NUM];   //存放每条计算的参数(APP不分左右声道)
#endif
static struct eq_div_t eq_div[2];               //存放所有div eq的参数(左右声道)

//根据index设置12条EQ示例，包含高低音
//Q:0.750000
static const int tbl_coef[12][2] = {
    //param0                    param1
    { CAL_FIX(0.0029444916),    CAL_FIX(0.9999902462)},     //Band:0(31Hz)
    { CAL_FIX(0.0059839058),    CAL_FIX(0.9999597162)},     //Band:1(63Hz)
    { CAL_FIX(0.0151961090),    CAL_FIX(0.9997401793)},     //Band:2(160Hz)
    { CAL_FIX(0.0237409561),    CAL_FIX(0.9993657117)},     //Band:3(250Hz)
    { CAL_FIX(0.0379729998),    CAL_FIX(0.9983764898)},     //Band:4(400Hz)
    { CAL_FIX(0.0946628813),    CAL_FIX(0.9898674722)},     //Band:5(1000Hz)
    { CAL_FIX(0.1874074140),    CAL_FIX(0.9596752251)},     //Band:6(2000Hz)
    { CAL_FIX(0.3597005044),    CAL_FIX(0.8419530754)},     //Band:7(4000Hz)
    { CAL_FIX(0.6057018917),    CAL_FIX(0.4177699622)},     //Band:8(8000Hz)
    { CAL_FIX(0.5060881129),    CAL_FIX(-0.6509365173)},    //Band:9(16000Hz)
    { CAL_FIX(0.0284864965),    CAL_FIX(0.9990866674)},     //BASS:300Hz
    { CAL_FIX(0.2763541250),    CAL_FIX(0.9100351062)},     //TREB:3000Hz
};

void eq_index_init(void)
{
    eq_div[0].total_gain = 0x800000;
    eq_div[1].total_gain = 0x800000;
#if EQ_APP_EN
    for (int i = 0; i < EQ_APP_NUM; i++) {
        app_coef[i].param0 = tbl_coef[i][0];
        app_coef[i].param1 = tbl_coef[i][1];
        app_coef[i].coef = eq_div[0].coef[i];                               //只配置左声道
    }
#endif
    music_eq_divband_init(0, DAC_EQ_NUM_TOTAL, (u32 *)eq_div[0].coef); //DACL
    music_eq_divband_init(1, DAC_EQ_NUM_TOTAL, (u32 *)eq_div[1].coef); //DACR

#if DAC_PT_EN
    for (int i = 0; i < 2; i++) {
        u8 dac_pt_num = cm_read8(PARAM_DAC_PT_NUM + i);
        if (dac_pt_num != 0) {
            cm_read(&(eq_div[i].coef[EQ_APP_NUM][0]), DAC_PT_PAGE(120 * i), dac_pt_num * 5 * 4);
            TRACE("dac pt cm_read param do\n");
        }
    }
#endif
}

void eq_msc_index_init(u8 flag)
{
}

#if DAC_PT_EN
//设置产测EQ
void music_set_pt_eq(u8 channel, u8 band_cnt, const u32 *param)
{
    if (band_cnt > DAC_PT_NUM || band_cnt == 0 || param == NULL) {
        TRACE("dac pt SDADT music_set_pt_eq err, band_cnt %d\n", band_cnt);
        return;
    }
    memcpy(&(eq_div[channel].coef[EQ_APP_NUM][0]), param, band_cnt * 5 * 4);
    TRACE("dac pt SDADT music_set_pt_eq, ch %d, band_cnt %d\n", channel, band_cnt);
    music_eq_set_by_num(sys_cb.eq_mode);
}
#endif

#if EQ_APP_EN
//根据EQ号来设置EQ[11:0]
void music_eq_set_for_index(u8 index, int gain)
{
    if (index >= EQ_APP_NUM || gain < -12 || gain > 12) {
        return;
    }
    printf("index:%d gain:%d\n",index,gain);

    eq_coef_cal(&app_coef[index], gain);
    memcpy(eq_div[1].coef[index], eq_div[0].coef[index], 20);           //左右声道保持一致
}

void music_eq_set_for_index_do(void)
{
    music_eq_set_by_num(sys_cb.eq_mode);
}

static const uint32_t eq_total_gain_tbl[14] = {
    0x287a27, 0x2d6a86, 0x32f52d, 0x392cee, 0x4026e7, 0x47facd, 0x50c336, 0x5a9df8,
    0x65ac8c, 0x721483, 0x800000, 0x8f9e4d, 0xa12478, 0xb4ce08,
};

//设置EQ总增益
void music_eq_set_overall_gain(int gain)
{
    if (gain > 3 || gain < -10) {
        return;
    }
    eq_div[1].total_gain = eq_div[0].total_gain = eq_total_gain_tbl[gain+10];
}

void music_get_eq_total_gain(u32* gain)
{
    if (gain == NULL) {               //msc eq total gain 无效时，填写默认0dB
        *gain = eq_div[0].total_gain;
    }
}
#endif
#endif

#if EQ_SYS_BASS_TREBLE_EN

struct eq_coef_t msc_bass = {
    .param0 = CAL_FIX(0.0284864965),    //BASS:300Hz
    .param1 = CAL_FIX(0.9990866674),    //BASS:300Hz
};

struct eq_coef_t msc_treb = {
    .param0 = CAL_FIX(0.2763541250),    //TREB:3000Hz
    .param1 = CAL_FIX(0.9100351062),    //TREB:3000Hz
};

//struct eq_coef_t mic_bass = {
//    .param0 = CAL_FIX(0.0284864965),    //BASS:300Hz
//    .param1 = CAL_FIX(0.9990866674),    //BASS:300Hz
//    .coef = music_bass_coef,
//};
//
//struct eq_coef_t mic_treb = {
//    .param0 = CAL_FIX(0.2763541250),    //TREB:3000Hz
//    .param1 = CAL_FIX(0.9100351062),    //TREB:3000Hz
//    .coef = music_treb_coef,
//};

//static struct eq_cfg_t eq_cfg = {
//#if EQ_SYS_BASS_TREBLE_EN
//    .msc_coef[10] = &msc_bass,
//    .msc_coef[11] = &msc_treb,
//
//    .mic_coef[6] = &mic_bass,
//    .mic_coef[7] = &mic_treb,
//#endif // EQ_SYS_BASS_TREBLE_EN
//};

void bsp_bass_treble_init(void)
{
//    bass_treble_coef_cal(&mic_bass, 0, 0);
//    bass_treble_coef_cal(&mic_treb, 0, 1);

    msc_bass.coef = (u32*)bsp_music_bass_eq_coef_get();   //初始化band0参数的数组
    msc_treb.coef = (u32*)bsp_music_treb_eq_coef_get();   //初始化band1参数的数组

    bass_treble_coef_cal(&msc_bass, 0, 0);
    bass_treble_coef_cal(&msc_treb, 0, 1);

    print_r32(msc_bass.coef,5);
    print_r32(msc_treb.coef,5);

    if (xcfg_cb.huart_en) {                    //与EQ等在线调节避开
         music_bass_treble_enable(1);                            //打开使能
    } else {
		 music_bass_treble_enable(0);                            //关闭使能
	}
}

//设置MIC高低音（不会影响其他段EQ，可以边调EQ边调高低音）
void mic_bass_treble_set(int mode, int gain)
{
//    if (mode == 0) {            //bass
//        bass_treble_coef_cal(&mic_bass, gain, 0);
//    } else if (mode == 1) {     //treble
//        bass_treble_coef_cal(&mic_treb, gain, 1);
//    }
//#ifdef RES_BUF_EQ_MIC_NORMAL_EQ
//    mic_set_eq_by_res(&RES_BUF_EQ_MIC_NORMAL_EQ, &RES_LEN_EQ_MIC_NORMAL_EQ);    //更新参数
//#endif
}

//设置MUSIC高低音（不会影响其他段EQ，可以边调EQ边调高低音）
void music_bass_treble_set(int mode, int gain)
{
    if (mode == 0) {                            //bass
        bass_treble_coef_cal(&msc_bass, gain, 0);
    } else if (mode == 1) {                     //treble
        bass_treble_coef_cal(&msc_treb, gain, 1);
    }
    music_eq_set_by_num(sys_cb.eq_mode);        //更新参数
}

//音乐低音参数设置
void bsp_music_set_bass(int bass_gain)          //gain:-12dB~12dB
{
    if (xcfg_cb.huart_en) {                      //与EQ等在线调节避开
        return;
    }

    if (bass_gain > 12) {
        bass_gain = 12;
    } else if (bass_gain < -12) {
        bass_gain = -12;
    }

    music_bass_treble_set(0, bass_gain);

    printf("bass_gain:%d \n",bass_gain);
}

//音乐高音参数设置
void bsp_music_set_treble(int treble_gain)      //gain:-12dB~12dB
{
    if (xcfg_cb.huart_en) {                      //与EQ等在线调节避开
        return;
    }

    if (treble_gain > 12) {
        treble_gain = 12;
    } else if (treble_gain < -12) {
        treble_gain = -12;
    }

    music_bass_treble_set(1, treble_gain);

    printf("treble_gain:%d \n",treble_gain);
}

#endif // EQ_SYS_BASS_TREBLE_EN

void bsp_eq_init(void)
{
    eq_var_init();
#if EQ_SYS_BASS_TREBLE_EN
    bsp_bass_treble_init();
#endif // EQ_SYS_BASS_TREBLE_EN
#if EQ_APP_EN || DAC_PT_EN
    eq_index_init();
#endif
}

