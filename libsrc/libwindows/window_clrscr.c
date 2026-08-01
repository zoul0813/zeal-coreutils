#include <core.h>
#include "windows.h"
void window_clrscr(window_t* w) {
    uint8_t color = COLOR(w->fg, w->bg);
    uint8_t x     = w->x + w->_attrs.offset;
    uint8_t y     = w->y + w->_attrs.offset;
    uint8_t min_x = x;
    uint8_t min_y = y;
    uint8_t max_x = (w->x + (w->w - 1)) - w->_attrs.offset;
    uint8_t max_y = (w->y + (w->h - 1)) - w->_attrs.offset;
    uint8_t width = max_x - min_x + 1;
    text_map_vram();
    for (y = min_y; y <= max_y; y++) {
        mem_set(&SCR_TEXT[y][min_x], CH_SPACE, width);
        mem_set(&SCR_COLOR[y][min_x], color, width);
    }
    text_demap_vram();
}
