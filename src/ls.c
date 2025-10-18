#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>

#include "common.h"

typedef enum {
    List_Details = 1 << 0,
    List_Single = 1 << 1,
    List_Hex = 1 << 2,
} list_options_t;

const char* root_path[PATH_MAX];
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
char buffer[8];

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
 * usage: ls [-options] [path]
 *   l - list details
 *   1 - 1 entry per line
 *   x - hex output
 */
void usage(void) {
    put_s("usage: ls [-options] [path]"); put_c('\n');
    put_s("  l - list details"); put_c('\n');
    put_s("  1 - 1 entry per line"); put_c('\n');
    put_s("  x - hex output"); put_c('\n');
}

int main(int argc, char **argv) {
    char* params = argv[0];

    if(argc == 1) {
        if(*params == '-') {
            params++;
            while(params) {
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
                    case 'h': {
                        usage();
                        return ERR_SUCCESS;
                    } break;
                    case '\0':
                    case CH_SPACE:
                        goto parsed;
                    default: {
                        put_s("invalid option: ");
                        put_c(*params);
                        put_c('\n');
                        usage();
                        return ERR_INVALID_PARAMETER;
                    } break;
                }
                params++;
            }
        }
parsed:
        while(*params == CH_SPACE) params++;
        if(*params != 0) {
            str_cpy(root_path, params);
        }

    }

    if(root_path[0] == 0) {
        curdir(root_path);
    }


    dev = opendir(root_path);
    if(dev < 0) {
        put_s(root_path); put_s(" not found\n");
        exit(-dev);
    }

    k = 0;
    total_entries = 0;
    while((err = readdir(dev, &dir_entry)) == ERR_SUCCESS) {
        total_entries++;
        if(total_entries == 1) {
            // test for O_WRONLY, this doesn't work if the first entry is a directory though ...
            zos_dev_t wr = open(dir_entry.d_name, O_WRONLY | O_APPEND);
            if(wr < 0 && -wr == ERR_READ_ONLY) {
                writable = 0;
            }
            close(wr);
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
        if(k < 5) put_c(CH_NEWLINE);
    }

    // put_s("\nk = "); put_c('0' + k); put_s("\n");

    if((options & List_Details)) {
        put_s("total ");
        itoa(total_size, buffer, 10, 'A');
        put_s(buffer);
        put_s("B, ");
        itoa(total_entries, buffer, 10, 'A');
        put_s(buffer);
        put_s(" entries\n");
    }

    return close(dev);
}