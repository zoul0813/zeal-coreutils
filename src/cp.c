#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

zos_err_t err;

zos_err_t copy(const char* src, const char* dst) {
    zos_err_t err;
    zos_dev_t s, d;

    s = open(src, O_RDONLY);
    if(s < 0) {
        put_s("src: "); put_s(src); put_c(CH_SPACE); put_s("not found\n");
        return -s;
    }

    d = open(dst, O_WRONLY | O_CREAT);
    if(d < 0) {
        put_s("dst: "); put_s(dst); put_c(CH_SPACE); put_s("not found\n");
        return -d;
    }

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

void usage(void) {
    put_s("usage: cp <src> <dst>\n");
}

int main(int argc, char** argv) {
    char *src = argv[0];
    char *dst = NULL;

    if(argc == 1) {
        char *p = argv[0];

        while(*p) {
            if(*p == CH_SPACE) {
                *p = 0;
                dst = p + 1;
            }
            p++;
        }
    } else {
        usage();
        return ERR_SUCCESS;
    }

    if(src == NULL || str_len(src) < 1) {
        put_s("invalid src\n");
        usage();
        return ERR_INVALID_PARAMETER;
    }
    if(dst == NULL || str_len(dst) < 1) {
        put_s("invalid dst\n");
        usage();
        return ERR_INVALID_PARAMETER;
    }

    return copy(src, dst);
}