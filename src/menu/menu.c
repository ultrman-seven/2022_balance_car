#include "oledio.h"
#include "stdlib.h"
#include "menu.h"

#define MAX_DISPLAY_LINE 4

uint8_t currentChoose = 0;

void menuInit(void)
{
    void keyInterruptInit(void);

    oledInit();
    oledReset();
    chooseLine = 0;
    screenClear();
    keyInterruptInit();
    menuManager.append(mainMenu);
    // showMenu();
}

void showMenu(MenuTypedef *menuNode)
{
    uint8_t totalLine = 1;
    screenClear();
    if (currentChoose <= MAX_DISPLAY_LINE - 1)
    {
        while (menuNode->caption != NULL)
        {
            if (totalLine <= MAX_DISPLAY_LINE)
            {
                OLED_printf("%s\n", menuNode->caption);
                // printf("%s\n", menuNode->caption);
                menuNode++;
                totalLine++;
            }
            else
                break;
        }
    }
    else
    {
        menuNode += currentChoose;
        menuNode -= (MAX_DISPLAY_LINE - 1);
        totalLine = MAX_DISPLAY_LINE;
        while (totalLine--)
        {
            OLED_printf("%s\n", menuNode->caption);
            menuNode++;
        }
    }
}

#define currentMenu (menuManager.getCurrentMenu())
void keyUpOption(void)
{
    if (currentChoose > 0)
    {
        currentChoose--;
        if (chooseLine > 0 && currentChoose < MAX_DISPLAY_LINE - 1)
            chooseLine -= 2;
        showMenu(menuManager.getCurrentMenu());
    }
}
void keyDownOption(void)
{
    if (currentMenu[currentChoose + 1].caption != NULL)
    {
        currentChoose++;
        if (chooseLine / 2 < MAX_DISPLAY_LINE - 1)
            chooseLine += 2;
        showMenu(menuManager.getCurrentMenu());
    }
}

void keyLeftOption(void)
{
    if (currentMenu[currentChoose].left != NULL)
        currentMenu[currentChoose].left();
}
void keyMidOption(void)
{
    if (currentMenu[currentChoose].mid != NULL)
        currentMenu[currentChoose].mid();
}
void keyRightOption(void)
{
    if (currentMenu[currentChoose].right != NULL)
        currentMenu[currentChoose].right();
}
