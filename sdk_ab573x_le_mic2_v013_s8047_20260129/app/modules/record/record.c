#include "include.h"

#if (MUSIC_REC_FILE_FILTER || REC_FAST_PLAY)
AT(.text.fs.scan)
bool is_mic_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_mic_sfn_exfat, strlen(dir_mic_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_mic_sfn, strlen(dir_mic_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_mic, strlen(dir_path_mic))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

AT(.text.fs.scan)
bool is_fm_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_fm_sfn_exfat, strlen(dir_fm_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_fm_sfn, strlen(dir_fm_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_fm, strlen(dir_path_fm))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

AT(.text.fs.scan)
bool is_aux_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_aux_sfn_exfat, strlen(dir_aux_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_aux_sfn, strlen(dir_aux_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_aux, strlen(dir_path_aux))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

AT(.text.fs.scan)
bool is_bt_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_bt_sfn_exfat, strlen(dir_bt_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_bt_sfn, strlen(dir_bt_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_bt, strlen(dir_path_bt))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

//判断是否录音文件夹
AT(.text.fs.scan)
bool is_record_dir(char *sfn)
{
#if REC_ONE_FOLDER_EN
    if (!memcmp(sfn, dir_path_rec, strlen(dir_path_rec))) {
        return true;
    }
    return false;
#else

#if REC_DIR_LFN
    //录音根目录
    if (!memcmp(sfn, root_dir_path, strlen(root_dir_path))) {
        return true;
    }
#endif // REC_DIR_LFN

    //录音子目录
    if (is_mic_record_dir(sfn) || is_fm_record_dir(sfn) || is_aux_record_dir(sfn) || is_bt_record_dir(sfn)) {
        return true;
    }
    return false;
#endif // REC_ONE_FOLDER_EN
}
#endif // MUSIC_REC_FILE_FILTER

#if OPUS_ENC_EN
void asr_kick_start(void);
void opus_enc_exit(void);
bool opus_enc_init(u32 spr, u32 nch, u32 bitrate);
int opus_enc_frame(s16 *pcm, u8 *packet);
u16 opus_pcm_len(void);
void huart_wait_txdone(void);

#define REC_OBUF_SIZE                0x2000              //ADC PCM缓存BUF SIZE
#define REC_ENC_SIZE                 0xA00               //录音压缩数据缓存BUF SIZE
#define REC_OPUS_MIC_DUMP_EN         0

u8 rec_obuf[REC_OBUF_SIZE] AT(.opus.rec.obuf);
u8 rec_encbuf[REC_ENC_SIZE] AT(.opus.rec.enc);
au_stm_t rec_enc_stm AT(.opus.rec.buf);
au_stm_t rec_pcm_stm AT(.opus.rec.buf);
peri_nr_cfg_t opus_mic_nr_cfg AT(.opus.rec.nr);

typedef struct {
    u8 spr;                 //采样率
    u8 nch;                 //声道数
    u8 packet_size;         //编码后一帧opus packet大小
    bool opus_is_recing;
    u16 bitrate;			//压缩比特率
} opus_cb_t;
static opus_cb_t opus_cb;
u8 opus_pack[40] AT(.opus.rec.buf);
u8 mic_data[640] AT(.opus.rec.buf);

#if REC_OPUS_MIC_DUMP_EN
u8 dump_header[7];
AT(.opus_dump_buf)
u8 huart_buf[256 + 7];
AT(.com_text.opus_dump)
void opus_mic_tx(u8 type, u8* addr, u16 data_len)
{
    //特别说明：type1--5只能是pcm数据，type6必须是opus编码后的数据
    huart_wait_txdone();
    u16 len = 0;
    dump_header[0] = 0x36;
    dump_header[1] = 0xAD;
    dump_header[2] = 0xf9;
    dump_header[3] = 0x54;
    dump_header[4] = (u8)(data_len >> 8);
    dump_header[5] = (u8)(data_len >> 0);
    dump_header[6] = type;
    memcpy(&huart_buf[len],dump_header,7);
    len += 7;
    memcpy(&huart_buf[len],addr,data_len);
    len += data_len;
    huart_tx(huart_buf,len);
}
#endif

void rec_stm_buff_init(void)
{
    memset(&rec_pcm_stm, 0, sizeof(rec_pcm_stm));
    memset(&rec_enc_stm, 0, sizeof(rec_enc_stm));

    rec_pcm_stm.buf = rec_pcm_stm.rptr = rec_pcm_stm.wptr = rec_obuf;
    rec_pcm_stm.size = REC_OBUF_SIZE;

    rec_enc_stm.buf = rec_enc_stm.rptr = rec_enc_stm.wptr = rec_encbuf;
    rec_enc_stm.size = REC_ENC_SIZE;
}

u8 bsp_get_opus_param_packetsize(void)
{
    return opus_cb.packet_size;
}

u8 bsp_get_opus_param_nchannel(void)
{
    return opus_cb.nch;
}

void bsp_opus_rec_mic_start(void)
{
    if (opus_cb.opus_is_recing == true) return;
    printf("%s\n",__func__);
    opus_cb.spr = SPR_16000;
    opus_cb.nch = 1;
    opus_cb.opus_is_recing = false;
    opus_cb.packet_size = 40;
    opus_cb.bitrate = 16000;

    if (opus_enc_init(opus_cb.spr, opus_cb.nch, opus_cb.bitrate)) {
        sys_clk_req(INDEX_ANC, SYS_160M);
        rec_stm_buff_init();
        audio_path_init(AUDIO_PATH_OPUS);
        audio_path_start(AUDIO_PATH_OPUS);
        opus_cb.opus_is_recing = true;
        printf("opus_enc_init ok\n");
    } else {
        printf("opus_enc_init fail\n");
    }

}

void bsp_opus_rec_mic_stop(void)
{
    if (opus_cb.opus_is_recing == false) return;
    printf("%s\n",__func__);
    audio_path_exit(AUDIO_PATH_OPUS);
    opus_enc_exit();
    opus_cb.opus_is_recing = false;
    sys_clk_free(INDEX_ANC);
}

AT(.com_text.opus.sta)
bool bsp_opus_rec_is_working(void)
{
    return opus_cb.opus_is_recing;
}

AT(.com_text.opus.rec)
void opus_sdadc_process(u8 *ptr, u32 samples, u32 pcm_mode)
{
#if REC_OPUS_MIC_DUMP_EN
    opus_mic_tx(1, ptr, samples<<1); //dump mic原始数据
#endif
    puts_stm_buf(&rec_pcm_stm, ptr, samples<<1);
    if (rec_pcm_stm.len >= opus_pcm_len() * 2) {
        asr_kick_start();
    }
}

//借用asr线程
void asr_kws_process(void)
{
    if (gets_stm_buf(&rec_pcm_stm, mic_data, opus_pcm_len() * 2)) {
        u16 len = opus_enc_frame((s16*)mic_data,opus_pack);
        puts_stm_buf(&rec_enc_stm, opus_pack, len);
        #if REC_OPUS_MIC_DUMP_EN
        opus_mic_tx(6, opus_pack, len); //dump opus编码后的
        #endif
    }
}

u16 bsp_get_opus_data(u8* buf, u16 len)
{
    if (gets_stm_buf(&rec_enc_stm, buf, len)) {
        return len;
    } else {
        return 0;
    }
}

#else
AT(.com_text.opus.rec)
void opus_sdadc_process(u8 *ptr, u32 samples, u32 pcm_mode){}
#endif
