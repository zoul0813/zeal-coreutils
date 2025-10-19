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

uint32_t filesize32;
uint16_t filesize16;
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

    d = opendir(path);
    if(d < 0) return -d;

    err = readdir(d, &dir_entry);

    while(err == ERR_SUCCESS) {
        err = readdir(d, &next_dir_entry);

        uint8_t is_dir = D_ISDIR(dir_entry.d_flags);

        filesize_suffix = 'B';

        if(is_dir) {
            total_dirs++;
        } else {
            total_files++;
        }

        if(depth != 0) {
            put_c(CH_TREE_TRUNK);
            for(uint8_t i = depth; i > 0; i--) {
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

        put_sn(dir_entry.d_name, FILENAME_LEN_MAX);

        if(is_dir) {
            put_c('/');
        }
        put_c(CH_NEWLINE);

        if(is_dir) {
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


int main(int argc, char** argv) {
    curdir(cwd);

    if(argc == 1) {
         chdir(argv[0]);
    }

    tree(".", 0);
    chdir(cwd);

    itoa(total_dirs, buffer, 10, 'A');
    put_s(buffer);
    put_s(" dirs, ");
    itoa(total_files, buffer, 10, 'A');
    put_s(buffer);
    put_s(" files\n");

    return ERR_SUCCESS;
}