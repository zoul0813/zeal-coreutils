#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

zos_err_t err;

typedef enum {
    OPTION_NONE = 0,
    OPTION_PREFIX = 1,
    OPTION_VERBOSE = 2,
} option_e;

void usage(void) {
    put_s("usage: mkdir [-pv] dir_name\n");
}

int main(int argc, char** argv) {
    if(argc == 0) {
        usage();
        return ERR_INVALID_PARAMETER;
    }

    char* params = argv[0];
    option_e options = OPTION_NONE;

    if(*params == '-') {
        params++;
        while(params) {
            switch(*params) {
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
                case '\0':
                case ' ':
                    goto parsed;
                default: {
                    put_s("Invalid option: ");
                    put_s(params);
                    put_c(CH_NEWLINE);
                    return ERR_INVALID_PARAMETER;
                }
            }
        }
    }
parsed:
    while(*params == CH_SPACE) params++;
    char *src = params;
    return mkdir(src);
}
