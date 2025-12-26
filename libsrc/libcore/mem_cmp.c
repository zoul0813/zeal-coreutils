#include <stdint.h>
#include <stddef.h>

int mem_cmp(const void* p1, const void* p2, size_t size)
{
    const uint8_t* a = p1;
    const uint8_t* b = p2;

    while (size--) {
        uint8_t x = *a++;
        uint8_t y = *b++;
        if (x != y) {
            return x - y;
        }
    }
    return 0;
}