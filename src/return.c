#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>

#include <core.h>


int main(int argc, char** argv) {
    if(!argc) {
        put_s("invalid args\n");
        return ERR_INVALID_PARAMETER;
    }

    put_s("Exec '");
    put_s(argv[0]);
    put_s("'\n");

    uint8_t retval;
    char *cmd = argv[0];
    char *args = NULL;
    zos_err_t err = exec(EXEC_PRESERVE_PROGRAM, cmd, args, &retval);
    if(err) {
        put_s("error occurred ");
        put_hex(err);
        put_s("\n");
        return ERR_FAILURE;
    }

    put_s("Returned ");
    put_s(" $");
    put_u8(retval);
    put_s("\n");
    return 0;
}