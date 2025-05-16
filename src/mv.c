#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

zos_err_t err;

zos_err_t copy(const char* src, const char* dst) {
    zos_err_t err;
    zos_dev_t s, d;

    s = open(src, O_RDONLY);
    if(s < 0) return -s;

    d = open(dst, O_WRONLY | O_CREAT);
    if(d < 0) return -d;

    char buffer[1024];
    uint16_t size = 1024;;

    do {
        err = read(s, buffer, &size);
        err = write(d, buffer, &size);
    } while(err == ERR_SUCCESS && size > 0);

    err = close(s);
    if(err != ERR_SUCCESS) return err;
    err = close(d);
    return err;
}

int main(int argc, char** argv) {

    if(argc == 1) {
        char *src = argv[0];
        char *dst = NULL;

        while(*src) {
            if(*src == CH_SPACE) {
                *src = 0;
                dst = src + 1;
            }
            src++;
        }
        src = argv[0];
        err = copy(src, dst);
        if(err != ERR_SUCCESS) exit(err);
        return rm(src);
    } else {
        put_s("usage: cp <src> <dst>\n");
    }

    return ERR_INVALID_PARAMETER;
}