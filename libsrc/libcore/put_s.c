#include <stdint.h>
#include "core.h"

uint16_t put_s(const char* str) {
    return put_sn(str, UINT16_MAX);
}
