#include "menu.h"
#include "stdlib.h"
#include "stdio.h"
#include "flash.h"

#define MAX_Array_Size 16
#define MAX_Array_Num 6

static uint16_t currentIndex = 0;
static uint16_t currentLen = 0;

char arrayCaption[32][MAX_Array_Size];

MenuTypedef arrayUnitMenu[MAX_Array_Size + 4];
MenuTypedef arraysMenu[MAX_Array_Num + 4];

int32_t *arrays[MAX_Array_Num];
uint8_t arraySizes[MAX_Array_Num] = {0};

static uint8_t choose_array_index = 0;

void arrayMenuCaptionUpdate(void)
{
    uint8_t i = 4;
    if (currentChoose >= arraySizes[choose_array_index])
        return;
    if (currentChoose >= MAX_DISPLAY_LINE - 1)
        while (i--)
            sprintf(arrayUnitMenu[currentChoose - i].caption, "%d.:%d", currentChoose - i, arrays[choose_array_index][currentChoose - i]);
    else
        while (i--)
            sprintf(arrayUnitMenu[i].caption, "%d.:%d", i, arrays[choose_array_index][i]);
}
void arrayCaptionFullUpdate(void)
{
    uint8_t i = arraySizes[choose_array_index];
    while (i--)
        sprintf(arrayUnitMenu[i].caption, "%d.:%d", i, arrays[choose_array_index][i]);
}
void arraySave(void)
{
    uint16_t tmp, cnt = currentIndex;
    uint32_t *dat = (uint32_t *)calloc(currentLen, sizeof(uint32_t));
    uint32_t *dat_cp = dat;
    while (cnt--)
    {
        tmp = arraySizes[cnt];
        while (tmp--)
            *dat_cp++ = arrays[cnt][tmp];
    }
    Flash_saveData(FlashPage_Array, dat, currentLen);
    free(dat);
}
void arrayLoad(void)
{
    uint16_t tmp, cnt = currentIndex;
    uint32_t *buf = (uint32_t *)calloc(currentLen, sizeof(uint32_t));
    uint32_t *buf_cp = buf;
    Flash_loadData(FlashPage_Variable, buf, currentLen);
    while (cnt--)
    {
        tmp = arraySizes[cnt];
        while (tmp--)
            arrays[cnt][tmp] = *buf_cp++;
    }
    free(buf);
    arrayCaptionFullUpdate();
}

void gotoArrayUnitMenu(void)
{
    choose_array_index = currentChoose;
    if (choose_array_index > currentIndex)
        return;
}

void arrayMenuInit(void)
{
    uint16_t i = MAX_Array_Num;

    arraysMenu[MAX_Array_Num + 3].caption = NULL;

    arraysMenu[MAX_Array_Num + 2].caption = "返回上一级";
    arraysMenu[MAX_Array_Num + 2].left = goBackToLastMenu;
    arraysMenu[MAX_Array_Num + 2].right = goBackToLastMenu;
    arraysMenu[MAX_Array_Num + 2].mid = goBackToLastMenu;

    arraysMenu[MAX_Array_Num + 1].caption = "载入";
    arraysMenu[MAX_Array_Num + 1].left = arrayLoad;
    arraysMenu[MAX_Array_Num + 1].right = arrayLoad;
    arraysMenu[MAX_Array_Num + 1].mid = arrayLoad;

    arraysMenu[MAX_Array_Num].caption = "保存";
    arraysMenu[MAX_Array_Num].left = arraySave;
    arraysMenu[MAX_Array_Num].right = arraySave;
    arraysMenu[MAX_Array_Num].mid = arraySave;
    while (i--)
    {
        sprintf(arrayCaption[i], "%d. empty", i);
        arraysMenu[i].caption = arrayCaption[i];
        arraysMenu[i].mid = goBackToLastMenu;
        arraysMenu[i].left = gotoArrayUnitMenu;
        arraysMenu[i].right = gotoArrayUnitMenu;
    }

    i = MAX_Array_Size;
    arrayUnitMenu[MAX_Array_Size + 3].caption = NULL;

    arrayUnitMenu[MAX_Array_Size].caption = "返回上一级";
    arrayUnitMenu[MAX_Array_Size].left = goBackToLastMenu;
    arrayUnitMenu[MAX_Array_Size].right = goBackToLastMenu;
    arrayUnitMenu[MAX_Array_Size].mid = goBackToLastMenu;

    arrayUnitMenu[MAX_Array_Size + 2].caption = "载入";
    arrayUnitMenu[MAX_Array_Size + 2].left = arrayLoad;
    arrayUnitMenu[MAX_Array_Size + 2].right = arrayLoad;
    arrayUnitMenu[MAX_Array_Size + 2].mid = arrayLoad;

    arrayUnitMenu[MAX_Array_Size + 1].caption = "保存";
    arrayUnitMenu[MAX_Array_Size + 1].left = arraySave;
    arrayUnitMenu[MAX_Array_Size + 1].right = arraySave;
    arrayUnitMenu[MAX_Array_Size + 1].mid = arraySave;

    while (i--)
    {
        sprintf(arrayCaption[i], "%d. empty", i);
        arraysMenu[i].caption = arrayCaption[i];
        arraysMenu[i].mid = goBackToLastMenu;
        arraysMenu[i].left = gotoArrayUnitMenu;
        arraysMenu[i].right = gotoArrayUnitMenu;
    }
}

void pushArray(char *name, int32_t *a, uint8_t len)
{
    if (currentIndex < MAX_Array_Num)
    {
        arrays[currentIndex] = a;
        arraysMenu[currentIndex].caption = name;
        arraySizes[currentIndex] = len;
        currentIndex++;
        currentLen += len;
    }
}

void gotoArrayMenu(void)
{
    gotoNextMenu(arraysMenu);
    showMenu(menuManager.getCurrentMenu());
}
