#include <stdint.h>
#include <stddef.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>

#ifndef COMMON_H
#define COMMON_H

#define KILOBYTE    1024

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
void* mem_set(void* ptr, uint8_t value, size_t size);
void* mem_cpy(void* dst, const void* src, size_t size);
void* str_cpy(void* dst, const void* src);
void* str_cat(void* dst, const void* src);
void itoa(int num, char* str, uint8_t base, char alpha);

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
void itoa(int num, char* str, uint8_t base, char alpha) {
    int i = 0;
    int is_negative = 0;

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