#ifndef __WY_LIB_FLASH_H__
#define __WY_LIB_FLASH_H__
typedef enum
{
    FlashPage_PID = 0x0800f000,
    FlashPage_Variable = FlashPage_PID + 0x400,
    FlashPage_Array = FlashPage_Variable + 0x400,
    FlashPage_Fuzzy_BLEuart = FlashPage_Array + 0x400
} FlashPageAreas;
void Flash_saveData(uint32_t PAGE_ADD, uint32_t *dat, uint16_t length);
void Flash_loadData(uint32_t PAGE_ADD, uint32_t *buf, uint16_t length);
#endif /* __WY_LIB_FLASH_H__ */
