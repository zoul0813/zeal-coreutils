#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>

#include <core.h>
#include <ansi.h>

zos_err_t err;
zos_dev_t dev;
uint16_t size;

char buffer[1024];
char output[ANSI_BUF_MAX];

int main(int argc, char** argv) {
    if (argc == 0) {
        put_s("usage: acat <path>\n");
        return ERR_INVALID_PARAMETER;
    }

    dev = open(argv[0], O_RDONLY);
    if(dev < 0) {
        put_s("ERROR: Failed to open file: %d\n");
        put_u8(-dev);
        return -dev;
    }

    while(1) {
        size = sizeof(buffer);
        err = read(dev, buffer, &size);
        if(size < 1) break;
        for(uint16_t i = 0; i < size; i++) {
            char c = buffer[i];
            uint8_t n = ansi_parse(c, output);
            if(n == ANSI_PRINT) {
                ansi_print(output);
            }
        }
    }

    err = close(dev);

    put_s("\n");
    return 0;
}