#include <zos_errors.h>
#include <inttypes.h>

#include "common.h"

#define CH_TREE_BRANCH  195
#define CH_TREE_LEAF    196
#define CH_TREE_LAST    192
#define CH_TREE_TRUNK   179

const char* cwd[PATH_MAX];

zos_dir_entry_t dir_entry;
zos_dir_entry_t next_dir_entry;
zos_stat_t zos_stat;

uint32_t filesize32;
uint16_t filesize16;
char filesize[8];
char filesize_suffix = 'B';
char errcode[2];

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

    d = opendir(path);
    if(d < 0) return -d;

    err = readdir(d, &dir_entry);

    while(err == ERR_SUCCESS) {
        err = readdir(d, &next_dir_entry);

        filesize_suffix = 'B';

        // if(D_ISFILE(dir_entry.d_flags)) {
        //     err = stat(dir_entry.d_name, &zos_stat);

        //     filesize32 = zos_stat.s_size;

        //     if(filesize32 > (KILOBYTE * 64)) {
        //         filesize32 = filesize32 / KILOBYTE;
        //         filesize_suffix = 'K';
        //     }
        //     filesize16 = filesize32 & 0xFFFF;
        //     itoa(filesize16, filesize, 10, '0');
        // }

        if(depth != 0) {
            put_c(CH_TREE_TRUNK);
            for(uint8_t i = depth; i > 0; i--) {
                // if(i < depth) {
                //     put_c(CH_TREE_TRUNK);
                // }
                put_s("   ");
            }
        }

        if(err != ERR_SUCCESS) {
            put_c(CH_TREE_LAST);
        } else {
            put_c(CH_TREE_BRANCH);
        }
        put_c(CH_TREE_LEAF);
        put_c(CH_SPACE);
        // if(D_ISFILE(dir_entry.d_flags)) {
        //     put_c('[');
        //     put_s(filesize);
        //     put_c(filesize_suffix);
        //     put_c(']');
        //     put_c(CH_SPACE);
        // }

        put_s(dir_entry.d_name);

        if(D_ISDIR(dir_entry.d_flags)) {
            put_c('/');
        }
        put_c(CH_NEWLINE);

        if(D_ISDIR(dir_entry.d_flags)) {
            chdir(dir_entry.d_name);
            tree(".", depth+1);
            chdir("..");
        }

        // current = next
        mem_cpy(&dir_entry, &next_dir_entry, sizeof(zos_dir_entry_t));
    }

check_error:
    if(err != ERR_NO_MORE_ENTRIES) {
        handle_error(err);
    }

    return close(d);
}


int main(void) {
    curdir(cwd);
    tree(cwd, 0);
    chdir(cwd);
    return ERR_SUCCESS;
}