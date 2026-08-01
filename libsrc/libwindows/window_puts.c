#include "windows.h"

uint8_t _window_putc_color_mapped(window_t* w, char c, uint8_t color);

uint8_t window_puts_color(window_t* w, const char* s, uint8_t color)
{
    uint8_t lines = 0;
    uint8_t clr = color;
    const char* p = s;

    if (*p == 0)
        return 0;

    text_map_vram();
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
        lines += _window_putc_color_mapped(w, ch, clr);
    }
    text_demap_vram();
    return lines;
}

uint8_t window_puts(window_t* w, const char* s) {
    return window_puts_color(w, s, COLOR(w->fg, w->bg));
}
