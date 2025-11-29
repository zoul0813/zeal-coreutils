#include <stdint.h>

int16_t str_cmpn(const void* src1, const void* src2, uint16_t n) {
    const uint8_t* s1 = src1;
    const uint8_t* s2 = src2;
    uint16_t i = 0;

    while (i < n) {
        char c1 = *s1++;
        char c2 = *s2++;
        if (c1 != c2)
            return (signed char)(c1 - c2);
        if (c1 == '\0')
            return 0;
        i++;
    }
    return 0;
}

int16_t str_cmp(void* src1, const void* src2) {
    return str_cmpn(src1, src2, UINT16_MAX);
}