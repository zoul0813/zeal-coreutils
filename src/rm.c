#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

zos_err_t err;

int main(int argc, char** argv) {
    if(argc == 1) {
        err = rm(argv[0]);
    } else {
        put_s("usage: rm <path_to_dir>\n");
    }

    return err;
}