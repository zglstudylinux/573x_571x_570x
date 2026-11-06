#include "include.h"
#include "agc.h"
#include "api_alg.h"


#if AGC_EN

#define INFS			    48000		//可调参数，不同采样率数据修改此处即可(默认只做48k)
#define INBIT				16			//输入数据位数，仅支持16位输入
#define COMPRESSIONGAINDB	13			//可调参数，compressionGaindB: default 9 dB，表示音频最大的增益能力，设置为9 dB表示最大可以被提升9 dB
#define TARGETLEVERDBFS		5			//可调参数，targetLevelDbfs: default 3 ,表示音量均衡结果的目标值(3表示音量的目标值为-3 dB)
#define AGCMODE				2			//可调参数，0:kAgcModeUnchanged, 1:kAgcModeAdaptiveAnalog, 2:kAgcModeAdaptiveDigital, 3:kAgcModeFixedDigital

#define FRAME_60			0
#define FRAME_120			1
#define FRAME_480			0
#define FRAME_720			0

#define AGC_INFO_PRINT          0
#define UARTDUMP_AGC_EN         0                         //DUMP数据一键配置，需要使用定制上位机
#define FRAME_LEN		        720                       //算法处理帧长
#define PROCESS_OUT_SAMPLES     120                       //每次存取帧长

static agc_cfg_t agc_cfg AT(.buf.agc);
static agc_cb_t agc_cb AT(.buf.agc);

AT(.text.agc_proc.input)
void agc_audio_input(u8 *ptr, u32 samples, int ch_mode, void *param)
{
    s16 *rptr = (s16 *)ptr;

    AgcProcess(rptr,samples/2);
    AgcProcess(&rptr[60],samples/2);
    if (agc_cfg.callback) {
        agc_cfg.callback((u8 *)rptr, samples, ch_mode, param);
    }
}

AT(.text.front_set.callback)
void agc_audio_output_callback_set(audio_callback_t callback)
{
    agc_cfg.callback = callback;
}


AT(.text.front_init)
void agc_audio_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset((uint8_t *)&agc_cb, 0, sizeof(agc_cb));


	if ((FRAME_60) || (FRAME_120) || (FRAME_480)) {
		agc_cb.frameSh        = 160;
	}
	if (FRAME_720) {
		agc_cb.frameSh        = 240;
	}

	//agc_cb.frameSh_true	   = 60;
	agc_cb.frame_2p5ms_en	   = 0;
	agc_cb.inbits24_en		   = 0;
	agc_cb.sampleHz		       = INFS;
	agc_cb.frameMs             = 10;
	agc_cb.CompressiondB       = COMPRESSIONGAINDB;
	agc_cb.TargetdBfs          = TARGETLEVERDBFS;
	agc_cb.limiterEnable       = 1;
	agc_cb.smooth_en		   = 0;
	agc_cb.agcMode             = AGCMODE;
    agcInit(&agc_cb);
}

#endif
