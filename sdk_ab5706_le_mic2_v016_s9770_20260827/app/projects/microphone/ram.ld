//-----------------------------------------------------------------------------------
//  Created by zoro on 2024-5-22.
//
//  代码、常量、变量分段命名规则
//
//  格式: .seg.module.xxx
//      [seg    ] 声明段功能，例如.text, .bss, .data等
//      [module ] 声明所属模块，例如.text.dac, .text.adc等
//      [xxx    ] 声明模块功能，例如.text.dac.init, .text.dac.exit, .text.dac.proc.v1
//
//  [seg] 默认规则
//  .com_text.module.xxx        公共区代码段
//  .com_rodata.module.xxx      公共区常量
//  .text.module.xxx            flash代码段（默认放flash，也可以在ram.ld指定到RAM）
//  .rodata.module.xx           flash常量（默认放flash，也可以在ram.ld指定到RAM）
//  .data.module.xx             有初值的变量（公共区）
//  .bss.module.xx              无初值的变量（公共区）
//  .buf.module.xx              无初值的buffer（默认放公共区，也可以在ram.ld指定RAM空间）
//-----------------------------------------------------------------------------------

#include "config.h"

ENTRY(_start)

//Define the flash memory
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

//Define the sram memory
__sram_vma          = 0x00010000;
__sram_size         = 164k;
__dram_vma          = 0x100000;
__dram_size         = 12k;
__eram_vma          = 0x39000;
__eram_size         = 4k;

// Define the usage of sram
__cache_stack_vma   = 0x10600;  //for loader
__cache_stack_size  = 512;      //for loader
__stack_vma         = 0x10800;
__stack_ram_size    = (1k+256);
__comm_vma          = __stack_vma + __stack_ram_size;

__comm_ram_size     = 60k;      //根据实际调整大小，尽量按512对齐
__cram_vma          = __comm_vma + __comm_ram_size;
__cram_size         = __sram_vma + __sram_size - __cram_vma;


//-----------------------------------------------------------------------------------
//msc       :   msc_overlap     |   msc_comm                                    |
//adapter   :   adapter_comm    |   lc3_dec     |   echo/magic...   |   usbdev  |
//device    :   device_comm     |   lc3_enc     |   echo/magic...   |           |

//cram复用段
//1.1 MUSIC_IO(MP3/WAV)
__msc_overlap_vma          = __cram_vma;
__msc_overlap_size         = __cram_size;

//1.2 WIRELESS_MIC(ADAPTER)
__adapter_overlap_vma      = __cram_vma;
__adapter_overlap_size     = __cram_size;

//1.3 WIRELESS_MIC(DEVICE)
__device_overlap_vma       = __cram_vma;
__device_overlap_size      = __cram_size;


MEMORY
{
    init                : org = __base,                 len = 512
    flash(rx)           : org = __base + 512,           len = __max_flash_size
    comm(rx)            : org = __comm_vma,             len = __comm_ram_size
    stack               : org = __stack_vma,            len = __stack_ram_size
    boot                : org = __cache_stack_vma,      len = __cache_stack_size

    dram                : org = __dram_vma,             len = __dram_size

    //ADAPTER
    adapter_comm(rx)    : org = __adapter_overlap_vma,  len = __adapter_overlap_size
    //DEVICE
    device_comm(rx)     : org = __device_overlap_vma,   len = __device_overlap_size
    //MUSIC
    msc_comm(rx)        : org = __msc_overlap_vma,      len = __msc_overlap_size
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .data_cpu (NOLOAD) : {
        *(.buf.cpu_regs)
    } > boot

    .code_comm : {
        __vertor_vma = .;
        *(.vector)
        *(.com_text*)
        *(.com_huart*)
        *debug.o(.rodata*)
        *(.com_rodata*)
        __code_start_piano = .;
        *(.piano_com*)
        __code_end_piano = .;
        *(.data*)
        *(.sdata*)
        *(.plt)
        . = ALIGN(4);
    } > comm AT > flash

//-----------------------------------------------------------------------------------
//MUSIC
    //mp3
    .code_mp3_dec __msc_overlap_vma : {
        __code_start_mp3dec = .;
#if !WARNING_MP3_RES_EN
        *(.mp3dec*)
        . = ALIGN(4);
#endif
        __code_end_mp3dec = .;
    } > msc_comm AT > flash

    .data_mp3_dec __code_end_mp3dec (NOLOAD) : {
#if !WARNING_MP3_RES_EN
        __data_start_mp3dec = .;
        *(.mp3buf.dec)
        *(.mp3_id3_buf)
        __data_end_mp3dec = .;
#endif

//        . = ALIGN(4);
        __msc_mp3_overlap_end = .;
    } > msc_comm

    .dram_mp3 __dram_vma (NOLOAD) : {
        *(.mp3_decram_buf)
    } > dram

    //wav
    .code_wav_dec __msc_overlap_vma : {
        __code_start_wavdec = .;
        *(.wavdec*)
        __code_end_wavdec = .;

        . = ALIGN(4);
        __msc_wav_overlap_end = .;
    } > msc_comm AT > flash

    .dram_wav __dram_vma (NOLOAD) : {
        *(.wavbuf.dec)
    } > dram

    //计算msc最大的复用vma地址
    __msc_comm_vma = MAX(__msc_mp3_overlap_end, __msc_wav_overlap_end);

    .data_music __msc_comm_vma (NOLOAD) : {
        *(.avio_buf)
        *(.diskio_buf)
        *(.pff.buf)
        *(.fname.buf)
        *(.pff_buf*)
        *(.pff.scan.tempbuf)
        *(.fnav_buf)
        *(.lrc_buf*)
        *(.stm_buf)

        *(.karaok.rec)
    } > msc_comm

//-----------------------------------------------------------------------------------
//TEST
    .data_ota __dram_vma (NOLOAD) : {
        *(.fot_data.buf)
        *(.upd_ota*)
//        *(.upd_buf*)
    } > dram

    .data_fcc __dram_vma (NOLOAD) : {
        *(.fcc_buf.huart)
        *(.test_buf)
    } > dram


    .dram_tone __dram_vma (NOLOAD) : {
        *(.wsbc_buf*)
        *(.tone_buf*)
    } > dram

//-----------------------------------------------------------------------------------
//ADAPTER
    .usb_comm_adapter __dram_vma : {
  #if !(WARNING_MP3_RES_EN || WARNING_WSBC_RES_EN)
        //没开MP3/WSBC提示音时，放到DRAM
        __code_start_usbdev = .;
        *(.usbdev.com*)
        . = ALIGN(4);
        __code_end_usbdev = .;
  #endif
    } > dram AT > flash

    .code_adapter : {
#if (WARNING_MP3_RES_EN || WARNING_WSBC_RES_EN)
        //打开MP3/WSBC提示音时，放到ADAPTER
        __code_start_usbdev = .;
        *(.usbdev.com*)
        . = ALIGN(4);
        __code_end_usbdev = .;
#endif
//        *(.text.adapter.wl_rx*)
//        *(.text.adapter.wl_tx*)
//        *(.text.adapter.mic_dec*)
//        *(.com_text.ble.wl_pscan*)
//        *(.com_text.ble.wl_page*)
//        *(.com_text.ble.t2r*)
//        *(.com.text.dac2_out*)

#if !DEVICE_INTERPHONE_EN
#if WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3S
        __code_start_lc3dec = .;
        *(.text.m_lc3_dec*)
        *(.rodata.m_lc3_dec*)
        . = ALIGN(4);
        __code_end_lc3dec = .;
#elif WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3B
#if FUNC_ADAPTER_EN
       __code_start_lc3com = .;
        *(.text.lc3_com*)
        *(.rodata.lc3_com*)
        *(.rodata.lc3)
        . = ALIGN(4);
        __code_end_lc3com = .;
#endif // FUNC_ADAPTER_EN
        __code_start_lc3dec = .;
        *(.text.lc3_dec*)
        *(.rodata.lc3_dec*)
        . = ALIGN(4);
        __code_end_lc3dec = .;
#endif // WIRELESS_CON_CODEC_SEL
#endif
        __code_start_src = .;
        *(.text.src_proc*)
        __code_end_src = .;

        __code_start_alg_trans = .;
        *(.text.alg_prio_trans*)
        __code_end_alg_trans = .;

        __code_start_dnr_track = .;
        *(.text.dnr_noise_track_proc*)
        *(.rodata.dnr_noise_track*)
        . = ALIGN(4);
        __code_end_dnr_track = .;
#if !DEVICE_INTERPHONE_EN
        __code_start_plc = .;
        *(.text.plc_proc*)
        *(.rodata.plc*)
        . = ALIGN(4);
        __code_end_plc = .;
#endif

#if ADAPTER_HOWLING_DNN_EN
        __code_start_howling_dnn = .;
        *(.text.howling_dnn_proc*)
        *(.rodata.howling_dnn*)
        . = ALIGN(4);
        __code_end_howling_dnn = .;
#endif

#if ADAPTER_ECHO_EN
        __code_start_echo = .;
        *(.text.echo_proc*)
        *(.rodata.echo*)
        . = ALIGN(4);
        __code_end_echo = .;
#endif

#if ADAPTER_MAGIC_EN
        __code_start_pitch = .;
        *(.text.pitch_shift_proc*)
        *(.rodata.pitch_shift*)
        . = ALIGN(4);
        __code_end_pitch = .;
#endif

#if ADAPTER_ROOM_REVERB_EN
        __code_start_room_reverb = .;
        *(.text.room_reverb_proc*)
        *(.rodata.room_reverb*)
        __code_end_room_reverb = .;
#endif

        __code_start_agc = .;
        *(.text.agc_proc*)
        *(.rodata.agc*)
        __code_end_agc = .;

        __code_start_drc_v3 = .;
        *(.text.drc_v3_proc*)
        *(.rodata.drc_v3_proc*)
        . = ALIGN(4);
        __code_end_drc_v3 = .;

#if ADAPTER_DNR_FRE_EN
        __code_start_dnr_fre = .;
        *(.text.dnr_fre_proc*)
        *(.rodata.dnr_fre*)
        __code_end_dnr_fre = .;
#endif

#if ADAPTER_MIC_AINS4_EN
        __code_start_ains4 = .;
        *(.text.ains4_proc*)
        *(.rodata.ains4*)
        __code_end_ains4 = .;
#endif
        *(.text.mic_mix_proc*)

        *(.text.karaok_proc*)

#if ADAPTER_MIC_DNN_L2_EN
        __code_start_dnn_L2 = .;
        *(.text.dnn_L2_proc*)
        *(.rodata.dnn_L2*)
        *(.rodata1.dnn_L2*)
        *(.rodata2.dnn_L2*)
//        *(.rodata3.dnn_L2*) //gru
        __code_end_dnn_L2 = .;
#endif

#if ADAPTER_HUART_OUTPUT_EN || ADAPTER_HUART_INPUT_EN
        __code_huart_output = .;
        *(.text.huart*)
        *(.rodata.huart*)
        __code_huart_output = .;
#endif // ADAPTER_HUART_OUTPUT_EN

#if ADAPTER_MIC_DNN_L3_EN
        __code_start_dnn_L3 = .;
        *(.text.dnn_L3_proc*)
        *(.rodata.dnn_L3*)
        *(.rodata1.dnn_L3*)
        *(.rodata2.dnn_L3*)
//        *(.rodata3.dnn_L3*) //gru
        __code_end_dnn_L3 = .;
#endif

#if ADAPTER_MIC_DNR_EN
        __code_start_mic_dnr = .;
        *(.text.mic_dnr_proc*)
        *(.rodata.mic_dnr*)
        __code_end_mic_dnr = .;
#endif

#if WARNING_MP3_RES_EN
        __code_start_mp3dec = .;
        *(.mp3dec*)
        . = ALIGN(4);
        __code_end_mp3dec = .;
#endif // WARNING_MP3_RES_EN

        . = ALIGN(4);
    } > adapter_comm AT > flash

    .data_adapter (NOLOAD) : {
        __buf_start_usbdev = .;
        *(.buf.usb*)
        *(.udev_buf*)
        *(.usb_buf*)
        *(.ude.aubuf)
        __buf_end_usbdev = .;

#if !DEVICE_INTERPHONE_EN
        __buf_start_lc3dec = .;
        *(.buf.lc3_dec*)
        __buf_end_lc3dec = .;

        __buf_start_plc = .;
        *(.buf.plc_buf*)
        __buf_end_plc = .;
#endif
        __buf_start_src = .;
        *(.buf.src*)
        __buf_end_src = .;

        __buf_start_alg_trans = .;
        *(.buf.alg_prio_trans*)
        __buf_end_alg_trans = .;

        __buf_start_dnr_track = .;
        *(.buf.dnr_noise_track*)
        __buf_end_dnr_track = .;

        __buf_start_pacc_voice = .;
        *(.buf.pacc_voice*)
        __buf_end_pacc_voice = .;

#if ADAPTER_HUART_OUTPUT_EN || ADAPTER_HUART_INPUT_EN
        __buf_huart = .;
        *(.buf.huart*)
        __buf_huart = .;
#endif // ADAPTER_HUART_OUTPUT_EN

#if ADAPTER_ECHO_EN
        __buf_start_echo = .;
        *(.buf.echo*)
        __buf_end_echo = .;
#endif

#if ADAPTER_MAGIC_EN
        __buf_start_pitch = .;
        *(.buf.pitch_shift*)
        __buf_end_pitch = .;
#endif

#if ADAPTER_ROOM_REVERB_EN
        __buf_start_room_reverb = .;
        *(.buf.room_reverb*)
        *(.buf1.room_reverb*)
        __buf_end_room_reverb = .;
#endif


        __buf_start_agc = .;
        *(.buf.agc*)
        __buf_end_agc = .;

        __buf_start_mic_mix = .;
        *(.mic_mix_buf*)
        *(.buf.mic_mix*)
        __buf_end_pitch_mic_mix = .;
#if ADAPTER_HOWLING_DNN_EN
        __buf_start_howling_dnn = .;
        *(.buf.howling_dnn*)
        __buf_end_howling_dnn = .;
#endif

#if ADAPTER_LOCAL_MIC_EN
        *(.buf.karaok*)
        *(.buf.mic_effect*)
#endif

#if ADAPTER_DNR_FRE_EN
        __buf_start_dnr_fre = .;
        *(.buf.dnr_fre*)
        __buf_end_dnr_fre = .;
#endif

#if ADAPTER_MIC_AINS4_EN
        __buf_start_ains4 = .;
        *(.buf.ains4*)
        __buf_end_ains4 = .;
#endif

#if I2S_EN
        __buf_start_i2s = .;
        *(.i2s_buf*)
        __buf_end_i2s = .;
#endif

#if ADAPTER_MIC_DNN_L2_EN
        __buf_start_dnn_L2 = .;
        *(.buf.dnn_L2*)
        __buf_end_dnn_L2 = .;
#endif

#if ADAPTER_MIC_DNN_L3_EN
        __buf_start_dnn_L3 = .;
        *(.buf.dnn_L3*)
        __buf_end_dnn_L3 = .;
#endif

#if ADAPTER_MIC_DNR_EN
        __buf_start_mic_dnr = .;
        *(.buf.mic_dnr*)
        __buf_end_mic_dnr = .;
#endif

#if WARNING_MP3_RES_EN
        __buf_start_mp3dec = .;
        *(.mp3buf.dec)
        *(.mp3_id3_buf)
        __buf_end_mp3dec = .;
#endif // WARNING_MP3_RES_EN

        *(.buf.adapter*)

    } > adapter_comm

    .code_device : {
        __code_start_lc3com = .;
        *(.text.lc3_com*)
        *(.rodata.lc3)
        *(.rodata.lc3_com*)
        . = ALIGN(4);
        __code_end_lc3com = .;

        __code_start_lc3enc = .;
        *(.text.lc3_enc*)
        *(.rodata.lc3_enc*)
        __code_end_lc3enc = .;

#if DEVICE_INTERPHONE_EN
        __code_start_lc3dec = .;
        *(.text.m_lc3_dec*)
        *(.rodata.m_lc3_dec*)
        . = ALIGN(4);
        __code_end_lc3dec = .;

        __code_start_plc = .;
        *(.text.plc_proc*)
        *(.rodata.plc*)
        . = ALIGN(4);
        __code_end_plc = .;
#endif



        __code_start_drc_v3 = .;
        *(.text.drc_v3_proc*)
        *(.rodata.drc_v3_proc*)
        . = ALIGN(4);
        __code_end_drc_v3 = .;

#if WIRELESS_MIC_EQ_DRC_EN
        __code_start_mic_eq_drc = .;
        *(.text.mic_eq_drc_proc*)
        __code_end_mic_eq_drc = .;
#endif

#if WIRELESS_MIC_DNR_FRE_EN
        __code_start_dnr_fre = .;
        *(.text.dnr_fre_proc*)
        *(.rodata.dnr_fre*)
        __code_end_dnr_fre = .;
#endif

#if WIRELESS_MIC_MAGIC_EN
        __code_start_pitch = .;
        *(.text.pitch_shift2_proc*)
        *(.rodata.pitch_shift*)
        . = ALIGN(4);
        __code_end_pitch = .;
#endif

#if WIRELESS_MIC_AINS4_EN
        __code_start_ains4 = .;
        *(.text.ains4_proc*)
        *(.rodata.ains4*)
        __code_end_ains4 = .;
#endif // WIRELESS_MIC_AINS4_EN

#if WIRELESS_MIC_ECHO_EN
        __code_start_echo = .;
        *(.text.echo_proc*)
        *(.rodata.echo*)
        . = ALIGN(4);
        __code_end_echo = .;
#endif

#if WIRELESS_MIC_DNN_L2_EN
        __code_start_dnn_L2 = .;
        *(.text.dnn_L2_proc*)
        *(.rodata.dnn_L2*)
        *(.rodata1.dnn_L2*)
        *(.rodata2.dnn_L2*)
//        *(.rodata3.dnn_L2*) //gru
        __code_end_dnn_L2 = .;
#endif

#if WIRELESS_MIC_DNN_L3_EN
        __code_start_dnn_L3 = .;
        *(.text.dnn_L3_proc*)
        *(.rodata.dnn_L3*)
        *(.rodata1.dnn_L3*)
        *(.rodata2.dnn_L3*)
//        *(.rodata3.dnn_L3*) //gru
        __code_end_dnn_L3 = .;
#endif

#if WIRELESS_MIC_DNR_EN
        __code_start_mic_dnr = .;
        *(.text.mic_dnr_proc*)
        *(.rodata.mic_dnr*)
        __code_end_mic_dnr = .;
#endif

#if WIRELESS_MIC_SOFT_VOL_EN
        __code_start_mic_soft_gain = .;
        *(.text.mic_soft_gain*)
        *(.rodata.mic_soft_gain*)
        __code_end_mic_soft_gain = .;
#endif

        . = ALIGN(4);
    } > device_comm AT > flash

    .data_device (NOLOAD) : {
        __buf_start_lc3enc = .;
        *(.buf.lc3_enc*)
        __buf_end_lc3enc = .;
#if DEVICE_INTERPHONE_EN
        __buf_start_lc3dec = .;
        *(.buf.lc3_dec*)
        __buf_end_lc3dec = .;

        __buf_start_plc = .;
        *(.buf.plc_buf*)
        __buf_end_plc = .;
#endif // DEVICE_INTERPHONE_EN

        *(.buf.device_mic*)
#if WIRELESS_MIC_MAGIC_EN
        __buf_start_pitch = .;
        *(.buf.pitch_shift*)
        __buf_end_pitch = .;
#endif

#if WIRELESS_MIC_EQ_DRC_EN
        *(.buf.mic_effect*)
	*(.buf.mic_eq_drc*)
#endif

#if WIRELESS_MIC_DNR_FRE_EN
        __buf_start_dnr_fre = .;
        *(.buf.dnr_fre*)
        __buf_end_dnr_fre = .;
#endif

#if WIRELESS_MIC_AINS4_EN
        __buf_start_ains4 = .;
        *(.buf.ains4*)
        __buf_end_ains4 = .;
#endif

#if WIRELESS_MIC_ECHO_EN
        __buf_start_echo = .;
        *(.buf.echo*)
        __buf_end_echo = .;
#endif

#if WIRELESS_MIC_DNN_L2_EN
        __buf_start_dnn_L2 = .;
        *(.buf.dnn_L2*)
        __buf_end_dnn_L2 = .;
#endif

#if WIRELESS_MIC_DNN_L3_EN
        __buf_start_dnn_L3 = .;
        *(.buf.dnn_L3*)
        __buf_end_dnn_L3 = .;
#endif

#if WIRELESS_MIC_DNR_EN
        __buf_start_mic_dnr = .;
        *(.buf.mic_dnr*)
        __buf_end_mic_dnr = .;
#endif


    } > device_comm

//-----------------------------------------------------------------------------------
    .flash : {
        . = ALIGN(512);

        __code_start_stream = .;
        *(.com_stream.text*)
        . = ALIGN(512);
        __code_end_stream = .;

        __code_start_ws_com = .;
        *(.text.adapter.wl_rx*)
        *(.text.adapter.wl_tx*)
        *(.text.adapter.mic_dec*)
        *(.text.ble.t2r*)
        *(.text.ble.isr.t2r*)
        *(.text.ble.wl_page*)
        *(.text.ble.wl_pscan*)
        . = ALIGN(512);
        __code_end_ws_com = .;

        __code_start_sleep = .;
        *(.saradc_text*)
        *(.sleep_text*)
        . = ALIGN(512);
        __code_end_sleep = .;

        __code_start_charge = .;
        *(.rodata.charge*)
        *(.text.charge*)
        *(.text.charge_box)
        *(.text.bsp.tkey)
        . = ALIGN(512);
        __code_end_charge = .;

        __code_start_pwrsave = .;
        *(.text.pwroff*)
        . = ALIGN(512);
        __code_end_pwrsave = .;

        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        __code_start_cbuf = .;
        *(.cbuf_text_comm)
        . = ALIGN(512);
        __code_end_cbuf = .;

        __code_start_fota = .;
        *(.text.fot.cache*)
        . = ALIGN(512);
        __code_end_fota = .;

        *(.text.aligned.32)
        *(.text.aligned.64)
        *(.text.aligned.128)
        *(.text.aligned.256)
        *(.text.aligned.512)
        . = ALIGN(512);

        __code_start_bts = .;
        *(.text.stk.tws.txreq*)
        *(.text.stk.l2cap.txreq*)
//        . = ALIGN(512);
        *(.text.stk.tws.send*)
//        . = ALIGN(512);
        *(.text.stk.tws.recv*)
//        . = ALIGN(512);
        *(.text.stk.hci.tx*)
//        . = ALIGN(512);
        *(.text.bts.hci.rx*)
//        . = ALIGN(512);
        *(.text.stk.bt.acl.tx*)
        . = ALIGN(512);
        __code_end_bts = .;

        *(.irq_init.aligned)
        *(.text.qtest.process)
        *(.text.func.process)
        *(.text.charge_box.process)
        *(.text.ab_mate.process)
        *(.text.func.bt.process)
        *(.text.warning.proc*)
        *(.text.ble.chmap*)

        *(.text*)
        *(.rodata*)
        *(.srodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);

    } > flash

    .data_comm (NOLOAD) : {
        . = ALIGN(4);
        __heap_start = .;
        *(.mem_heap)
        __heap_end = .;

        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)

        *(.btmem.stack.sdp)
        *(.btmem.stack.spp)
        *(.btmem.stack.l2cap)
        *(.btmem.stack.hid*)
        *(.qtest_buf)

        *(.dac_obuf*)
        *(.adc_buf*)
        *(.tkey_buf.debug)
        *(.au0out_buf*)
        *(.sco_enc.obuf)

        ///bram
        . = ALIGN(4);
        *(.btmem.bthw)
        *(.btmem*)

        *(.ble_cache*)
        *(.ble_buf*)
        *(.gfps*)
        *(.app.buf*)
        *(.fot_data.com*)
        __bss_end = .;
    } > comm
    __bss_size  = __bss_end - __bss_start;
    __heap_size = __heap_end - __heap_start;
}

//stack
__irq_stack_start = __stack_vma;
__irq_stack_end = __stack_vma + __stack_ram_size;
__irq_stack_size = __stack_ram_size;

//Calc the lma
__bank_size = SIZEOF(.flash);
__comm_lma  = LOADADDR(.code_comm);
__comm_size = SIZEOF(.code_comm);
__data_size = SIZEOF(.data_comm);

//MUSIC
__code_mp3_dec_vma      = __msc_overlap_vma;
__code_mp3_dec_lma      = LOADADDR(.code_mp3_dec);
__code_mp3_dec_size     = SIZEOF(.code_mp3_dec);

//WIRELESS_MIC
__code_adapter_vma      = __adapter_overlap_vma;
__code_adapter_lma      = LOADADDR(.code_adapter);
__code_adapter_size     = SIZEOF(.code_adapter);
__data_adapter_size     = SIZEOF(.data_adapter);
__code_device_vma       = __device_overlap_vma;
__code_device_lma       = LOADADDR(.code_device);
__code_device_size      = SIZEOF(.code_device);
__data_device_size      = SIZEOF(.data_device);

//usb
__code_usb_vma          = __dram_vma;
__code_usb_lma          = LOADADDR(.usb_comm_adapter);
__code_usb_size         = SIZEOF(.usb_comm_adapter);
__usb_buf_size          = __buf_end_usbdev - __buf_start_usbdev;
