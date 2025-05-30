#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>

#include "common.h"

#define METADATA_PACKET (0x1c)
#define METADATA_SIZE   (24)
#define ACK_BYTE        (0x06)
#define BLOCK_SIZE      (16 * 1024)
#define BUFFER_SIZE 1024

typedef enum {
    Xfer_Send = 1 << 0,
    Xfer_Receive = 1 << 1,
} xfer_options_t;

typedef struct {
    char marker;
    char filename[FILENAME_LEN_MAX];
    uint8_t blocks;
    uint16_t remainder;
    uint32_t filesize;
} xfer_header_t;

zos_err_t err;
xfer_options_t options;
zos_dev_t uart;
zos_dev_t file;
uint16_t size;
char* path[PATH_MAX];
xfer_header_t metadata;
uint8_t BUFFER[BUFFER_SIZE];

// this should be fine, lol ... as long as xfer remains under 16K :)
uint8_t __at(0x8000) UART_BUFFER[BLOCK_SIZE];

zos_err_t handle_error(zos_err_t error, const char *str, const char *arg) {
    if(error == ERR_SUCCESS) return error;
    put_s("ERROR: ");
    put_s(str);
    if(arg != NULL) {
        put_c(CH_SPACE);
        put_s(arg);
    }
    put_c(CH_NEWLINE);

    // if open, close the uart
    if(uart >= 0) close(uart);
    // if open, close the file
    if(file >= 0) close(file);

    exit(error);
    return error;
}

zos_dev_t init_device(void) {
    uart = open("#SER0", O_RDWR);
    if(uart < 0) {
        handle_error(-uart, "failed to open #SERO", NULL);
    }
    return uart;
}

zos_dev_t init_file(void) {
    if(str_len(path) < 1) {
        handle_error(ERR_INVALID_NAME, "invalid filename", NULL);
    }

    file = open(path, O_RDWR);
    if(file < 0) {
        handle_error(-file, "failed to open", path);
    }
    return file;
}

void ack(void) {
    size = 1;
    UART_BUFFER[0] = ACK_BYTE;
    err = write(uart, UART_BUFFER, &size);
    handle_error(err, "ack", NULL);
}

zos_err_t send(void) {
    handle_error(ERR_NOT_IMPLEMENTED, "send not yet implemented", NULL);

    put_s("sending file: "); put_s(path); put_c(CH_NEWLINE);

    init_device();
    init_file();

    return ERR_NOT_IMPLEMENTED;
}

zos_err_t receive(void) {
    put_s("receiving file: "); put_s(path); put_c(CH_NEWLINE);

    init_device();

    // Receive header from host
    size = METADATA_SIZE;
    err = read(uart, &metadata, &size);
    handle_error(err, "metadata packet", NULL);
    // verify the metadata packet is the correct size
    if(size != METADATA_SIZE) handle_error(ERR_ENTRY_CORRUPTED, "invalid meta packet", NULL);
    // make sure first byte is correct
    if(metadata.marker != METADATA_PACKET) handle_error(ERR_ENTRY_CORRUPTED, "invalid marker", NULL);
    // if user did not set a path override, set it now
    if(str_len(path) == 0) {
        str_cpy(path, metadata.filename);
    }

    // open the file
    init_file();

    // send an ACK to the host, it will guve us time before sending next chunk
    ack();

    while(metadata.blocks > 0) {
        size = BLOCK_SIZE;
        err = read(uart, UART_BUFFER, &size);
        handle_error(err, "read", "block");
        metadata.blocks--;

        err = write(file, UART_BUFFER, &size);
        handle_error(err, "write", "block");
    }

    // get the remaining bytes
    if(metadata.remainder > 0) {
        size = metadata.remainder;
        err = read(uart, UART_BUFFER, &size);
        handle_error(err, "read", "remainder");

        err = write(file, UART_BUFFER, &size);
        handle_error(err, "write", "remainder");
    }

complete:
    // if open, close the uart
    if(uart >= 0) close(uart);
    // if open, close the file
    if(file >= 0) close(file);
    return ERR_SUCCESS;
}

int main(int argc, char** argv) {

    char* params = argv[0];

    if(argc == 1) {
        if(*params == '-') {
            params++;
            while(params) {
                switch(*params) {
                    case 's': {
                        options |= Xfer_Send;
                    } break;
                    case 'r': {
                        options |= Xfer_Receive;
                    } break;
                    case CH_SPACE: {
                        while(*params == CH_SPACE) params++;
                        goto parsed;
                    } break;
                }
                params++;
            }
        }
parsed:
        while(*params == CH_SPACE) params++;
        if(*params != 0) {
            str_cpy(path, params);
        }
    } else {
        put_s("usage: xfer [-s|-r] [<path>]\n");
        put_s("  -s      Send a file\n");
        put_s("  -r      Receive a file\n");
        put_s("  <path>  Path to file\n\n");
    }

    if(options & Xfer_Send) {
        return send();
    }
    if(options & Xfer_Receive) {
        return receive();
    }

    return handle_error(ERR_INVALID_PARAMETER, "invalid parameter", NULL);
}