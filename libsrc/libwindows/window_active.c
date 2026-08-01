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
        for(x = min_x; x <= max_x; x++) {
            COLOR_WRITE(w, x, min_y, color);
            COLOR_WRITE(w, x, max_y, color);
        }
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
        for(uint8_t i = 0; i < len; i++) {
            x++;
            COLOR_WRITE(w, x, min_y, color);
        }
    }

    text_demap_vram();
}
