#include "fuzzy.h"

#define def_binary_search(ValType, ArrayType)                                                                          \
    uint16_t binary_search__V_##ValType##_A_##ArrayType##(##ValType value, ##ArrayType## * array, uint16_t array_size) \
    {                                                                                                                  \
        uint16_t l, r, m;                                                                                              \
        l = 0;                                                                                                         \
        r = array_size - 1;                                                                                            \
        if (array_size < 3)                                                                                            \
            return 0;                                                                                                  \
        if (value <= *array)                                                                                           \
            return 0;                                                                                                  \
        if (value >= array[r])                                                                                         \
            return r;                                                                                                  \
        while (1)                                                                                                      \
        {                                                                                                              \
            m = (l + r) / 2;                                                                                           \
            if (value < array[m])                                                                                      \
                r = m;                                                                                                 \
            else if (value > array[m + 1])                                                                             \
                l = m;                                                                                                 \
            else                                                                                                       \
                return m;                                                                                              \
            if (l == r)                                                                                                \
                return l;                                                                                              \
        }                                                                                                              \
    }

def_binary_search(float, uint32_t);
def_binary_search(float, int32_t);
def_binary_search(float, float);

float fuzzy_1_dimensional(float input, uint8_t fuzzy_size, int32_t *fuzzy_rule, int32_t *input_section)
{
    float membership, result;
    uint16_t index = binary_search__V_float_A_int32_t(input, input_section, fuzzy_size);
    if (index == fuzzy_size - 1 || index == 0)
    {
        membership = 1;
        result = fuzzy_rule[index];
    }
    else
    {
        membership = (input_section[index + 1] - input) / (input_section[index + 1] - input_section[index]);
        result = fuzzy_rule[index] * membership + fuzzy_rule[index + 1] * (1 - membership);
    }
    return result;
}

float fuzzy_2_dimensional(float in1, float in2, uint8_t size1, uint8_t size2, float *fuzzy_rule, float *input_section1, float *input_section2)
{
    return 0;
}
