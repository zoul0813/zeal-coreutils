#include <stdint.h>

void* str_catn(void* dst, const void* src, uint16_t n) {
    uint8_t* d = dst;
    const uint8_t* s = src;
    uint16_t i = 0;

    // Move d to the end of the destination string
    while (*d != 0x00) d++;

    // Copy up to n characters from src
    while (i < n && *s != 0x00) {
        *d++ = *s++;
        i++;
    }
    *d = 0x00;
    return dst;
}

void* str_cat(void* dst, const void* src) {
    return str_catn(dst, src, UINT16_MAX);
}