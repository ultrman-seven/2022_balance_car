#include "common.h"
#include <stdlib.h>
#include <math.h>
uint8_t imgBin[1024] = {0};

// #define THRESHOLD 60
uint16_t THRESHOLD = 120;
uint8_t OTSU(uint8_t *image, uint8_t IMAGE_H, uint8_t IMAGE_W)
{
#define GRAY_SCALE 256
    uint8_t threshold = 0; // 最终计算出来的阈值，返回值

    uint16_t pixel_graylevel_count[GRAY_SCALE] = {0}; // 256个元素的数组，储存图片中每个灰阶值对应的像素的个数
    register uint8_t *pixel;
    int16_t i = 0, j = 0; // 循环变量

    uint32_t c1 = 0; // 低于阈值像素数量
    uint32_t c2 = 0; // 高于阈值像素数量
    uint32_t c = 0;
    uint64_t a1 = 0; //低于阈值像素数量与灰度值的乘积和
    uint64_t a2 = 0; //高于阈值像素数量与灰度值的乘积和
    uint64_t t1 = 0;
    uint64_t t2 = 0;
    uint64_t t = 0;

    uint32_t var = 0;
    uint32_t var_max = 0; // 最大类间方差，最后求出来的并不是最大类间方差，省略了每项都有的东西，毕竟只用来比大小，通过公式变换可略去一些相同项

    pixel = image;
    // 记录图像中，从0-255每一个灰阶值拥有的像素个数
    for (i = 0; i < IMAGE_H; i++) // 行
    {
        for (j = 0; j < IMAGE_W; j++) // 列
        {
            pixel_graylevel_count[*pixel] += 1; // 对应灰阶值计数+1，使用指针形式，行变量乘一行的像素数
            pixel++;                            // 像素指针平移
        }
    }

    // 计算总灰度
    for (i = 0; i < GRAY_SCALE; i++)
    {
        a2 += i * pixel_graylevel_count[i];
    }

    // 遍历每一个灰度值，作为阈值
    for (i = 0; i < GRAY_SCALE; i++)
    {
        c1 += pixel_graylevel_count[i];     //小于等于阈值像素数量  // 前景像素数量
        c2 = IMAGE_H * IMAGE_W - c1;        //高于阈值像素数量    //背景像素数量
        a1 += i * pixel_graylevel_count[i]; //小于等于阈值像素数量与灰度值的乘积和  // 前景总灰度
        a2 -= i * pixel_graylevel_count[i]; //大于阈值像素数量与灰度值的乘积和  // 背景总灰度
        if (i <= 15)                        // 跳过前15个阈值
        {
            var = 1;
            continue;
        }
        t1 = c1 * a2;
        t2 = c2 * a1;
        if (t1 > t2)
            t = t1 - t2;
        else
            t = t2 - t1;
        t = t >> 8; // 分子
        t = t * t;
        c = c1 * c2; // 分母
        c >>= 7;
        c += 1;
        var = (uint32_t)(t / c);
        if (var > var_max)
        {
            var_max = var;
            threshold = (uint8_t)i;
        }
        else if (i >= 128)
        {
            break;
        }
    }
    THRESHOLD = threshold;
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
    // while (currentCol--)
    //     imgBin[currentCol] = 0x00;
    while (l--)
    {
        currentCol = 0;
        while (currentCol < c)
        {
            imgBin[128 * currentPage + currentCol++] |=
                ((uint8_t)((*img > t) ? 1 : 0)) << (currentBit);
            currentCol++;
#ifndef _IPS_114__
            *img = (*img > t) ? 255 : 0;
#endif
            img++;
        }
        if (++currentBit == 8)
        {
            currentBit = 0;
            currentPage++;
        }
    }
}

// uint8_t colTmp[64] = {0};
#define MIN_VAL 2
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
            if (cnt < MIN_VAL)
            {
                x += col;
                break;
            }
        }
        else
        {
            if (cnt > MIN_VAL)
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
            if (cnt < MIN_VAL)
            {
                y += line;
                break;
            }
        }
        else
        {
            if (cnt > MIN_VAL)
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

// void conv(uint8_t *mat, uint8_t mat_size, uint8_t *src, uint8_t l, uint8_t c)
// {
//     uint8_t *result = (uint8_t *)calloc(l * c, sizeof(uint8_t));
//     uint8_t row, col, r, i, j;
//     uint16_t num;
//     int16_t l_test, c_test;
//     r = mat_size / 2;
//     for (row = 0; row < l; row++)
//         for (col = 0; col < c; col++)
//         {
//             num = 0;
//             for (i = 0; i < mat_size; i++)
//             {
//                 l_test = row - r + i;
//                 if (l_test < 0 || l_test >= l)
//                     continue;
//                 for (j = 0; j < mat_size; j++)
//                 {
//                     c_test = col - r + j;
//                     if (c_test < 0 || c_test >= c)
//                         ;
//                     else
//                         num += src[l_test * l + c_test] * mat[i * mat_size + j];
//                 }
//             }
//             result[row * l + col] = (uint8_t)(num / 10);
//         }
//     for (i = 0; i < c * l; i++)
//         src[i] = result[i];
//     free(result);
// }

// 图像标记
#define PIXEL_LABEL_BLACK 255    // 黑点
#define PIXEL_LABEL_EXTERNAL 1   // 不属于图像的点（边界）
#define PIXEL_LABEL_MAIN 2       // BFS主区域的点
#define PIXEL_LABEL_INQUENE 3    // BFS队列中的点
#define PIXEL_LABEL_EDGE 4       // BFS边缘的点
#define PIXEL_LABEL_EDGELINE 5   // 8领域边缘搜出来的点
#define PIXEL_LABEL_SUPPLEMENT 6 // 通过补线补出来的点
#define PIXEL_LABEL_ERRORLINE 8  // 置error数组，error数组的点
#define PIXEL_LABEL_ERRORMID 9
#define PIXEL_LABEL_INFLECTION 10
#define PIXEL_LABEL_WHITE 0 // 白点

#define QUE_LENTH 500

uint8_t que_row[QUE_LENTH]; // 队列数组 行
uint8_t que_col[QUE_LENTH]; // 队列数组 列
uint16_t BFS(uint8_t *image, uint16_t IMAGE_H, uint16_t IMAGE_W)
{
    // uint16_t QUE_LENTH = (2 * (IMAGE_H + IMAGE_W) + 1);
    // uint8_t *que_row = (uint8_t *)calloc(QUE_LENTH, sizeof(uint8_t)); // 队列数组 行
    // uint8_t *que_col = (uint8_t *)calloc(QUE_LENTH, sizeof(uint8_t)); // 队列数组 列
    uint16_t front = 0; // 队首位置变量
    uint16_t rear = 0;  // 队尾位置变量
    // register uint8_t *p_front_row = que_row; // 队首指针 行
    // register uint8_t *p_front_col = que_col; // 队首指针 列
    // register uint8_t *p_rear_row = que_row;  // 队尾指针 行
    // register uint8_t *p_rear_col = que_col;  // 队尾指针 列
    uint8_t *p_front_row = que_row; // 队首指针 行
    uint8_t *p_front_col = que_col; // 队首指针 列
    uint8_t *p_rear_row = que_row;  // 队尾指针 行
    uint8_t *p_rear_col = que_col;  // 队尾指针 列
    uint8_t new_row = 0;            // 临时行列号变量
    uint8_t new_col = 0;
    int16_t t = 0; // 循环变量

    // register uint8_t *pixel = image;             // 临时像素指针
    // register uint8_t *front_pixel = NULL;        // 队首像素指针
    uint8_t *pixel = image;                      // 临时像素指针
    uint8_t *front_pixel = NULL;                 // 队首像素指针
    int8_t _directions_4_row[4] = {1, 0, -1, 0}; // 四领域方向数组
    int8_t _directions_4_col[4] = {0, -1, 0, 1};
    uint32_t midCol = 0;
    uint16_t cnt = 0;
    // 图像保护，默认搜到图像，如果BFS首元素就是黑像素，则图像失效，置图像失效标志位。
    //    image_abnormal_flags[IMAGE_FAILURE_FLAG] = 0;

    // 四周安全边框
    for (t = 0; t < IMAGE_W; t++) // 上边框 ，上边框画完指针位于第二行第一个
    {
        *pixel = PIXEL_LABEL_EXTERNAL;
        pixel++;
    }
    for (t = 0; t < IMAGE_H - 1; t++)
    {
        *pixel = PIXEL_LABEL_EXTERNAL;
        pixel += IMAGE_W;
    }
    pixel -= IMAGE_W; // 最后一行第一个
    for (t = 0; t < IMAGE_W; t++)
    {
        *pixel = PIXEL_LABEL_EXTERNAL;
        pixel++;
    }
    pixel--;
    pixel -= IMAGE_W;
    for (t = 0; t < IMAGE_H - 2; t++)
    {
        *pixel = PIXEL_LABEL_EXTERNAL;
        pixel -= IMAGE_W;
    }

    // 首元素入队
    *p_rear_row = IMAGE_H - 2; // 根据图像宽度和高度动态调整
    *p_rear_col = IMAGE_W / 2;

    // 队尾平移
    rear++;
    p_rear_row++;
    p_rear_col++;

    while (rear != front) // 循环结束条件，队尾指针与队头指针相同
    {
        // 置队首指针
        front_pixel = (image + (*p_front_row) * IMAGE_W + (*p_front_col));

        // 保护，如果队首是黑像素点，意味着车不在赛道上，需要触发图像保护
        if (*front_pixel == PIXEL_LABEL_BLACK)
        {
            // free(que_col);
            // free(que_row);
            return IMAGE_W / 2;
        }

        // 队首标记
        *front_pixel = PIXEL_LABEL_MAIN;

        //      4领域入队，右，上，左，下
        for (t = 3; t >= 0; t--)
        {
            // 置四领域点的位置
            new_row = *p_front_row + _directions_4_row[t];
            new_col = *p_front_col + _directions_4_col[t];

            // 置像素指针
            pixel = (image + (new_row * IMAGE_W) + new_col);

            //  标记白色点（255）
            if (*pixel == PIXEL_LABEL_WHITE)
            {
                // 标记
                *pixel = PIXEL_LABEL_INQUENE; // 3号标记表示在队列中

                // 入队
                *p_rear_row = new_row;
                *p_rear_col = new_col;

                // 队尾平移
                rear++;
                p_rear_row++;
                p_rear_col++;
                if (rear == QUE_LENTH) // 超出数组长度，回到数组开头
                {
                    rear = 0;
                    p_rear_row = que_row;
                    p_rear_col = que_col;
                }
            }
            // 4领域内出现黑点，这点就是边界（注意是图像内的黑点，而不是四周边界或者畸变矫正后标记的值为1的点）
            else if (*pixel == PIXEL_LABEL_BLACK)
            {
                *front_pixel = PIXEL_LABEL_EDGE; // 标记为4 （4号标记表示4领域边框）
                midCol += new_col;
                cnt++;
            }
        }
        // 队首指针顺延，出队
        front++;
        p_front_row++;
        p_front_col++;
        if (front == QUE_LENTH) //圆环数组，越界时回到数组开头，循环存储
        {
            front = 0;
            p_front_row = que_row;
            p_front_col = que_col;
        }
    }
    // free(que_col);
    // free(que_row);

    if (cnt)
    {
        t = midCol / cnt;
        if (t >= IMAGE_W)
            return IMAGE_W - 1;
        if (t < 0)
            return 0;
        return t;
    }
    return 0;
}

uint16_t findMax(uint8_t *img, uint16_t height, uint8_t width)
{
    uint16_t maxVal = *img;
    uint16_t maxIdx = 0;
    uint16_t idx = 0;
    height *= width;
    while (idx++ < height)
        if (img[idx] > maxVal)
        {
            maxVal = img[idx];
            maxIdx = idx;
        }
    THRESHOLD = maxVal * 0.75;
    return maxIdx;
}
