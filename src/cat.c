#include <zos_errors.h>
#include <zos_vfs.h>
#include <core.h>

#define BUFFER_SIZE 1024

zos_err_t err;
zos_dev_t dev;
uint16_t size;
char buffer[BUFFER_SIZE];


int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: cat <path>\n");
        return ERR_INVALID_PARAMETER;
    }

    dev = open(argv[0], O_RDONLY);
    if (dev < 0) {
        put_s(argv[0]);
        put_s(" not found\n");
        return -dev;
    }

    size = BUFFER_SIZE;
    do {
        err = read(dev, buffer, &size);
        if (size > 0)
            err = write(DEV_STDOUT, buffer, &size);
    } while (err == ERR_SUCCESS && size > 0);
    err = close(dev);
    put_c(CH_NEWLINE);

    if (err != ERR_SUCCESS) {
        put_s(argv[0]);
        put_s(" error reading\n");
    }
    return err;
}
