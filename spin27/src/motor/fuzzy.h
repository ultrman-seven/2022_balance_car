#ifndef __WY_FUZZY_H__
#define __WY_FUZZY_H__
#include "common.h"

#define defAverageMembership(name, num, a, b)           \
    const uint8_t name##_fuzzy_size = num * 2 - 1;      \
    const float __##name##__fuzzy_start = a;            \
    const float __##name##__fuzzy_step = (b - a) / num; \
    float fuzzy_##name##[num * 2 - 1]

#define defMembership(name, num, ...)              \
    const uint8_t name##_fuzzy_size = num * 2 - 1; \
    int32_t fuzzy_##name##[num * 2 - 1] = {##__VA_ARGS__}

#define initAverageMembership(name)                                                         \
    {                                                                                       \
        uint8_t cnt = 0;                                                                    \
        while (cnt < name##_fuzzy_size)                                                     \
            fuzzy_##name##[cnt++] = __##name##__fuzzy_start + cnt * __##name##__fuzzy_step; \
    }

#define defFuzzyRule(name, ...) int32_t fuzzyRule_##name##[] = {##__VA_ARGS__}
float fuzzy_1_dimensional(float input, uint8_t fuzzy_size, int32_t *fuzzy_rule, int32_t *input_section);
float fuzzy_2_dimensional(float in1, float in2,
                          uint8_t size1, uint8_t size2,
                          float *fuzzy_rule, float *input_section1, float *input_section2);
#endif /*__ WY_FUZZY_H__ */
