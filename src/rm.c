#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

zos_err_t err;

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: rm <path_to_dir>\n");
        return ERR_INVALID_PARAMETER;
    }

    return rm(argv[0]);
}
