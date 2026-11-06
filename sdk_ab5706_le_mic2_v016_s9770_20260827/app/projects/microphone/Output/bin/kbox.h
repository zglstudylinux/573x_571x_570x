#ifndef _KBOX_H
#define _KBOX_H


#define BUF_OFS_CFG_MIX_DRC          		(0x00000016)
#define BUF_LEN_OFS_CFG_MIX_DRC      		(0x00000012)
#define SHOWBUF_OFS_CFG_MIX_DRC      		(0x00000016)
#define SHOWBUF_LEN_OFS_CFG_MIX_DRC  		(0x000001D6)

#define BUF_OFS_CFG_MAGIC            		(0x0000002A)
#define BUF_LEN_OFS_CFG_MAGIC        		(0x00000026)
#define SHOWBUF_OFS_CFG_MAGIC        		(0x0000002A)
#define SHOWBUF_LEN_OFS_CFG_MAGIC    		(0x000001DA)

#define BUF_OFS_CFG_ECHO             		(0x0000003E)
#define BUF_LEN_OFS_CFG_ECHO         		(0x0000003A)
#define SHOWBUF_OFS_CFG_ECHO         		(0x0000003E)
#define SHOWBUF_LEN_OFS_CFG_ECHO     		(0x000001DE)

#define BUF_OFS_CFG_HOWL             		(0x00000052)
#define BUF_LEN_OFS_CFG_HOWL         		(0x0000004E)
#define SHOWBUF_OFS_CFG_HOWL         		(0x00000052)
#define SHOWBUF_LEN_OFS_CFG_HOWL     		(0x000001E2)

#define BUF_OFS_CFG_LOCEQ            		(0x00000066)
#define BUF_LEN_OFS_CFG_LOCEQ        		(0x00000062)
#define SHOWBUF_OFS_CFG_LOCEQ        		(0x00000066)
#define SHOWBUF_LEN_OFS_CFG_LOCEQ    		(0x000001E6)

#define BUF_OFS_CFG_LOCDRC           		(0x0000007A)
#define BUF_LEN_OFS_CFG_LOCDRC       		(0x00000076)
#define SHOWBUF_OFS_CFG_LOCDRC       		(0x0000007A)
#define SHOWBUF_LEN_OFS_CFG_LOCDRC   		(0x000001EA)

#define BUF_OFS_CFG_DAC_EQ           		(0x0000008E)
#define BUF_LEN_OFS_CFG_DAC_EQ       		(0x0000008A)
#define SHOWBUF_OFS_CFG_DAC_EQ       		(0x0000008E)
#define SHOWBUF_LEN_OFS_CFG_DAC_EQ   		(0x000001EE)

#define BUF_OFS_CFG_DAC_DRC          		(0x000000A2)
#define BUF_LEN_OFS_CFG_DAC_DRC      		(0x0000009E)
#define SHOWBUF_OFS_CFG_DAC_DRC      		(0x000000A2)
#define SHOWBUF_LEN_OFS_CFG_DAC_DRC  		(0x000001F2)

#define BUF_OFS_CFG_PRE_EQ           		(0x000000B6)
#define BUF_LEN_OFS_CFG_PRE_EQ       		(0x000000B2)
#define SHOWBUF_OFS_CFG_PRE_EQ       		(0x000000B6)
#define SHOWBUF_LEN_OFS_CFG_PRE_EQ   		(0x000001F6)

#define BUF_OFS_CFG_DY_EQ            		(0x000000CA)
#define BUF_LEN_OFS_CFG_DY_EQ        		(0x000000C6)
#define SHOWBUF_OFS_CFG_DY_EQ        		(0x000000CA)
#define SHOWBUF_LEN_OFS_CFG_DY_EQ    		(0x000001FA)

#define BUF_OFS_CFG_MULT_DRC         		(0x000000DE)
#define BUF_LEN_OFS_CFG_MULT_DRC     		(0x000000DA)
#define SHOWBUF_OFS_CFG_MULT_DRC     		(0x000000DE)
#define SHOWBUF_LEN_OFS_CFG_MULT_DRC 		(0x000001FE)

#define BUF_OFS_CFG_ALL_EQ           		(0x000000F2)
#define BUF_LEN_OFS_CFG_ALL_EQ       		(0x000000EE)
#define SHOWBUF_OFS_CFG_ALL_EQ       		(0x000000F2)
#define SHOWBUF_LEN_OFS_CFG_ALL_EQ   		(0x00000202)

#define BUF_OFS_CFG_ALL_DRC          		(0x00000106)
#define BUF_LEN_OFS_CFG_ALL_DRC      		(0x00000102)
#define SHOWBUF_OFS_CFG_ALL_DRC      		(0x00000106)
#define SHOWBUF_LEN_OFS_CFG_ALL_DRC  		(0x00000206)

#define BUF_OFS_CFG_MSBC_EQ          		(0x0000011A)
#define BUF_LEN_OFS_CFG_MSBC_EQ      		(0x00000116)
#define SHOWBUF_OFS_CFG_MSBC_EQ      		(0x0000011A)
#define SHOWBUF_LEN_OFS_CFG_MSBC_EQ  		(0x0000020A)

#define BUF_OFS_CFG_MSBC_DRC         		(0x0000012E)
#define BUF_LEN_OFS_CFG_MSBC_DRC     		(0x0000012A)
#define SHOWBUF_OFS_CFG_MSBC_DRC     		(0x0000012E)
#define SHOWBUF_LEN_OFS_CFG_MSBC_DRC 		(0x0000020E)

#define BUF_OFS_CFG_CSVD_EQ          		(0x00000142)
#define BUF_LEN_OFS_CFG_CSVD_EQ      		(0x0000013E)
#define SHOWBUF_OFS_CFG_CSVD_EQ      		(0x00000142)
#define SHOWBUF_LEN_OFS_CFG_CSVD_EQ  		(0x00000212)

#define BUF_OFS_CFG_CSVD_DRC         		(0x00000156)
#define BUF_LEN_OFS_CFG_CSVD_DRC     		(0x00000152)
#define SHOWBUF_OFS_CFG_CSVD_DRC     		(0x00000156)
#define SHOWBUF_LEN_OFS_CFG_CSVD_DRC 		(0x00000216)

#define BUF_OFS_CFG_COUNTEQ          		(0x0000016A)
#define BUF_LEN_OFS_CFG_COUNTEQ      		(0x00000166)
#define SHOWBUF_OFS_CFG_COUNTEQ      		(0x0000016A)
#define SHOWBUF_LEN_OFS_CFG_COUNTEQ  		(0x0000021A)

#define BUF_OFS_CFG_CLAS_EQ          		(0x0000017E)
#define BUF_LEN_OFS_CFG_CLAS_EQ      		(0x0000017A)
#define SHOWBUF_OFS_CFG_CLAS_EQ      		(0x0000017E)
#define SHOWBUF_LEN_OFS_CFG_CLAS_EQ  		(0x0000021E)

#define BUF_OFS_CFG_POP_EQ           		(0x00000192)
#define BUF_LEN_OFS_CFG_POP_EQ       		(0x0000018E)
#define SHOWBUF_OFS_CFG_POP_EQ       		(0x00000192)
#define SHOWBUF_LEN_OFS_CFG_POP_EQ   		(0x00000222)

#define BUF_OFS_CFG_ROCK_EQ          		(0x000001A6)
#define BUF_LEN_OFS_CFG_ROCK_EQ      		(0x000001A2)
#define SHOWBUF_OFS_CFG_ROCK_EQ      		(0x000001A6)
#define SHOWBUF_LEN_OFS_CFG_ROCK_EQ  		(0x00000226)

#define BUF_OFS_CFG_JAZZ_EQ          		(0x000001BA)
#define BUF_LEN_OFS_CFG_JAZZ_EQ      		(0x000001B6)
#define SHOWBUF_OFS_CFG_JAZZ_EQ      		(0x000001BA)
#define SHOWBUF_LEN_OFS_CFG_JAZZ_EQ  		(0x0000022A)

#define BUF_OFS_CFG_MIX_EQ           		(0x000001CE)
#define BUF_LEN_OFS_CFG_MIX_EQ       		(0x000001CA)
#define SHOWBUF_OFS_CFG_MIX_EQ       		(0x000001CE)
#define SHOWBUF_LEN_OFS_CFG_MIX_EQ   		(0x0000022E)



#define ALL_MODE_NAME_SUM            		 0x00002BE9


//EFFECT_MODE_IDX
enum {
	CFG_MIX_DRC = 0,
	CFG_MAGIC,
	CFG_ECHO,
	CFG_HOWL,
	CFG_LOCEQ,
	CFG_LOCDRC,
	CFG_DAC_EQ,
	CFG_DAC_DRC,
	CFG_PRE_EQ,
	CFG_DY_EQ,
	CFG_MULT_DRC,
	CFG_ALL_EQ,
	CFG_ALL_DRC,
	CFG_MSBC_EQ,
	CFG_MSBC_DRC,
	CFG_CSVD_EQ,
	CFG_CSVD_DRC,
	CFG_COUNTEQ,
	CFG_CLAS_EQ,
	CFG_POP_EQ,
	CFG_ROCK_EQ,
	CFG_JAZZ_EQ,
	CFG_MIX_EQ,
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