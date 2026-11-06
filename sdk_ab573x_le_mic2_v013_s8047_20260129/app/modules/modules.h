#ifndef __MODULES_H
#define __MODULES_H

#include "key_scan.h"
#include "audio/mic.h"
#include "audio/audio.h"
#include "audio/mic_mix_mic0.h"
#include "audio/bsp_adc.h"
#include "bluetooth/bluetooth.h"

#include "wireless/wireless.h"

#include "gui/gui.h"
//#include "fs/fs.h"

#include "charge/charge.h"
#include "charge/charge_box.h"

#include "warning/warning_play.h"
#include "warning/res_play.h"

#include "record/record.h"

#include "test/iodm.h"
#include "test/qtest.h"
#include "test/vusb_test.h"

#include "usb_device/usb_device.h"
#include "usb_device/usb_audio.h"
#include "usb_device/usb_enum.h"
#include "usb_device/usb_table.h"


///“Ù¿÷¥¶¿Ì
//#include "effect/music_effect.h"
#include "effect/effect_idx.h"
#include "effect/mic_effect.h"

#include "debug/offline_log.h"
#include "debug/debug.h"

#include "tool/toolkit.h"
#include "tool/toolkit_effect.h"
#include "tool/dump_buf.h"

#include "voice/howling_dnn.h"
#include "voice/dnn_L1.h"
#include "voice/echo.h"
#include "voice/dnn_L2.h"
#include "voice/magic.h"
#include "voice/dnn_L3.h"
#include "voice/dnn_L3_32k.h"
#include "voice/room_reverb.h"
#include "voice/dnr_fre.h"
#include "voice/user_alg.h"
#include "voice/ylcrn_L1_32k.h"
#include "voice/ains4.h"

#include "gui/led/led.h"

//iis
#include "i2s_audio/i2s_audio.h"
#include "i2s_audio/bsp_i2s.h"
#endif // __MODULES_H
