#include <stdint.h>
#include <stddef.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>

#ifndef COMMON_H
#define COMMON_H

#define KILOBYTE    (uint32_t)(1024)

#define SCREEN_COL80_WIDTH  80
#define SCREEN_COL80_HEIGHT 40
#define SCREEN_COL40_WIDTH  40
#define SCREEN_COL40_HEIGHT 20

#define CH_NEWLINE      0x0A // \n
#define CH_SPACE        0x20 // Space
#define CH_TAB          0x09 // \t
#define CH_PERIOD       0x2E // .


/** Definitions */
uint16_t put_s(const char* str);
uint16_t put_c(const char c);
uint16_t str_len(const char* str);
int16_t str_pos(const char* str, char c);
int16_t str_cmp(void* src1, const void* src2);
int16_t str_ends_with(const char *str, const char *suffix);
void* mem_set(void* ptr, uint8_t value, size_t size);
void* mem_cpy(void* dst, const void* src, size_t size);
void* str_cpy(void* dst, const void* src);
void* str_cat(void* dst, const void* src);
void itoa(uint16_t num, char* str, uint16_t base, char alpha);
void u8tohex(uint8_t value, char* buffer, char alpha);

/** Implementation */

uint16_t put_sn(const char* str, uint16_t size) {
    uint16_t l = str_len(str);
    uint16_t s = l;
    if(l > size) s = size;
    zos_err_t err = write(DEV_STDOUT, str, &s);
    if(err != ERR_SUCCESS) exit(err);
    return size;
}

uint16_t put_s(const char* str) {
    return put_sn(str, UINT16_MAX);
}

uint16_t max(uint16_t a, uint16_t b) {
    if(a > b) return a;
    return b;
}

uint16_t min(uint16_t a, uint16_t b) {
    if(a < b) return a;
    return b;
}

uint16_t put_c(const char c) {
    uint16_t size = 1;
    zos_err_t err = write(DEV_STDOUT, &c, &size);
    if(err != ERR_SUCCESS) exit(err);
    return size;
}

uint16_t str_len(const char* str)
{
    uint16_t length = 0;
    while (str[length]) length++;
    return length;
}

int16_t str_ends_with(const char *str, const char *suffix) {
    if(str == NULL || suffix == NULL) return 0;

    size_t s_len = str_len(str);
    size_t suffix_len = str_len(suffix);

    if(suffix_len > s_len) return 0;

    return str_cmp(str + s_len - suffix_len, suffix) == 0;
}

int16_t str_cmp(void* src1, const void* src2) {
    const uint8_t* s1 = src1;
    const uint8_t* s2 = src2;

    for(;;) {
        char d = *s2++;
        signed char ret = *s1++ - d;
        if(ret != 0 || d == '\0')
            return ret;
    }
}

int16_t str_pos(const char* str, char c) {
    if (!str) return -1;
    const char* start = str;
    while (*str) {
        if (*str == c) {
            return (int16_t)(str - start);
        }
        ++str;
    }
    return -1;
}

void* mem_set(void* ptr, uint8_t value, size_t size)
{
    uint8_t* p = ptr;
    while (size--) {
        *p++ = value;
    }
    return ptr;
}

void* mem_cpy(void* dst, const void* src, size_t size)
{
    uint8_t* d       = dst;
    const uint8_t* s = src;
    while (size--) *d++ = *s++;
    return dst;
}

void* str_cpy(void* dst, const void* src) {
    uint8_t* d = dst;
    const uint8_t* s = src;

    while(*s != 0x00) *d++ = *s++;
    *d = 0x00;
    return dst;
}

void* str_cat(void* dst, const void* src) {
    uint8_t* d = dst;
    const uint8_t* s = src;

    while(*d != 0x00) *d++;
    while(*s != 0x00) *d++ = *s++;
    *d = 0x00;
    return dst;
}

// Helper function to convert an integer to a string
void itoa(uint16_t num, char* str, uint16_t base, char alpha) {
    uint16_t i = 0, j, k, rem;

    // safety checks
    if(base < 2 || base > 36) {
        str[0] = '\0';
        return;
    }

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Process individual digits
    while (num != 0) {
        rem = num % base;
        str[i++] = (char)((rem > 9) ? (rem - 10) + alpha : rem + '0');
        num = num / base;
    }

    str[i] = '\0';

    // Reverse the string
    for (j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void u8tohex(uint8_t value, char* buffer, char alpha) {
    // Convert high nibble
    uint8_t high = (value >> 4) & 0x0F;
    buffer[0] = (high < 10) ? ('0' + high) : (alpha + high - 10);

    // Convert low nibble
    uint8_t low = value & 0x0F;
    buffer[1] = (low < 10) ? ('0' + low) : (alpha + low - 10);

    buffer[2] = 0; // Null terminator
}


#endif