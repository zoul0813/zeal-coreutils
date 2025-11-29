#include "windows.h"

uint8_t window_puts_color(window_t* w, const char* s, uint8_t color)
{
    uint8_t lines = 0;
    uint8_t clr = color;
    const char* p = s;

    while (*p) {
        char ch = *p;
        if (ch == 0x1B && p[1] && p[2]) {
            clr = p[1];
            ch = p[2];
            p += 3;
        } else {
            clr = color;
            ch = *p++;
        }
        lines += window_putc_color(w, ch, clr);
    }
    return lines;
}

uint8_t window_puts(window_t* w, const char* s) {
    return window_puts_color(w, s, COLOR(w->fg, w->bg));
}
