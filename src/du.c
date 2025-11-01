#include <zos_errors.h>
#include <inttypes.h>

#include "common.h"

#define CH_TREE_BRANCH  195
#define CH_TREE_LEAF    196
#define CH_TREE_LAST    192
#define CH_TREE_TRUNK   179

const char* cwd[PATH_MAX];

zos_dir_entry_t dir_entry;
zos_stat_t zos_stat;

char filesize[8];
char filesize_suffix = 'B';
char errcode[2];

uint16_t total_dirs = 0;
uint16_t total_files = 0;
char buffer[8];

void handle_error(zos_err_t code) {
    put_s("ERROR[$");
    u8tohex(code, errcode, 'A');
    put_s(errcode);
    put_s("]: error occured\n");
    exit(code);
}

zos_err_t tree(const char* path, int depth) {
    zos_dev_t d;
    zos_err_t err;
    uint16_t l = str_len(path);
    char* last = &path[l];

    uint32_t filesize32;

    d = opendir(path);
    if(d < 0) return -d;

    while((err = readdir(d, &dir_entry)) == ERR_SUCCESS) {
        uint8_t is_dir = D_ISDIR(dir_entry.d_flags);

        if(is_dir) {
            total_dirs++;
            if(*last != '/') str_cat(cwd, "/");
            str_cat(cwd, dir_entry.d_name);
            tree(cwd, depth+1);
            *last = '\0';
        } else {
            total_files++;
            if(*last != '/') str_cat(cwd, "/");
            str_cat(cwd, dir_entry.d_name);
            zos_err_t serr = stat(cwd, &zos_stat);
            *last = '\0';
            if(serr != ERR_SUCCESS) {
                put_s("stat failed: ");
                put_s(zos_stat.s_name);
                put_c(CH_NEWLINE);
                handle_error(serr);
            }
            filesize32 += zos_stat.s_size;
        }
    }

check_error:
    if(err != ERR_NO_MORE_ENTRIES) {
        handle_error(err);
    }

    char filesize_suffix = 'B';
    if(filesize32 > (KILOBYTE * 64)) {
        filesize32 = filesize32 / KILOBYTE;
        filesize_suffix = 'K';
    }

    filesize_suffix = 'B';
    uint16_t filesize16 = filesize32 & 0xFFFF;

    char filesize[8];
    itoa(filesize16, filesize, 10, '0');
    put_s(filesize);
    put_c(filesize_suffix);

    uint8_t i, len;
    len = str_len(filesize);
    for(i = len; i < 8; i++) {
        put_c(CH_SPACE);
    }

    put_s(path);
    put_c(CH_NEWLINE);

    return close(d);
}


int main(int argc, char** argv) {
    curdir(cwd);

    if(argc == 1) {
        str_cat(cwd, argv[0]);
    }

    tree(cwd, 0);

    itoa(total_dirs, buffer, 10, 'A');
    put_s(buffer);
    put_s(" dirs, ");
    itoa(total_files, buffer, 10, 'A');
    put_s(buffer);
    put_s(" files\n");

    return ERR_SUCCESS;
}