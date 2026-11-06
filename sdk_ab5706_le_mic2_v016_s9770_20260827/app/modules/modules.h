#ifndef __MODULES_H
#define __MODULES_H

#include "key/key_scan.h"
#include "audio/audio.h"
#include "audio/mic.h"
#include "audio/mic_mix.h"
#include "audio/mic_dnr.h"
#include "audio/mic_eq_drc.h"
#include "bluetooth/bluetooth.h"
#include "wireless/wireless.h"

#include "gui/gui.h"
#include "fs/fs.h"

#include "charge/charge.h"

#include "music/bsp_id3_tag.h"
#include "music/bsp_karaok.h"
#include "music/bsp_lrc.h"
#include "music/bsp_music.h"

#include "warning/warning_play.h"
#include "warning/res_play.h"

#include "record/record.h"

#include "test/iodm.h"
#include "test/qtest.h"

#include "usb_device/usb_audio.h"
#include "usb_device/usb_enum.h"
#include "usb_device/usb_table.h"
#include "tool/dump_buf.h"

///人声处理
#include "voice/howling_dnn.h"
#include "voice/echo.h"
#include "voice/magic.h"
#include "voice/ains4.h"
#include "voice/dnn_L2.h"
#include "voice/dnn_L3.h"
#include "voice/agc.h"
#include "voice/room_reverb.h"
#include "voice/dnr_fre.h"

#include "tool/toolkit.h"
#include "tool/toolkit_effect.h"
#include "tool/toolkit_audio_dump.h"
#include "effect/effect_idx.h"
#include "effect/music_effect.h"
#include "effect/mic_effect.h"

#include "wireless/wireless_rf.h"
#include "wireless/wireless_coexist.h"

#include "i2s_audio/i2s_audio.h"
#include "huart_audio/huart_audio_in_mix.h"
#include "huart_audio/huart_audio_out.h"
#include "rtc/rtc_clock.h"

#include "uart/uart_command.h"
#include "uart/command_sync.h"

#endif // __MODULES_H
