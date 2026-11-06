#ifndef __API_SRC_H
#define __API_SRC_H

typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int ch_mode, void *param);
/********************************** src0_adjust_speed(int phase) ***********************************************
调速设置，比原来快or慢phase/(fs_in/fs_out * 1920 * 2^12)
phase>0,数据读取速度调快，比原来快phase/(fs_in/fs_out * 1920 * 2^12)，phase<0,数据读取速度调慢，比原来慢phase/(fs_in/fs_out * 1920 * 2^12)
例：fs_in=48k,fs_out=48k,inlen=120,phase=65536，phase/(48k/48k*1920*2^12)=1/120，比原来快1/120（每120个点后多1个点）
例：fs_in=48k,fs_out=48k,inlen=120,phase=-65536，-phase/(48k/48k*1920*2^12)=-1/120，比原来慢1/120（每120个点后少1个点）
******************************************************************************************************************/

void src0_init(u8 sample_rate, u16 samples, u8 channel);
void src0_stereo_init(u8 sample_rate, u16 samples, u8 channel);
void src0_audio_input(u8 *buf, u32 samples, int nch, void *params);
void src0_audio_output_callback_set(audio_callback_t callback);
void src0_adjust_speed(int speed);
void src0_stop(void);

void src1_init(u8 sample_rate, u16 samples, u8 channel);
void src1_stereo_init(u8 sample_rate, u16 samples, u8 channel);
void src1_audio_input(u8 *buf, u32 samples, int nch, void *params);
void src1_audio_output_callback_set(audio_callback_t callback);
void src1_adjust_speed(int speed);
void src1_stop(void);

#endif