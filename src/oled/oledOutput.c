#include "oledio.h"
#include "font.h"

uint8_t line = 0;
char wordCount = 0;
char chooseLine = -1;
char oled_str[64] = {0};

void screenClear(void)
{
	Screen_FillClear(0x00);
	line = wordCount = 0;
}

void OLED_putchar(char ch)
{
	if (wordCount >= 16)
	{
		wordCount = 0;
		line += asciiHigh / 8;
	}

	switch (ch)
	{
	case '\n':
		line += asciiHigh / 8;
		wordCount = -1;
		break;
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

	switch (ch)
	{
	case '\n':
		line += asciiHigh / 8;
		wordCount = -1;
		break;
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
