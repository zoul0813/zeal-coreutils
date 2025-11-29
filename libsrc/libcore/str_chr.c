#include <stddef.h>
#include <stdint.h>
#include "core.h"

char* str_chr(const char* str, char c) {
    int16_t idx = str_pos(str, c);
    if (idx < 0) return NULL;
    return (char*)(str + idx);
}