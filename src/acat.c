#include <stdio.h>
#include <string.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>

#include <ansi.h>

char buffer[1024];
char output[ANSI_BUF_MAX];
uint16_t size;

int main(int argc, char** argv) {
    (void*)argc;
    (void*)argv;

    zos_dev_t f = open(argv[0], O_RDONLY);
    if(f < 0) {
        printf("ERROR: Failed to open file: %d\n", -f);
        return -f;
    }

    while(1) {
        size = sizeof(buffer);
        zos_err_t err = read(f, buffer, &size);
        if(size < 1) break;
        for(uint16_t i = 0; i < size; i++) {
            char c = buffer[i];
            uint8_t n = ansi_parse(c, output);
            if(n == ANSI_PRINT) {
                ansi_print(output);
            }
        }
    }

    close(f);

    printf("\n");
    return 0;
}