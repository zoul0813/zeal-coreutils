#include <zos_errors.h>
#include <zos_vfs.h>
#include <core.h>

#define BUFFER_SIZE 16

zos_err_t err;
zos_dev_t dev;
uint16_t size;
uint16_t addr;
uint8_t i, j;
uint8_t buffer[BUFFER_SIZE];
uint8_t output[BUFFER_SIZE];

/**
 * TODO:
 *
 * Support the following flags?
 *
 * -c - One-byte hex display
 * -x - Two-byte hex display
 * -b - One-byte octal display
 * -d - Two-byte decimal display
 * -o - Two-byte octal display
 * -n - read only n bytes
 * -s - skip offset bytes
 */

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: hexdump <path>\n");
        return ERR_INVALID_PARAMETER;
    }

    dev = open(argv[0], O_RDONLY);
    if (dev < 0) {
        put_s(argv[0]);
        put_s(" not found\n");
        return -dev;
    }

    addr = 0;
    do {
        // 0000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
        // 0010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
        // 0020:
        //
        itoa(addr, output, 16, 'A');
        j = str_len(output);
        for (; j < 4; j++) {
            put_c('0');
        }
        put_s(output);
        put_s(": ");

        size = BUFFER_SIZE;
        err  = read(dev, buffer, &size);
        if (size < 1)
            break;

        for (i = 0; i < size; i++) {
            u8tohex(buffer[i], output, 'A');
            put_s(output);
            put_c(CH_SPACE);
        }

        put_s("   ");

        i = size;
        for (; i < BUFFER_SIZE; i++) {
            put_s("   ");
        }

        i = 0;
        for (; i < size; i++) {
            if (buffer[i] >= ' ' && buffer[i] <= '~')
                put_c(buffer[i]);
            else
                put_c('.');
        }

        put_c(CH_NEWLINE);

        addr += size;
    } while (err == ERR_SUCCESS && size > 0);

    put_c(CH_NEWLINE);

    err = close(dev);
    if (err != ERR_SUCCESS) {
        put_s(argv[0]);
        put_s(" error reading\n");
    }

    return err;
}
