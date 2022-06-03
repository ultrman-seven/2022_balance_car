#include "oledio.h"
#include "font.h"
#include "stdlib.h"

uint8_t line = 0;
char wordCount = 0;
int16_t chooseLine = -1;
char oled_str[64] = {0};
void w25qRead(uint32_t add, uint8_t *buf, uint16_t len);

void screenClear(void)
{
	Screen_FillClear(0x00);
	line = wordCount = 0;
}

uint16_t utf8ToUnicode(uint8_t *c)
{
	uint16_t result = 0;
	uint8_t tmp;
	result += c[2] & 0x3f;
	tmp = c[1] & 0x3f;
	result += tmp << 6;
	tmp = c[0] & 0x0f;
	result += tmp << 12;
	return result;
}
#define ZhStart 0x3400
#define MidLen 0x19b5
#define Zh2Start 0x4e00
uint8_t zhBuf[3];
uint8_t zhCnt;
void OLED_putchar(char ch)
{
	if (wordCount >= 16)
	{
		wordCount = 0;
		line += asciiHigh / 8;
	}

	if (zhCnt)
	{
		zhBuf[zhCnt++] = (uint8_t)ch;
		if (zhCnt == 3)
		{
			zhCnt = 0;
			uint16_t zh = utf8ToUnicode(zhBuf);
			uint8_t zhSize = 32;
			uint8_t *pic = (uint8_t *)calloc(zhSize, sizeof(uint8_t));
			if (zh - ZhStart > MidLen)
				w25qRead((MidLen + zh - Zh2Start + 1) * zhSize, pic, zhSize);
			else
				w25qRead((zh - ZhStart) * zhSize, pic, zhSize);
			Picture_display(pic, 1 + wordCount * asciiWide, line, 16, 16);
			wordCount += 2;
			free(pic);
		}
		return;
	}
	if (((uint8_t)ch & 0xf0) == 0xe0)
	{
		zhBuf[zhCnt++] = (uint8_t)ch;
		return;
	}

	switch (ch)
	{
	case '\n':
		line += asciiHigh / 8;
		wordCount = 0;
		return;
	case '\b':
		wordCount--;
		break;
	case 127:
		break;
	default:
		if (ch >= 32)
			Picture_display(ASCII[ch - 32], 1 + wordCount * asciiWide, line, asciiHigh, asciiWide);
		break;
	}
	wordCount++;
}

void OLED_putContrastChar(char ch)
{
	if (wordCount >= 16)
	{
		wordCount = 0;
		line += asciiHigh / 8;
	}

	if (zhCnt)
	{
		zhBuf[zhCnt++] = (uint8_t)ch;
		if (zhCnt == 3)
		{
			zhCnt = 0;
			uint16_t zh = utf8ToUnicode(zhBuf);
			uint8_t zhSize = 32;
			uint8_t *pic = (uint8_t *)calloc(zhSize, sizeof(uint8_t));
			if (zh - ZhStart > MidLen)
				w25qRead((MidLen + zh - Zh2Start + 1) * zhSize, pic, zhSize);
			else
				w25qRead((zh - ZhStart) * zhSize, pic, zhSize);
			PictureContrastDisplay(pic, 1 + wordCount * asciiWide, line, 16, 16);
			wordCount += 2;
			free(pic);
		}
		return;
	}
	if (((uint8_t)ch & 0xf0) == 0xe0)
	{
		zhBuf[zhCnt++] = (uint8_t)ch;
		return;
	}
	switch (ch)
	{
	case '\n':
		line += asciiHigh / 8;
		wordCount = 0;
		return;
	case '\b':
		wordCount--;
		break;
	case 127:
		break;
	default:
		if (ch >= 32)
			PictureContrastDisplay(ASCII[ch - 32], 1 + wordCount * asciiWide, line, asciiHigh, asciiWide);
		break;
	}
	wordCount++;
}

void OLED_putFractionalIntNumber(int num, int bit)
{
	uint32_t count;
	if (num < 0)
	{
		OLED_putchar('-');
		num = -num;
	}
	while (bit--)
		count *= 10;
	if (num > count)
	{
		OLED_putNumber(num / count);
		OLED_putchar('.');
		OLED_putNumber(num % count);
	}
	else
		OLED_putNumber(num);
}

#ifndef __RECURSION__
void OLED_putNumber(__IO int num)
{
	uint32_t count;
	if (num == 0)
		OLED_putchar('0');
	if (num < 0)
	{
		OLED_putchar('-');
		num = -num;
	}
	for (count = 1; num / count; count *= 10)
		;
	count /= 10;
	while (count >= 1)
	{
		OLED_putchar(num / count + '0');
		num %= count;
		count /= 10;
	}
}
#else
void OLED_putNumber(__IO int num)
{
	if (num < 0)
	{
		OLED_putchar('-');
		num = -num;
	}
	if (num >= 10)
		OLED_putNumber(num / 10);
	OLED_putchar(num % 10 + '0');
}
#endif

void OLED_print(char *str)
{
	if (chooseLine == line)
		while (*str)
			OLED_putContrastChar(*str++);
	else
		while (*str)
			OLED_putchar(*str++);
}
