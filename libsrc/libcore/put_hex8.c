#include <stdint.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include "core.h"

uint16_t put_hex8(uint8_t i) {
    char c[3];
    uint16_t size;
    zos_err_t err;

    u8tohex(i, c, 'A');
    size = 2;
    err = write(DEV_STDOUT, c, &size);
    if(err != ERR_SUCCESS) exit(err);

    return 2;
}