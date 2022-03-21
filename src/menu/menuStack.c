#include "common.h"
#include "menu.h"
#include "stdlib.h"

#define MAX_MENU_RECODER 10
uint8_t currentStackIndex = 0;
MenuTypedef *menuStackRecoder[MAX_MENU_RECODER];

void pushMenu(MenuTypedef *menu)
{
    if (currentChoose < MAX_MENU_RECODER)
        menuStackRecoder[currentStackIndex++] = menu;
}

MenuTypedef *popMenu(void)
{
    if (currentStackIndex > 0)
        return menuStackRecoder[--currentStackIndex-1];
    else
        return NULL;
}

MenuTypedef *getCurrentStackValue(void)
{
    return menuStackRecoder[currentStackIndex-1];
}

MenuRecorderObject menuManager={
    .append=pushMenu,
    .getLastMenu=popMenu,
    .getCurrentMenu=getCurrentStackValue
};

