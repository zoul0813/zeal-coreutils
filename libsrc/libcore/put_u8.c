#include <stdint.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include "core.h"

uint16_t put_u8(uint8_t i) {
    char c[4];
    uint16_t size;
    zos_err_t err;

    itoa(i, c, 10, 'A');
    size = str_len(c);
    err = write(DEV_STDOUT, c, &size);
    if(err != ERR_SUCCESS) exit(err);
    return size;
}
