#include "menu.h"
#include "stdlib.h"
#include "stdio.h"

#define MAX_PARA_NUM 32
uint16_t currentIndex = 0;
MenuTypedef variableMenu[MAX_PARA_NUM + 4];
int32_t *variables[MAX_PARA_NUM] = {NULL};
const char *variableNames[MAX_PARA_NUM] = {NULL};
char variableCaption[32][MAX_PARA_NUM];
int32_t badVal = 0;
void pushVariable(const char *name, int32_t *val)
{
    if (currentIndex < MAX_PARA_NUM)
    {
        variables[currentIndex] = val;
        variableNames[currentIndex] = name;
        currentIndex++;
    }
}

void variableParaCaptionUpdate(void)
{
    uint8_t i = 4;
    if (currentChoose >= MAX_DISPLAY_LINE - 1)
        while (i--)
            sprintf(variableMenu[currentChoose - i].caption, "%d%s:%d", currentChoose - i, variableNames[currentChoose - i], *variables[currentChoose - i]);
    else
        while (i--)
            sprintf(variableMenu[i].caption, "%d%s:%d", i, variableNames[i], *variables[i]);
}

void variableAdjust_32(KeyChoose key, int32_t *val[MAX_PARA_NUM], int64_t maxVal, int64_t minVal, uint8_t step, void (*menuUpdataCallbackFunction)(void))
{
    uint8_t waitTime = 100;
    if (key == RIGHT_KEY)
    {
        if (*val[currentChoose] < maxVal)
            *val[currentChoose] += step;
        menuUpdataCallbackFunction();
        showMenu(menuManager.getCurrentMenu());
        while (GPIO_ReadInputDataBit(Button_Right_Key) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(Button_Right_Key) == 0)
        {
            if (*val[currentChoose] < maxVal)
                *val[currentChoose] += step;
            menuUpdataCallbackFunction();
            showMenu(menuManager.getCurrentMenu());
            delayMs(50);
        }
    }
    else
    {
        if (*val[currentChoose] > minVal)
            *val[currentChoose] -= step;
        menuUpdataCallbackFunction();
        showMenu(menuManager.getCurrentMenu());
        while (GPIO_ReadInputDataBit(Button_Left_Key) == 0 && waitTime--)
            delayMs(10);
        while (GPIO_ReadInputDataBit(Button_Left_Key) == 0)
        {
            if (*val[currentChoose] > minVal)
                *val[currentChoose] -= step;
            menuUpdataCallbackFunction();
            showMenu(menuManager.getCurrentMenu());
            delayMs(50);
        }
    }
}

void variableInc(void)
{
    variableAdjust_32(RIGHT_KEY, variables, 8000, -8000, 1, variableParaCaptionUpdate);
}
void variableDec(void)
{
    variableAdjust_32(LEFT_KEY, variables, 8000, -8000, 1, variableParaCaptionUpdate);
}

void variableParaCaptionFullUpdate(void)
{
    uint16_t i = MAX_PARA_NUM;
    while (i--)
        sprintf(variableMenu[i].caption, "%d%s:%d", i, variableNames[i] == NULL ? "empty" : variableNames[i], *variables[i]);
}

#include "flash.h"
void VariableSave(void)
{
    uint16_t cnt = currentIndex;
    uint32_t *dat = (uint32_t *)calloc(currentIndex, sizeof(uint32_t));
    while (cnt--)
        dat[cnt] = (uint32_t)(*variables[cnt]);
    Flash_saveData(FlashPage_Variable, dat, currentIndex);
    free(dat);
}
void VariableLoad(void)
{
    uint16_t cnt = currentIndex;
    uint32_t *buf = (uint32_t *)calloc(currentIndex, sizeof(uint32_t));
    Flash_loadData(FlashPage_Variable, buf, currentIndex);
    while (cnt--)
        *variables[cnt] = (int32_t)buf[cnt];
    free(buf);
    variableParaCaptionFullUpdate();
}

void variableMenuInit(void)
{
    uint16_t i = MAX_PARA_NUM;

    variableMenu[MAX_PARA_NUM + 3].caption = NULL;

    variableMenu[MAX_PARA_NUM + 2].caption = "返回上一级";
    variableMenu[MAX_PARA_NUM + 2].left = goBackToLastMenu;
    variableMenu[MAX_PARA_NUM + 2].right = goBackToLastMenu;
    variableMenu[MAX_PARA_NUM + 2].mid = goBackToLastMenu;

    variableMenu[MAX_PARA_NUM + 1].caption = "载入";
    variableMenu[MAX_PARA_NUM + 1].left = VariableLoad;
    variableMenu[MAX_PARA_NUM + 1].right = VariableLoad;
    variableMenu[MAX_PARA_NUM + 1].mid = VariableLoad;

    variableMenu[MAX_PARA_NUM].caption = "保存";
    variableMenu[MAX_PARA_NUM].left = VariableSave;
    variableMenu[MAX_PARA_NUM].right = VariableSave;
    variableMenu[MAX_PARA_NUM].mid = VariableSave;
    while (i--)
    {
        sprintf(variableCaption[i], "%d. empty", i);
        variables[i] = &badVal;
        variableMenu[i].caption = variableCaption[i];
        variableMenu[i].mid = goBackToLastMenu;
        variableMenu[i].left = variableDec;
        variableMenu[i].right = variableInc;
    }
}

void gotoVariableMenu(void)
{
    gotoNextMenu(variableMenu);
    variableParaCaptionFullUpdate();
    showMenu(menuManager.getCurrentMenu());
}
