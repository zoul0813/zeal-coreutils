#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

zos_err_t err;

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: mkdir <dst>\n");
        return ERR_INVALID_PARAMETER;
    }

    char* src = argv[0];
    char* dst = NULL;

    while (*src) {
        if (*src == CH_SPACE) {
            *src = 0;
            dst  = src + 1;
        }
        src++;
    }
    src = argv[0];

    return mkdir(src);
}
