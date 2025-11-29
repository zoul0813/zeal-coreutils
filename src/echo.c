#include <zos_errors.h>
#include <zos_vfs.h>
#include <core.h>

zos_err_t err;

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: echo <string>\n");
        return ERR_INVALID_PARAMETER;
    }

    put_s(argv[0]);
    put_c(CH_NEWLINE);
    return ERR_SUCCESS;
}
