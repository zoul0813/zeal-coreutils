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
int str_cmp(void* src1, const void* src2);
int str_ends_with(const char *str, const char *suffix);
void* mem_set(void* ptr, uint8_t value, size_t size);
void* mem_cpy(void* dst, const void* src, size_t size);
void* str_cpy(void* dst, const void* src);
void* str_cat(void* dst, const void* src);
void itoa(uint16_t num, char* str, uint8_t base, char alpha);

/** Implementation */

uint16_t put_s(const char* str) {
    uint16_t size = str_len(str);
    zos_err_t err = write(DEV_STDOUT, str, &size);
    if(err != ERR_SUCCESS) exit(err);
    return size;
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

int str_ends_with(const char *str, const char *suffix) {
    if(str == NULL || suffix == NULL) return 0;

    size_t s_len = str_len(str);
    size_t suffix_len = str_len(suffix);

    if(suffix_len > s_len) return 0;

    return str_cmp(str + s_len - suffix_len, suffix) == 0;
}

int str_cmp(void* src1, const void* src2) {
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

    while(s != 0x00) *d++ = *s++;
    *d = 0x00;
    return dst;
}

void* str_cat(void* dst, const void* src) {
    uint8_t* d = dst;
    const uint8_t* s = src;

    while(d != 0x00) *d++;
    while(s != 0x00) *d++ = *s++;

    return dst;
}

// Helper function to convert an integer to a string
void itoa(uint16_t num, char* str, uint8_t base, char alpha) {
    uint16_t i = 0;
    uint16_t is_negative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers only if base is 10
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + alpha : rem + '0';
        num = num / base;
    }

    // Append negative sign for negative numbers
    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

#endif