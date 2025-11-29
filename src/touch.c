#include <zos_errors.h>
#include <zos_vfs.h>
#include <core.h>

zos_err_t err;
zos_dev_t dev;

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: touch <path>\n");
        return ERR_INVALID_PARAMETER;
    }

    dev = open(argv[0], O_WRONLY | O_CREAT);
    if (dev < 0)
        return -dev;
    return close(dev);
}
