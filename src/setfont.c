#include <zos_errors.h>
#include <zos_vfs.h>
#include <zvb_hardware.h>
#include <core.h>

#define BUFFER_SIZE 3072

zos_err_t err;
zos_dev_t dev;
uint16_t size;

uint8_t mmu_page_current;
const __sfr __banked __at(0xF0) mmu_page0_ro;
__sfr __at(0xF0) mmu_page0;
uint8_t __at(0x3000) FONT[BUFFER_SIZE];
uint8_t buffer[BUFFER_SIZE];

inline void text_map_vram(void)
{
    __asm__("di");
    mmu_page_current = mmu_page0_ro;
    mmu_page0        = VID_MEM_PHYS_ADDR_START >> 14;
}

inline void text_demap_vram(void)
{
    mmu_page0 = mmu_page_current;
    __asm__("ei");
}

int main(int argc, char** argv)
{
    if (argc == 0) {
        put_s("usage: setfont <font.f12>\n");
        return ERR_INVALID_PARAMETER;
    }

    dev = open(argv[0], O_RDONLY);
    if (dev < 0) {
        put_s(argv[0]);
        put_s(" not found\n");
        return -dev;
    }

    size = BUFFER_SIZE;
    err  = read(dev, buffer, &size);
    if (err != ERR_SUCCESS) {
        put_s(argv[0]);
        put_s(" failed to read\n");
        return err;
    }
    err = close(dev);
    if (err != ERR_SUCCESS) {
        put_s(argv[0]);
        put_s(" failed to close\n");
        return err;
    }

    text_map_vram();
    mem_cpy(FONT, buffer, size);
    text_demap_vram();

    return err;
}
