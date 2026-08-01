#include <core.h>
#include "windows.h"
void window_clreol(window_t* w) {
    uint8_t color = COLOR(w->fg, w->bg);
    uint8_t x     = w->_attrs.pos_x;
    uint8_t y     = w->_attrs.pos_y;
    uint8_t min_x = x;
    uint8_t max_x = (w->x + (w->w - 1)) - w->_attrs.offset;
    uint8_t width = max_x - min_x + 1;
    text_map_vram();
    mem_set(&SCR_TEXT[y][min_x], CH_SPACE, width);
    mem_set(&SCR_COLOR[y][min_x], color, width);
    text_demap_vram();
    w->_attrs.pos_x = w->x + w->_attrs.offset;
    w->_attrs.pos_y++;
}
