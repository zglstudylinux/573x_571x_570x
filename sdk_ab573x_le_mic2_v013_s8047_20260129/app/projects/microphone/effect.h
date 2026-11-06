#ifndef _EFFECT_H
#define _EFFECT_H


#define BUF_OFS_CFG_MIC_EQ           		(0x00000016)
#define BUF_LEN_OFS_CFG_MIC_EQ       		(0x00000012)
#define SHOWBUF_OFS_CFG_MIC_EQ       		(0x00000016)
#define SHOWBUF_LEN_OFS_CFG_MIC_EQ   		(0x00000032)

#define BUF_OFS_CFG_MIC_DRC          		(0x0000002A)
#define BUF_LEN_OFS_CFG_MIC_DRC      		(0x00000026)
#define SHOWBUF_OFS_CFG_MIC_DRC      		(0x0000002A)
#define SHOWBUF_LEN_OFS_CFG_MIC_DRC  		(0x00000036)



#define ALL_MODE_NAME_SUM            		 0x000003DF


//EFFECT_MODE_IDX
enum {
	CFG_MIC_EQ = 0,
	CFG_MIC_DRC,
	CFG_MAX,
};


typedef struct {
	char effect_cfg_name[12];
	u32  effect_res_offset;
	u32  effect_len_offset;
} effect_info_cfg_t;

extern const effect_info_cfg_t effect_info[CFG_MAX];

typedef struct {
	void (*effect_update_callback)(u8 *buf, u32 len, u8 params);//0:初始化 1:更新
} effect_update_callback_t;

extern const effect_update_callback_t effect_update_callback_tbl[CFG_MAX];

#endif