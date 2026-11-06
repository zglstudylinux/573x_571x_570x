#include "config.h"
depend(0x01030500);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
#if (FLASH_RES2_SIZE > 0)
setkeepseg(FLASH_RES2_SIZE);
#endif
setspace(FLASH_RESERVE_SIZE);
//setauth(0xDDE05A0D, soft_key);    //LDAC加密狗
#if (FOT_EN && AB_FOT_TYPE == AB_FOT_TYPE_PACK)
setunpack(unpack.bin);
setpkgarea(AB_FOT_PACK_START_ADDR, AB_FOT_PACK_SIZE);
#endif
#if FOT_GUID_CHECK_EN
setid(AB000102-FFFF-FFFF-FFFF-FFFFFFFFFFFF); //自定义GUID，用于区分产品，限制相同芯片不同产品固件互相升级
#endif
setver(SW_VERSION);
make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, updater.bin);
save(dcf_buf, app.dcf);
