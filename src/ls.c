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
uint16_t total_size = 0;
uint16_t total_entries = 0;
uint8_t writable = 1;

void details(zos_dir_entry_t *entry) {
    // drwx  filename          65386B YYYY-MM-DD HH:mm:ss

    if(D_ISDIR(entry->d_flags)) {
        put_c('d');
    } else {
        put_c('-');
    }

    put_c('r'); // everything is readable

    if(!writable) {
        put_c('-');
    } else {
        put_c('w');
    }

    if(str_ends_with(entry->d_name, ".bin") || str_pos(entry->d_name, '.') < 0) {
        // assume it's not executable because it isn't a "bin"
        put_c('x');
    } else {
        put_c('-');
    }

    put_c(CH_SPACE);


    j = str_len(entry->d_name);
    put_s(entry->d_name);

    for(i = 0; i < FILENAME_LEN_MAX - j; i++) {
        put_c(CH_SPACE);
    }

    err = stat(entry->d_name, &zos_stat);

    total_size += zos_stat.s_size;

    uint32_t filesize32 = zos_stat.s_size;
    char filesize_suffix = 'B';
    if(filesize32 > (KILOBYTE * 64)) {
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
    char* params = argv[0];

    char buff[256];

    if(argc == 1) {
        if(*params == '-') {
            params++;
            while(params) {
                itoa(*params, buff, 16, 'A');
                switch(*params) {
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
                    case CH_SPACE: {
                        while(*params == CH_SPACE) params++;
                        goto parsed;
                    } break;
                }
                params++;
            }
        }
parsed:
        while(*params == CH_SPACE) params++;
        if(*params != 0) {
            str_cpy(cwd, params);
        }

    }

    if(cwd[0] == 0) {
        curdir(cwd);
    }

    dev = opendir(cwd);
    if(dev < 0) {
        put_s(cwd); put_s(" not found\n");
        exit(-dev);
    }

    k = 0;
    total_entries = 0;
    while((err = readdir(dev, &dir_entry)) == ERR_SUCCESS) {
        total_entries++;
        if(total_entries == 1) {
            // test for O_WRONLY, this doesn't work if the first entry is a directory though ...
            zos_dev_t wr = open(dir_entry.d_name, O_WRONLY);
            if(wr < 0 && -wr == ERR_READ_ONLY) {
                writable = 0;
            } else {
                close(wr);
            }
        }

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
    if((options & List_Details)) {
        put_s("total ");
        itoa(total_size, buff, 10, 'A');
        put_s(buff);
        put_s("B, ");
        itoa(total_entries, buff, 10, 'A');
        put_s(buff);
        put_s(" entries\n");
    }
    return close(dev);
}