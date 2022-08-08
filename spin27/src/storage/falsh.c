#include "common.h"

// #define PAGE_ADD 0x0800f000

void Flash_saveData(uint32_t PAGE_ADD, uint32_t *dat, uint16_t length)
{
    FLASH_Unlock();
    FLASH_ErasePage(PAGE_ADD);
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    if (length > 0 && length < 256)
        while (length--)
            FLASH_ProgramWord(PAGE_ADD + length * 4, *dat++);
    FLASH_Lock();
}

void Flash_loadData(uint32_t PAGE_ADD, uint32_t *buf, uint16_t length)
{
    if (length > 0 && length < 256)
        while (length--)
            *buf++ = *(__IO uint32_t *)(PAGE_ADD + length * 4);
}
