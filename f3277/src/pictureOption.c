#include <stdlib.h>
#include <math.h>
#include "stdio.h"
#include "camera.h"
// uint8_t imgBin[1024] = {0};

void printPic(uint8_t *pic, uint8_t h, uint8_t w)
{
}
// #define THRESHOLD 60
uint16_t THRESHOLD = 120;
uint8_t OTSU(uint8_t *image, uint16_t IMAGE_H, uint16_t IMAGE_W)
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

void imgGray2Bin(uint8_t *img, uint16_t l, uint16_t c)
{
    uint32_t cnt = l * c;
    uint16_t t = THRESHOLD; // otsuThreshold(img, c, l);
    while (cnt--)
    {
        *img = (*img > t) ? 255 : 0;
        img++;
    }
    // printf("l%d:,c:%d,cnt:%d\r\n", l, c, cnt);
}

// uint8_t colTmp[64] = {0};
#define MIN_VAL 2
uint32_t findPointCenter(uint8_t *img, uint16_t l, uint16_t c)
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

uint8_t pic_cp[PIC_COL * PIC_LINE] = {0};
#define array_2d_2_1d(NAME, ROW, COL, A_C) NAME[((ROW) * (A_C)) + (COL)]

uint8_t *conv(uint16_t *core, uint8_t core_size, uint8_t *src, uint16_t l, uint16_t c)
{
#define core2d(row, col) array_2d_2_1d(core, row, col, core_size)
#define src2d(row, col) array_2d_2_1d(src, row, col, c)
#define res2d(row, col) array_2d_2_1d(pic_cp_tmp, row, col, c)

    uint8_t *pic_cp_tmp = pic_cp + PIC_CUT;
    uint16_t l_tp, c_tp;
    uint8_t rl, rc, core_radius = core_size / 2;
    uint32_t tmp;
    uint16_t core_sum = 0;
    l_tp = core_size * core_size;
    while (l_tp--)
        core_sum += core[l_tp];

    for (l_tp = 0; l_tp < l; l_tp++)
    {
        for (c_tp = 0; c_tp < c; c_tp++)
        {
            tmp = 0;
            rl = core_size;
            while (rl--)
            {
                if (l_tp + rl - core_radius >= 0 && l_tp + rl - core_radius < l)
                {
                    rc = core_size;
                    while (rc--)
                        if (c_tp + rc - core_radius >= 0 && c_tp + rc - core_radius < c)
                            tmp +=
                                src2d(l_tp + rl - core_radius, c_tp + rc - core_radius) * core2d(rl, rc);
                }
            }
            res2d(l_tp, c_tp) = tmp / core_sum;
        }
    }
    for (l_tp = 0; l_tp < c * l; l_tp++)
        src[l_tp] = pic_cp_tmp[l_tp];

    return pic_cp;
}

uint16_t gaussian[] = {
    923, 1191, 923,
    1191, 1538, 1191,
    923, 1191, 923};

uint8_t *gaussianFilter(uint8_t *img, uint16_t l, uint16_t c) { return conv(gaussian, 3, img, l, c); }

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

#define QUE_LENTH 800

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

uint16_t findMax(uint8_t *img, uint16_t height, uint16_t width)
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

#define MAX_LABEL 512
uint8_t unionFind[MAX_LABEL] = {0};
point labelFirstPoint[MAX_LABEL];
// uint8_t picLabel[PL * PC] = {0};
uint8_t picLabel[PIC_LINE * PIC_COL];
uint8_t maxLabelValue;
// uint8_t labNum;
void twoPass(uint8_t *img, uint16_t l, uint16_t c)
{
#define img2d(row, col) array_2d_2_1d(img, row, col, c)
#define lab2d(row, col) array_2d_2_1d(picLabel, row, col, c)
    uint16_t l_tp, c_tp;
    uint8_t label = 1;
    uint8_t leftExistPixel, upExistPixel;
    // labNum = 0;
    l_tp = l * c;
    while (l_tp--)
        picLabel[l_tp] = 0;
    l_tp = MAX_LABEL;
    while (l_tp--)
        unionFind[l_tp] = 0;
    // first pass
    for (l_tp = 0; l_tp < l; l_tp++)
    {
        for (c_tp = 0; c_tp < c; c_tp++)
        {
            if (img2d(l_tp, c_tp))
            {
                leftExistPixel = (c_tp - 1 >= 0) && img2d(l_tp, c_tp - 1);
                upExistPixel = (l_tp - 1 >= 0) && img2d(l_tp - 1, c_tp);
                if (leftExistPixel)
                {
                    if (upExistPixel)
                    {
                        if (lab2d(l_tp, c_tp - 1) > lab2d(l_tp - 1, c_tp))
                            unionFind[lab2d(l_tp, c_tp - 1)] = lab2d(l_tp, c_tp) = (lab2d(l_tp - 1, c_tp));
                        else if (lab2d(l_tp, c_tp - 1) < lab2d(l_tp - 1, c_tp))
                            unionFind[lab2d(l_tp - 1, c_tp)] = lab2d(l_tp, c_tp) = (lab2d(l_tp, c_tp - 1));
                        else
                            lab2d(l_tp, c_tp) = (lab2d(l_tp, c_tp - 1));
                    }
                    else
                        lab2d(l_tp, c_tp) = lab2d(l_tp, c_tp - 1);
                }
                else if (upExistPixel)
                    lab2d(l_tp, c_tp) = lab2d(l_tp - 1, c_tp);
                else
                {
                    labelFirstPoint[label].x = c_tp;
                    labelFirstPoint[label].y = l_tp;
                    lab2d(l_tp, c_tp) = label++;
                }
            }
            else
                lab2d(l_tp, c_tp) = 0;
        }
    }

    // second pass
    maxLabelValue = 0;
    while (--label)
    {
        if (!unionFind[label])
        {
            if (label > maxLabelValue)
                maxLabelValue = label;
            unionFind[label] = label;
            continue;
        }
        l_tp = label;
        while (unionFind[l_tp])
            l_tp = unionFind[l_tp];
        unionFind[label] = l_tp;
    }

    l_tp = c * l;
    while (l_tp--)
        picLabel[l_tp] = unionFind[picLabel[l_tp]];
}

point getCenter(uint8_t label)
{
    uint16_t l_tp = 0, c_tp;
    point result = {0, 0};
    uint32_t x = 0, y = 0;
    uint16_t cnt = 0;

    // while (picLabel[l_tp] != label)
    //     if (l_tp++ >= PIC_COL * PIC_LINE)
    //         return result;

    // c_tp = l_tp % PIC_COL;
    // l_tp /= PIC_COL;
    for (l_tp = labelFirstPoint[label].y; l_tp < PIC_LINE; l_tp++)
    {
        c_tp = 0;
        while (array_2d_2_1d(picLabel, l_tp, c_tp, PIC_COL) != label)
            c_tp++;
        for (; c_tp < PIC_COL; c_tp++)
        {
            if (array_2d_2_1d(picLabel, l_tp, c_tp, PIC_COL) == label)
            {
                x += c_tp;
                y += l_tp;
                cnt++;
            }
            else
                break;
        }
    }
    if (cnt == 0)
        return result;
    result.x = x / cnt;
    result.y = y / cnt;
    return result;
}

typedef struct
{
    point position;
    uint8_t maxGray, minGray, arvGray;
    uint16_t area;
} AreaFeature;
#include "string.h"
AreaFeature getFeature(uint8_t label)
{
    AreaFeature result;

    uint16_t l_tp = 0, c_tp;
    uint32_t x = 0, y = 0, bright = 0;
    uint16_t cnt = 0;
    uint8_t min = 0xff, max = 0x00;
    memset(&result, 0, sizeof(AreaFeature));
    for (l_tp = labelFirstPoint[label].y; l_tp < PIC_LINE; l_tp++)
    {
        c_tp = 0;
        while (array_2d_2_1d(picLabel, l_tp, c_tp, PIC_COL) != label)
            c_tp++;
        for (; c_tp < PIC_COL; c_tp++)
        {
            if (array_2d_2_1d(picLabel, l_tp, c_tp, PIC_COL) == label)
            {
                if (array_2d_2_1d(pic_cp, l_tp, c_tp, PIC_COL) > max)
                    max = array_2d_2_1d(pic_cp, l_tp, c_tp, PIC_COL);
                if (array_2d_2_1d(pic_cp, l_tp, c_tp, PIC_COL) < min)
                    min = array_2d_2_1d(pic_cp, l_tp, c_tp, PIC_COL);
                bright += array_2d_2_1d(pic_cp, l_tp, c_tp, PIC_COL);
                x += c_tp;
                y += l_tp;
                cnt++;
            }
            else
                break;
        }
    }
    if (cnt == 0)
        return result;
    result.area = cnt;
    result.position.x = x / cnt;
    result.position.y = y / cnt;
    result.maxGray = max;
    result.minGray = min;
    result.arvGray = bright / cnt;
    return result;
}

uint8_t valExist(uint8_t *arr, uint8_t len, uint8_t val)
{
    while (len--)
        if (arr[len] == val)
            return 1;
    return 0;
}

// void printAllFeatures(void)
// {
//     uint8_t tmp, cnt = 1;
//     uint8_t *l = (uint8_t *)calloc(maxLabelValue, sizeof(uint8_t));
//     memset(l, 0x00, maxLabelValue);
//     l[0] = unionFind[1];
//     for (tmp = 2; tmp <= maxLabelValue; tmp++)
//     {
//         if (!valExist(l, cnt, unionFind[tmp]))
//             l[cnt++] = unionFind[tmp];
//     }
//     printf("=====*****area numbers%d:*****=====\r\n", cnt);
//     while (cnt--)
//     {
//         tmp = l[cnt];
//         AreaFeature f = getFeature(tmp);
//         printf("area%d:\r\n", tmp);
//         printf("position:    x: %3d, y: %3d\r\n", f.position.x, f.position.y);
//         printf("brightness:    max: %3d, min: %3d, arv: %3d\r\n", f.maxGray, f.minGray, f.arvGray);
//         printf("total area: %3d\r\n\r\n", f.area);
//     }
//     free(l);
// }

float getAspectRatio(uint8_t label, point center)
{
    uint16_t x1, x2, y1, y2;
    float result;

    x1 = 0;
    while (array_2d_2_1d(picLabel, center.y, x1, PIC_COL) != label)
        x1++;

    x2 = PIC_COL - 1;
    while (array_2d_2_1d(picLabel, center.y, x2, PIC_COL) != label)
        x2--;

    y1 = 0;
    while (array_2d_2_1d(picLabel, y1, center.x, PIC_COL) != label)
        y1++;

    y2 = PIC_LINE - 1;
    while (array_2d_2_1d(picLabel, y2, center.x, PIC_COL) != label)
        y2--;

    if ((y2 - y1 + 1) == 0)
        return 0;
    result = (float)(x2 - x1 + 1) / (float)(y2 - y1 + 1);
    return result;
}

uint8_t bright_area_ok(uint8_t b, uint16_t a)
{
    uint64_t x6, x5, x4, x3;
    uint32_t x2;
    float result;

    x2 = a * a;
    x3 = x2 * a;
    x4 = x2 * x2;
    x5 = x3 * x2;
    x6 = x3 * x3;

    result = -3981509.0 / x6 + 6123470.0 / x5 - 2473755.0 / x4 + 333106.0 / x3 + 1891.0 / x2 - 3386.0 / a + 200.1;
    // result = 29684.57 / x6 - 132074.725 / x5 + 189163.6 / x4 - 115328.5 / x3 + 32444.2 / x2 - 4075.34 / a + 203.14;
    result /= b;
    if (result <= 2)
    // result -= b;
    // if (result <= 40)
        return 1;
    return 0;
}
point findLamp(void)
{
    uint8_t tmp, cnt = 1;
    uint8_t *l = (uint8_t *)calloc(maxLabelValue, sizeof(uint8_t));

    memset(l, 0x00, maxLabelValue);
    l[0] = unionFind[1];
    for (tmp = 2; tmp <= maxLabelValue; tmp++)
    {
        if (!valExist(l, cnt, unionFind[tmp]))
            l[cnt++] = unionFind[tmp];
    }

    while (cnt--)
    {
        tmp = l[cnt];
        AreaFeature f = getFeature(tmp);
        if (bright_area_ok(f.arvGray, f.area))
        {
            free(l);
            return f.position;
        }
    }
    free(l);
    point p = {0, 0};
    return p;
}

// uint16_t findLamp(void)
// {
//     uint8_t tmp, cnt = 1;
//     uint8_t *l = (uint8_t *)calloc(maxLabelValue, sizeof(uint8_t));
//     float r;

//     memset(l, 0x00, maxLabelValue);
//     l[0] = unionFind[1];
//     for (tmp = 2; tmp <= maxLabelValue; tmp++)
//     {
//         if (!valExist(l, cnt, unionFind[tmp]))
//             l[cnt++] = unionFind[tmp];
//     }
//     while (cnt--)
//     {
//         tmp = l[cnt];
//         point p = getCenter(tmp);
//         if (p.x == 0)
//             continue;
//         r = getAspectRatio(tmp, p);
//         // printf("l:%d\r\n", tmp);
//         if (r > 2 || r < 0.5)
//             continue;
//         else
//         {
//             free(l);
//             return p.x;
//         }
//     }
//     free(l);
//     return 0;
// }

void fuck_zaoDian(uint8_t *img, uint16_t l, uint16_t c)
{
    uint8_t *out = pic_cp;
    for (uint8_t i = 1; i < l - 1; ++i)
        for (uint8_t j = 1; j < c - 1; ++j)
        {
            uint8_t cnt = 0;
            if (img2d(i, j) >= 30)
            {
                cnt += abs(img2d(i, j) - img2d(i - 1, j - 1)) > 10;
                cnt += abs(img2d(i, j) - img2d(i - 1, j + 1)) > 10;
                cnt += abs(img2d(i, j) - img2d(i + 1, j - 1)) > 10;
                cnt += abs(img2d(i, j) - img2d(i + 1, j + 1)) > 10;
            }
            array_2d_2_1d(out, i, j, c) = (cnt >= 4 ? ((img2d(i - 1, j - 1) + img2d(i - 1, j) + img2d(i - 1, j + 1) + img2d(i, j - 1) + img2d(i, j) + img2d(i, j + 1) + img2d(i + 1, j - 1) + img2d(i + 1, j) + img2d(i + 1, j + 1)) / 9) : img2d(i, j));
        }

    for (uint16_t i = 0; i < PIC_COL * PIC_LINE; i++)
        img[i] = out[i];
}

//数组向前越界
#define CONV_ABS(A) (((A) > 0) ? (A) : (-(A)))
//数组向后越界
#define CONV_OUT(A, B) (((A) >= (B)) ? ((B)*2 - 2 - (A)) : (A))
void convFast(int16_t *core, uint8_t core_size, uint8_t *src, uint16_t l, uint16_t c)
{
    uint8_t *pic_cp_tmp = pic_cp;
    uint16_t l_tp, c_tp;
    uint8_t r, core_radius = core_size / 2;
    uint32_t tmp;
    uint16_t core_sum = 0;
    //求归一化系数g' (完整高斯核归一化系数的根号)
    while (core_size--)
        core_sum += core[core_size];

    //第一次循环，行卷积核
    for (l_tp = 0; l_tp < l; l_tp++)
    {
        for (c_tp = 0; c_tp < c; c_tp++)
        {
            tmp = src2d(l_tp, c_tp) * core[core_radius];
            r = core_radius + 1;
            while (--r)
            {
                tmp += src2d(l_tp, CONV_ABS(c_tp - r)) * core[core_radius - r];
                tmp += src2d(l_tp, CONV_OUT(c_tp + r, PIC_COL)) * core[core_radius + r];
            }
            res2d(l_tp, c_tp) = tmp / core_sum;
        }
    }

    //第二次循环，列卷积核
    for (l_tp = 0; l_tp < l; l_tp++)
    {
        for (c_tp = 0; c_tp < c; c_tp++)
        {
            tmp = res2d(l_tp, c_tp) * core[core_radius];
            r = core_radius + 1;
            while (--r)
            {
                tmp += res2d(CONV_ABS(l_tp - r), c_tp) * core[core_radius - r];
                tmp += res2d(CONV_OUT(l_tp + r, PIC_LINE), c_tp) * core[core_radius + r];
            }
            src2d(l_tp, c_tp) = tmp / (core_sum);
        }
    }
}

int16_t gaussian1D[] = {30, 39, 30};
uint16_t sobel1D[] = {0};

void gaussianFilterFast(uint8_t *img, uint16_t l, uint16_t c) { convFast(gaussian1D, 3, img, l, c); }
