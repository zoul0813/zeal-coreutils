#include <stdint.h>

void* str_cpyn(void* dst, const void* src, uint16_t n) {
    uint8_t* d = dst;
    const uint8_t* s = src;
    uint16_t i = 0;

    while(i < n && *s != 0x00) {
        *d++ = *s++;
        i++;
    }
    if (i < n) *d = 0x00;
    return dst;
}

void* str_cpy(void* dst, const void* src) {
    return str_cpyn(dst, src, UINT16_MAX);
}