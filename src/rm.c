#include <zos_errors.h>
#include <zos_vfs.h>

#include "common.h"

typedef enum {
    OPTION_NONE      = 0,
    OPTION_RECURSIVE = 1,
    OPTION_FORCE     = 2,
    OPTION_VERBOSE   = 4,
} option_e;

void usage(void)
{
    put_s("usage: rm [-rfv] <path>\n");
}

zos_err_t rm_recursive(char* path, option_e options)
{
    zos_stat_t zos_stat;
    zos_err_t err = stat(path, &zos_stat);
    if (err)
        return err;

    // If it's a file, just delete it
    if (D_ISFILE(zos_stat.s_flags)) {
        if (options & OPTION_VERBOSE) {
            put_s("Removing file: ");
            put_s(path);
            put_c(CH_NEWLINE);
        }
        return rm(path);
    }

    zos_dev_t dir_dev;
    dir_dev = opendir(path);
    if (dir_dev < 0) {
        return -dir_dev;
    }

    zos_dir_entry_t dir_entry;
    char full_path[PATH_MAX];

    // Read all directory entries
    while (readdir(dir_dev, &dir_entry) == ERR_SUCCESS) {
        char* p   = full_path;
        char* src = path;
        while (*src) *p++ = *src++; // Copy path

        if (p > full_path && *(p - 1) != PATH_SEP) {
            *p++ = PATH_SEP;
        }

        src = dir_entry.d_name;
        while (*src) *p++ = *src++; // Copy filename
        *p = CH_NULL;                  // Null terminate

        // TODO: Add prompt for user confirmation here when not using -f flag
        // if (!(options & OPTION_FORCE)) {
        //     put_s("Remove ");
        //     put_s(full_path);
        //     put_s("? (y/N): ");
        //     // Read user input and check for 'y' or 'Y'
        //     // If not confirmed, continue to next entry
        // }

        // Recursively delete the entry
        err = rm_recursive(full_path, options);
        if (err != ERR_SUCCESS) {
            close(dir_dev);
            return err;
        }
    }

    close(dir_dev);

    // Now remove the empty directory
    if (options & OPTION_VERBOSE) {
        put_s("Removing directory: ");
        put_s(path);
        put_c(CH_NEWLINE);
    }
    return rm(path);
}

int main(int argc, char** argv)
{
    if (argc == 0) {
        usage();
        return ERR_INVALID_PARAMETER;
    }

    char* params     = argv[0];
    option_e options = OPTION_NONE;

    // Parse options
    if (*params == '-') {
        params++;
        while (*params && *params != ' ') {
            switch (*params) {
                case 'r':
                case 'R': options |= OPTION_RECURSIVE; break;
                case 'f': options |= OPTION_FORCE; break;
                case 'v': options |= OPTION_VERBOSE; break;
                case 'h': usage(); return ERR_SUCCESS;
                default:
                    put_s("Invalid option: ");
                    put_c(*params);
                    put_c(CH_NEWLINE);
                    return ERR_INVALID_PARAMETER;
            }
            params++;
        }
        while (*params == ' ') params++; // Skip spaces
    }

    if (!*params) {
        usage();
        return ERR_INVALID_PARAMETER;
    }

    zos_err_t result;
    if (options & OPTION_RECURSIVE) {
        result = rm_recursive(params, options);
    } else {
        // Regular rm - just delete the file/empty directory
        result = rm(params);
    }

    return result;
}
