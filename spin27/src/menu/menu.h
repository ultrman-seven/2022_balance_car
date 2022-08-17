#ifndef __WY_LIB_MENU_H__
#define __WY_LIB_MENU_H__

#include "common.h"
typedef struct
{
    char *caption;
    void (*mid)(void);
    void (*left)(void);
    void (*right)(void);
} MenuTypedef;
#define MAX_DISPLAY_LINE 4
typedef struct
{
    void (*append)(MenuTypedef *);
    MenuTypedef *(*getCurrentMenu)(void);
    MenuTypedef *(*getLastMenu)(void);
} MenuRecorderObject;

#define GO_BACK_MENU                                                                                       \
    {                                                                                                      \
        .caption = "返回上一级", .mid = goBackToLastMenu, .right = goBackToLastMenu, .left = goBackToLastMenu \
    }
#define END_OF_MENU                                               \
    {                                                             \
        .caption = NULL, .left = NULL, .right = NULL, .mid = NULL \
    }

#define MID_KEY_EXTI_LINE EXTI_Line12

#define Button_Left_Key GPIOB, GPIO_Pin_4
#define Button_Right_Key GPIOB, GPIO_Pin_3

extern MenuRecorderObject menuManager;
extern uint8_t currentChoose;
extern MenuTypedef mainMenu[];

extern char menuCaptionStr1[20];
extern char menuCaptionStr2[20];
extern char menuCaptionStr3[20];
extern char menuCaptionStr4[20];

extern uint16_t menuVal_u16_Buf[4];
extern int32_t menuVal_32_Buf[4];

void keyUpOption(void);
void keyDownOption(void);
void keyLeftOption(void);
void keyMidOption(void);
void keyRightOption(void);
void showMenu(MenuTypedef *menuNode);

typedef enum
{
    LEFT_KEY,
    RIGHT_KEY
} KeyChoose;

void goBackToLastMenu(void);
void gotoNextMenu(MenuTypedef *menu);

void valueAdjust_32(KeyChoose key, int32_t *val, int64_t maxVal, int64_t minVal, uint8_t step, void (*menuUpdataCallbackFunction)(void));
void valueAdjust_u16(KeyChoose key, uint16_t *val, uint64_t maxVal,uint8_t step, void (*menuUpdataCallbackFunction)(void));

void variableMenuInit(void);
void gotoVariableMenu(void);
void pushVariable(const char *name, int32_t *val);

void arrayMenuInit(void);
void gotoArrayMenu(void);
void pushArray(char *name, int32_t *a, uint8_t len);
#endif /* __WY_LIB_MENU_H__ */
