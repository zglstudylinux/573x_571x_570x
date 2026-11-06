#include "include.h"

#define FS_CRC_SEED         0xffff

uint calc_crc(void *buf, uint len, uint seed);

#if FUNC_MUSIC_EN

//扫描全盘文件
bool pf_scan_music(u8 new_dev)
{
    if (new_dev) {
#if USB_SD_UPDATE_EN
        func_update();                                  //尝试升级
#endif // USB_SD_UPDATE_EN
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = BIT(0);
    sys_cb.rec_num = 0;
    sys_cb.ftime = 0;
#endif // REC_FAST_PLAY

    f_msc.file_total = fs_get_total_files();
    if (!f_msc.file_total) {
        f_msc.dir_total = 0;
        return false;
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = 0;
#endif // REC_FAST_PLAY

    if (new_dev) {
#if MUSIC_PLAYDEV_BOX_EN
        gui_box_show_playdev();
#endif // MUSIC_PLAYDEV_BOX_EN

#if WARNING_USB_SD
        if (sys_cb.cur_dev <= DEV_SDCARD1) {
            bsp_res_play(RES_IDX_SDCARD_MODE);
        } else {
            bsp_res_play(RES_IDX_USB_MODE);
        }
        bsp_res_w4_finish(false);
#endif // WARNING_USB_SD
    }

    f_msc.dir_total = fs_get_dirs_count();          //获取文件夹总数
    return true;
}

#if MUSIC_PLAYMODE_NUM
//music播放模式切换
void music_playmode_next(void)
{
    sys_cb.play_mode++;
    if (sys_cb.play_mode >= MUSIC_PLAYMODE_NUM) {
        sys_cb.play_mode = NORMAL_MODE;
    }
    gui_box_show_playmode();
    plugin_playmode_warning();
}
#endif // MUSIC_PLAYMODE_NUM

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_clr(void)
{
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    param_msc_breakpoint_write();
    param_sync();
    //printf("%s\n", __func__);
}

void bsp_music_breakpoint_init(void)
{
    int clr_flag = 0;
    if (f_msc.brkpt_flag) {
        f_msc.brkpt_flag = 0;
        param_msc_breakpoint_read();
        //printf("restore: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
        if (calc_crc(f_msc.fname, 8, FS_CRC_SEED) == f_msc.brkpt.fname_crc) {
            music_set_jump(&f_msc.brkpt);
        } else {
            clr_flag = 1;
        }
    }
    f_msc.brkpt.fname_crc = calc_crc(f_msc.fname, 8, FS_CRC_SEED);
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    if (clr_flag) {
        param_msc_breakpoint_write();
    }
}

void bsp_music_breakpoint_save(void)
{
    music_get_breakpiont(&f_msc.brkpt);
    param_msc_breakpoint_write();
    param_sync();
    //printf("save: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
}
#endif // MUSIC_BREAKPOINT_EN
#endif // FUNC_MUSIC_EN

