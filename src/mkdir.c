#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

typedef enum {
    OPTION_NONE    = 0,
    OPTION_PREFIX  = 1,
    OPTION_VERBOSE = 2,
} option_e;

void usage(void)
{
    put_s("usage: mkdir [-pv] <path>\n");
}

zos_err_t mkdir_recursive(char* path)
{
    zos_stat_t zos_stat;
    zos_err_t err = stat(path, &zos_stat);

    switch(err) {
        case ERR_NO_SUCH_ENTRY: goto create;
        case ERR_SUCCESS: {
            if (D_ISDIR(zos_stat.s_flags))
                return ERR_SUCCESS;
            if (D_ISFILE(zos_stat.s_flags))
                return ERR_NOT_A_DIR;
        } break;
        default:
            return err;
    }

create:
    char* last_sep = NULL;
    for (char* p = path; *p; p++) {
        if (*p == PATH_SEP)
            last_sep = p;
    }

    if (last_sep && last_sep != path) {
        *last_sep = 0;
        err = mkdir_recursive(path);
        *last_sep = PATH_SEP;
        if (err != ERR_SUCCESS)
            return err;
    }
    err = mkdir(path);
    if(err = ERR_ALREADY_EXIST) return ERR_SUCCESS;
    return err;
}

int main(int argc, char** argv)
{
    if (argc == 0) {
        usage();
        return ERR_INVALID_PARAMETER;
    }

    char* params     = argv[0];
    option_e options = OPTION_NONE;

    if (*params == '-') {
        params++;
        while (*params) {
            switch (*params) {
                case 'p': {
                    options |= OPTION_PREFIX;
                } break;
                case 'v': {
                    options |= OPTION_VERBOSE;
                } break;
                case 'h': {
                    usage();
                    return ERR_SUCCESS;
                } break;
                case ' ': goto parsed;
                default: {
                    put_s("Invalid option: ");
                    put_s(params);
                    put_c(CH_NEWLINE);
                    return ERR_INVALID_PARAMETER;
                }
            }
            params++;
        }
    }
parsed:
    while (*params == CH_SPACE) params++;

    zos_err_t result;
    if (options & OPTION_PREFIX) {
        result = mkdir_recursive(params);
    } else {
        result = mkdir(params);
    }

    if (options & OPTION_VERBOSE && result == ERR_SUCCESS) {
        put_s("Created directory: ");
        put_s(params);
        put_c(CH_NEWLINE);
    }

    return result;
}
