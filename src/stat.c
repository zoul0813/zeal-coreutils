#include <zos_errors.h>
#include <zos_vfs.h>
#include <core.h>

#define BUFFER_SIZE 1024

typedef enum {
    Stat_None = 1 << 0,
    Stat_Dir  = 1 << 1,
    Stat_File = 1 << 2
} stat_options_t;

zos_err_t err;
zos_stat_t zos_stat;
char buffer[BUFFER_SIZE];

void usage(void)
{
    put_s("usage: stat [-fd] <path>\n");
}

int main(int argc, char** argv)
{
    if (argc == 0) {
        usage();
        return ERR_INVALID_PARAMETER;
    }

    stat_options_t options = Stat_None;
    char* path;
    char* params = argv[0];
    if (*params == '-') {
        params++;
        while (*params) {
            switch (*params) {
                case 'f': {
                    options |= Stat_File;
                } break;
                case 'd': {
                    options |= Stat_Dir;
                } break;
                case 'h': {
                    usage();
                    return ERR_SUCCESS;
                } break;
                case CH_NULL:
                case CH_SPACE: goto parsed;
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
    while (*params == CH_SPACE) params++;
    if (*params != 0) {
        path = params;
    }

    err = stat(path, &zos_stat);
    if (err) {
        put_s("ERROR: \n");
        put_s(path);
        put_c(CH_NEWLINE);
        return err;
    }

    if ((options & Stat_Dir)) {
        return D_ISDIR(zos_stat.s_flags) ? ERR_SUCCESS : ERR_NOT_A_DIR;
    }
    if ((options & Stat_File)) {
        return D_ISFILE(zos_stat.s_flags) ? ERR_SUCCESS : ERR_NOT_A_FILE;
    }

    // zos_stat.s_name
    put_s("Name: ");
    put_sn(zos_stat.s_name, FILENAME_LEN_MAX);
    put_c(CH_NEWLINE);
    // zos_stat.s_flags;
    if (D_ISDIR(zos_stat.s_flags)) {
        put_s("Type: dir\n");
    } else {
        put_s("Type: file\n");
    }

    itoa(zos_stat.s_size, buffer, 10, '0');
    put_s("Size: ");
    put_s(buffer);
    put_c(CH_NEWLINE);


    put_s("Date: ");
    put_date(&zos_stat.s_date, buffer);
    put_c(CH_NEWLINE);

    return ERR_SUCCESS;
}
