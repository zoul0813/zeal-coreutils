// #include <stdio.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>

#include "common.h"

typedef enum {
    List_Details = 1 << 0,
    List_Single = 1 << 1,
    List_Hex = 1 << 2,
} list_options_t;

const char* cwd[PATH_MAX];
zos_dev_t dev;
zos_err_t err;
uint16_t i, j, k;
zos_dir_entry_t dir_entry;
zos_stat_t zos_stat;
list_options_t options;
uint8_t num_base = 10;
char num_alpha = 'A';

void details(zos_dir_entry_t *entry) {
    // filename          65386B YYYY-MM-DD HH:mm:ss

    j = str_len(entry->d_name);
    put_s(entry->d_name);

    for(i = 0; i < FILENAME_LEN_MAX - j; i++) {
        put_c(CH_SPACE);
    }

    err = stat(entry->d_name, &zos_stat);

    uint32_t filesize32 = zos_stat.s_size;
    char filesize_suffix = 'B';
    if(filesize32 > KILOBYTE) {
        filesize32 = filesize32 / KILOBYTE;
        filesize_suffix = 'K';
    }
    uint16_t filesize16 = filesize32 & 0xFFFF;

    char filesize[8];
    itoa(filesize16, filesize, num_base, num_alpha);

    j = str_len(filesize);
    for(;j < 8; j++) {
        put_c(CH_SPACE);
    }

    put_s(filesize);
    put_c(filesize_suffix);
    put_c(CH_NEWLINE);
}

/**
 * usage: ls [-options]
 * l - list details
 * 1 - 1 entry per line
 * x - hex output
 */
int main(int argc, char **argv) {
    curdir(cwd);

    if(argc == 1) {
        if(argv[0][0] != '-') {
            put_s(argv[0]);
            put_c(CH_NEWLINE);
            exit(0);
        }

        char* p = &argv[0];
        while(p) {
            switch(*p) {
                case 'l': {
                    options |= List_Details;
                } break;
                case '1': {
                    options |= List_Single;
                } break;
                case 'x': {
                    options |= List_Hex;
                    num_base = 16;
                } break;
            }
            p++;
        }
    }

    dev = opendir(cwd);
    if(dev < 0) exit(-dev);

    k = 0;
    while((err = readdir(dev, &dir_entry)) == ERR_SUCCESS) {
        if((options & List_Details)) {
            details(&dir_entry);
        } else {
            j = str_len(dir_entry.d_name);
            put_s(dir_entry.d_name);
            if((options & List_Single)) {
                put_c(CH_NEWLINE);
            } else {
                for(i = 0; i < FILENAME_LEN_MAX - j; i++) {
                    put_c(CH_SPACE);
                }
                k++;
                if(k > 4) k = 0;
            }
        }
    }
    if(err != ERR_NO_MORE_ENTRIES) {
        exit(err);
    }
    if((options & (List_Single | List_Details)) == 0) {
        if(j < 3) put_c(CH_NEWLINE);
    }
    return close(dev);
}