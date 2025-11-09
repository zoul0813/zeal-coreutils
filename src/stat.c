#include <zos_errors.h>
#include <zos_vfs.h>
#include "common.h"

#define BUFFER_SIZE 1024

zos_err_t err;
zos_stat_t zos_stat;
char buffer[BUFFER_SIZE];

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: stat <path>\n");
        return ERR_INVALID_PARAMETER;
    }

    err = stat(argv[0], &zos_stat);
    if (err) {
        put_s("ERROR: \n");
        put_s(argv[0]);
        put_c(CH_NEWLINE);
        return err;
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
    itoa_pad(zos_stat.s_date.d_year_h, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    itoa_pad(zos_stat.s_date.d_year_l, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    put_c('-');
    itoa_pad(zos_stat.s_date.d_month, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    put_c('-');
    itoa_pad(zos_stat.s_date.d_day, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    put_c(' ');
    itoa_pad(zos_stat.s_date.d_hours, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    put_c(':');
    itoa_pad(zos_stat.s_date.d_minutes, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    put_c(':');
    itoa_pad(zos_stat.s_date.d_seconds, buffer, 16, 'A', '0', 2);
    put_s(buffer);
    put_c(CH_NEWLINE);

    return ERR_SUCCESS;
}
