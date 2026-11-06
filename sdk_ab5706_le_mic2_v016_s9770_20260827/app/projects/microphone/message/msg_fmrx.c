#include "include.h"
#include "func.h"
#include "func_fmrx.h"

#if FUNC_FMRX_EN
AT(.text.func.fmrx.msg)
void func_fmrx_message(u16 msg)
{
    switch (msg) {
        case MSG_FM_PLAY_PAUSE:
            sys_cb.mute = 0;
            if (fmrx_cb.sta < FMRX_PLAY) {
                fmrx_cb.sta = FMRX_SEEK_STOP;   //停止搜台
            } else {
                func_fmrx_pause_play();
            }
            break;

        case MSG_FM_AUTO_SEEK:
            sys_cb.mute = 0;
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            if (fmrx_cb.sta == FMRX_PLAY) {
                fmrx_cb.sta = FMRX_SEEK_START;
            } else {
                fmrx_cb.sta = FMRX_SEEK_STOP;
            }
            break;

        case MSG_FM_PREV:
            #if FMRX_TEST_CHANNEL
            fmrx_test_channel_switch(0);
            #else
            fmrx_switch_channel(0);
            //fmrx_switch_freq(0);
            #endif
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            break;

        case MSG_FM_NEXT:
            #if FMRX_TEST_CHANNEL
            fmrx_test_channel_switch(1);
            #else
            fmrx_switch_channel(1);
            //fmrx_switch_freq(1);
            #endif
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            break;

#if FMRX_HALF_SEEK_EN
        case MSG_FM_SEEK_PREV:
            fmrx_half_seek_start(0);
            break;

        case MSG_FM_SEEK_NEXT:
            fmrx_half_seek_start(1);
            break;
#endif

#if FMRX_REC_EN
        case MSG_REC:
            sfunc_record();
            break;
#endif // FMRX_REC_EN

//        case MSG_CHANGE_MODE:
//            printf("MSG_CHANGE_MODE test gain switch\n");
//            fmrx_gain_switch();
//            break;

//        case MSG_CHANGE_MODE:
//            bsp_fmrx_logger_out();
//            break;

#if IR_NUMKEY_EN
        case MSG_KEY_NUM0:
        case MSG_KEY_NUM1:
        case MSG_KEY_NUM2:
        case MSG_KEY_NUM3:
        case MSG_KEY_NUM4:
        case MSG_KEY_NUM5:
        case MSG_KEY_NUM6:
        case MSG_KEY_NUM7:
        case MSG_KEY_NUM8:
        case MSG_KEY_NUM9:
            gui_box_show_inputnum(msg - MSG_KEY_NUM0);
            break;
#endif // IR_NUMKEY_EN

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_FMRX_EN
