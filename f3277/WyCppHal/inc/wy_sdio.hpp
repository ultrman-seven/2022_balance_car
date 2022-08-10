#ifndef CD2BA78D_C087_4FBD_B9AD_CE54A10C5FFF
#define CD2BA78D_C087_4FBD_B9AD_CE54A10C5FFF
#include "common.h"
#include "hal_sdio.h"

extern SD_CardInfo SDCardInfo;
namespace sdio
{
    void sdioInit(void);
    u8 SD_ReadDisk(u8 *buf, u32 sector, u32 cnt);
    u8 SD_WriteDisk(const u8 *buf, u32 sector, u32 cnt);
    SD_Error SD_SendStatus(uint32_t *pcardstatus);
    SDCardState SD_GetState(void);
} // namespace sdio

#endif /* CD2BA78D_C087_4FBD_B9AD_CE54A10C5FFF */
