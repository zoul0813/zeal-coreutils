#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>

#include "common.h"

typedef enum {
    List_Details = 1 << 0,
    List_Single = 1 << 1,
    List_Hex = 1 << 2,
    List_Kilo = 1 << 3,
} list_options_t;

char root_path[PATH_MAX];
char path[PATH_MAX];
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


    j = min(str_len(entry->d_name), FILENAME_LEN_MAX);
    put_sn(entry->d_name, FILENAME_LEN_MAX);

    for(i = 0; i < FILENAME_LEN_MAX - j; i++) {
        put_c(CH_SPACE);
    }

    put_c(CH_SPACE);
    put_c(CH_SPACE);

    str_cpy(path, root_path);
    str_cat(path, entry->d_name);

    err = stat(path, &zos_stat);
    if(err) {
        put_s("    error");
        itoa(err, buffer, num_base, num_alpha);
        put_c(CH_SPACE);
        put_s(buffer);
        put_c(CH_SPACE);
        put_s(path);
        put_c(CH_NEWLINE);
        return;
    }

    total_size += zos_stat.s_size;

    uint32_t filesize32 = zos_stat.s_size;
    char filesize_suffix = 'B';
    if(((options & List_Kilo) && filesize32 > KILOBYTE) || (filesize32 > (KILOBYTE * 64))) {
        filesize32 = filesize32 / KILOBYTE;
        filesize_suffix = 'K';
    }
    uint16_t filesize16 = filesize32 & 0xFFFF;

    char filesize[8];
    itoa(filesize16, filesize, num_base, num_alpha);

    j = min(str_len(filesize), 8);
    for(;j < 8; j++) {
        put_c(CH_SPACE);
    }


    put_sn(filesize, 8);
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
    put_s("usage: ls [-options] [path]\n");
    put_s("  l - list details\n");
    put_s("  1 - 1 entry per line\n");
    put_s("  x - hex output\n");
    put_s("  k - kilobytes\n");
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
                    case 'k': {
                        options |= List_Kilo;
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
                        put_c(CH_NEWLINE);
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

    if(root_path[0] == '\0') {
        curdir(root_path);
    }

    // TODO: unsupported in native at the moment
    // err = stat(root_path, &zos_stat);
    // if(err) {
    //     put_sn(root_path, PATH_MAX); put_s(" not found\n");
    //     exit(err);
    // }
    //
    // if(D_ISFILE(zos_stat.s_flags)) {
    //     put_s("is file: "); put_s(root_path); put_c(CH_NEWLINE);
    //     str_cpy(dir_entry.d_name, zos_stat.s_name);
    //     dir_entry.d_flags = zos_stat.s_flags;
    //     uint16_t l = str_len(root_path);
    //     char *p = &root_path[l-1];
    //     while(*p-- != '/');
    //     p+=2;
    //     *p = '\0';
    //     details(&dir_entry);
    //     return ERR_SUCCESS;
    // }

    uint16_t l = str_len(root_path);
    if(root_path[l-1] != '/') {
        str_cat(root_path, "/");
    }

    dev = opendir(root_path);
    if(dev < 0) {
        put_sn(root_path, PATH_MAX); put_s(" not a dir\n");
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
            j = min(str_len(dir_entry.d_name), FILENAME_LEN_MAX);
            put_sn(dir_entry.d_name, FILENAME_LEN_MAX);
            if((options & List_Single)) {
                put_c(CH_NEWLINE);
            } else {
                for(i = 0; i < (FILENAME_LEN_MAX + 2) - j; i++) {
                    put_c(CH_SPACE);
                }
                k++;
                if(k > 4) k = 0;
            }
            if((total_entries % 4) == 0) {
                put_c(CH_NEWLINE);
            }
        }
    }

    if(err != ERR_NO_MORE_ENTRIES) {
        exit(err);
    }

    if((options & (List_Single | List_Details)) == 0) {
        if(k < 5) put_c(CH_NEWLINE);
    }

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