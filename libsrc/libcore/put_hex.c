#include <stdint.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include "core.h"

uint16_t put_hex(uint16_t i) {
    uint8_t h = (uint8_t)(i >> 8 & 0xFF);
    uint8_t l = (uint8_t)(i & 0xFF);

    return (uint16_t)(put_hex8(h) + put_hex8(l));
}
