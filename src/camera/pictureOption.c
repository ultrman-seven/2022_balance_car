#include "oledio.h"
#include <stdlib.h>
#include <math.h>
uint8_t imgBin[1024] = {0};

void printPic(void)
{
    showPic(imgBin, 0, 0, 64, 128);
}
#define THRESHOLD 60

uint16_t otsuThreshold(uint8_t *image, int col, int row)
{
#define GrayScale 256
    int width = col;
    int height = row;
    uint16_t pixelCount[GrayScale] = {0};     //每个灰度值所占像素个数
    float pixelPro[GrayScale] = {0};          //每个灰度值所占总像素比例
    uint16_t i, j, pixelSum = width * height; //总像素
    uint16_t threshold = 0;
    uint8_t *data = image; //指向像素数据的指针

    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
            pixelCount[(int)data[i * width + j]]++; //将像素值作为计数数组的下标
    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
    for (i = 0; i < GrayScale; i++) // i作为阈值
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {
            if (j <= i) //背景部分
            {
                pixelPro[i] = (float)pixelCount[i] / pixelSum; //计算每个像素在整幅图像中的比例
                w0 += pixelPro[j];                             //背景像素点占整个图像的比例
                u0tmp += j * pixelPro[j];
            }
            else //前景部分
            {
                pixelPro[i] = (float)pixelCount[i] / pixelSum; //计算每个像素在整幅图像中的比例
                w1 += pixelPro[j];                             //前景像素点占整个图像的比例
                u1tmp += j * pixelPro[j];
            }
        }
        u0 = u0tmp / w0;                                 //背景平均灰度μ0
        u1 = u1tmp / w1;                                 //前景平均灰度μ1
        deltaTmp = (float)(w0 * w1 * pow((u0 - u1), 2)); //类间方差公式 g = w1 * w2 * (u1 - u2) ^ 2
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = i;
        }
    }

    return threshold;
}

void imgGray2Bin(uint8_t *img, uint8_t l, uint8_t c)
{
    uint8_t currentBit = 0;
    uint8_t currentPage = 0;
    uint16_t currentCol = 1024;
    uint16_t t = THRESHOLD; // otsuThreshold(img, c, l);
    if (l > 64 || c > 128)
        return;
    while (currentCol--)
        imgBin[currentCol] = 0x00;
    while (l--)
    {
        currentCol = 0;
        while (currentCol < c)
            imgBin[128 * currentPage + currentCol++] |=
                ((uint8_t)((*img++ > t) ? 1 : 0)) << (currentBit);
        if (++currentBit == 8)
        {
            currentBit = 0;
            currentPage++;
        }
    }
}

// uint8_t colTmp[64] = {0};
uint32_t findPointCenter(uint8_t *img, uint8_t l, uint8_t c)
{
    uint16_t x = 0, y = 0;
    uint8_t line, col, startFlag = 0, cnt = 0;
    // uint16_t t = otsuThreshold(img, c, l);

    for (col = 0; col < c; col++)
    {
        for (line = 0; line < l; line++)
            if (img[line * c + col] > THRESHOLD)
                cnt++;
        if (startFlag)
        {
            if (cnt < 5)
            {
                x += col;
                break;
            }
        }
        else
        {
            if (cnt > 5)
            {
                x += col;
                startFlag = 1;
            }
        }
        cnt = 0;
    }
    startFlag = 0;
    for (line = 0; line < l; line++)
    {
        for (col = 0; col < c; col++)
            if (img[line * c + col] > THRESHOLD)
                cnt++;

        if (startFlag)
        {
            if (cnt < 5)
            {
                y += line;
                break;
            }
        }
        else
        {
            if (cnt > 5)
            {
                y += line;
                startFlag = 1;
            }
        }
        cnt = 0;
    }
    x /= 2;
    y /= 2;
    return ((y << 8) & 0xff00) + x;
}

void conv(uint8_t *mat, uint8_t mat_size, uint8_t *src, uint8_t l, uint8_t c)
{
    uint8_t *result = (uint8_t *)calloc(l * c, sizeof(uint8_t));
    uint8_t row, col, r, i, j;
    uint16_t num;
    int16_t l_test, c_test;
    r = mat_size / 2;
    for (row = 0; row < l; row++)
        for (col = 0; col < c; col++)
        {
            num = 0;
            for (i = 0; i < mat_size; i++)
            {
                l_test = row - r + i;
                if (l_test < 0 || l_test >= l)
                    continue;
                for (j = 0; j < mat_size; j++)
                {
                    c_test = col - r + j;
                    if (c_test < 0 || c_test >= c)
                        ;
                    else
                        num += src[l_test * l + c_test] * mat[i * mat_size + j];
                }
            }
            result[row * l + col] = (uint8_t)(num / 10);
        }
    for (i = 0; i < c * l; i++)
        src[i] = result[i];
    free(result);
}

uint8_t gaussian[] = {
    92, 119, 92,
    119, 154, 119,
    92, 119, 92};

void gaussianFilter(uint8_t *img, uint8_t l, uint8_t c)
{
    conv(gaussian, 3, img, l, c);
}
