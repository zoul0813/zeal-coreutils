#include <stddef.h>
#include <stdint.h>
#include "core.h"

char* str_chrr(const char* str, char c) {
    if (!str) return NULL;
    const char* last = NULL;
    while (*str) {
        if (*str == c) last = str;
        str++;
    }
    // Check for '\0' match
    if (c == '\0') return (char*)str;
    return (char*)last;
}