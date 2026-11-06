#include "include.h"
#include "effect.h"


const effect_info_cfg_t effect_info[CFG_MAX] = {
	[CFG_MIC_EQ]   = {"CFG_MIC_EQ##", BUF_OFS_CFG_MIC_EQ,    BUF_LEN_OFS_CFG_MIC_EQ},
	[CFG_MIC_DRC]  = {"CFG_MIC_DRC#", BUF_OFS_CFG_MIC_DRC,   BUF_LEN_OFS_CFG_MIC_DRC},
};


/*模块初始化更新回调注册表 cv到应用层使用
const effect_update_callback_t effect_update_callback_tbl[CFG_MAX] = {
	[CFG_MIC_EQ]   = {NULL},
	[CFG_MIC_DRC]  = {NULL},
};
*/