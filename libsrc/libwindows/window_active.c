#include <stddef.h>
#include <core.h>
#include "windows.h"
void window_active(window_t* w, uint8_t active) {
    uint8_t color = active ? COLOR(w->fg_highlight, w->bg) : COLOR(w->fg, w->bg);
    uint8_t min_x = w->x;
    uint8_t min_y = w->y;
    uint8_t max_x = w->x + w->w - 1;
    uint8_t max_y = w->y + w->h - 1;
    uint8_t x, y;

    text_map_vram();

    if(w->flags & WIN_BORDER) {
        mem_set(&SCR_COLOR[min_y][min_x], color, w->w);
        mem_set(&SCR_COLOR[max_y][min_x], color, w->w);
        for(y = min_y + 1; y < max_y; y++) {
            COLOR_WRITE(w, min_x, y, color);
            COLOR_WRITE(w, max_x, y, color);
        }
    } else if(w->title != NULL) {
        uint8_t len = str_len(w->title) + 4;
        x = min_x;
        if(w->flags & WIN_TITLE_RIGHT) {
            x = x + (w->w - len) - 1;
        } else if(!(w->flags & WIN_TITLE_LEFT)) {
            x = x + ((w->w - len) >> 1);
        }
        mem_set(&SCR_COLOR[min_y][x + 1], color, len);
    }

    text_demap_vram();
}
