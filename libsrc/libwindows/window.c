#include <stddef.h>
#include <core.h>
#include "windows.h"

window_t win_None = {
    .flags = WIN_NONE,
};
window_t* win_NonePtr = &win_None;

void window_title(window_t* w, const char* title)
{
    if (title != NULL) {
        /* Draw the window heading */
        uint8_t len = str_len(title) + 4;

        uint8_t x = w->x + ((w->w - len) >> 1);
        uint8_t y = w->y;
        text_map_vram();
        SCR_TEXT[y][x]   = '[';
        SCR_TEXT[y][++x] = ' ';

        for (int i = 0; i < SCREEN_COL80_WIDTH; i++) {
            unsigned char c = title[i];
            if (c == 0x00)
                break;
            SCR_TEXT[y][++x] = c;
        }
        SCR_TEXT[y][++x] = ' ';
        SCR_TEXT[y][++x] = ']';
        text_demap_vram();
    }
}


void window(window_t* w)
{
    uint8_t color = COLOR(w->fg, w->bg);

    w->_attrs.offset = (w->flags & WIN_BORDER) ? 1 : 0;
    w->_attrs.pos_x  = w->x + w->_attrs.offset;
    w->_attrs.pos_y  = w->y + w->_attrs.offset;

    text_map_vram();
    uint8_t y     = w->y;
    uint8_t x     = w->x;
    uint8_t min_x = x;
    uint8_t min_y = y;
    uint8_t max_x = (x + w->w - 1);
    uint8_t max_y = (y + w->h - 1);

    for (y = min_y; y <= max_y; y++) {
        for (x = min_x; x <= max_x; x++) {
            char c = CH_SPACE;
            TEXT_WRITE(w, x, y, c);
            COLOR_WRITE(w, x, y, color);
        }
    }

    // OPTIMIZE: move this into a separate block with loops like before...?
    if ((w->flags & WIN_BORDER) > 0) {
        x = min_x;
        y = min_y;

        TEXT_WRITE(w, x, y, CH_ULCORNER);
        TEXT_WRITE(w, max_x, y, CH_URCORNER);
        TEXT_WRITE(w, x, max_y, CH_LLCORNER);
        TEXT_WRITE(w, max_x, max_y, CH_LRCORNER);

        for (x++; x <= max_x - 1; x++) {
            TEXT_WRITE(w, x, y, CH_HLINE);
            TEXT_WRITE(w, x, max_y, CH_HLINE);
        }

        x = w->x;
        for (y++; y <= max_y - 1; y++) {
            TEXT_WRITE(w, x, y, CH_VLINE);
            TEXT_WRITE(w, max_x, y, CH_VLINE);
        }
    }

    if ((w->flags & WIN_SHADOW) > 0) {
        // draw the shadow
        // let's assume all shadows are black for now?
        x = min_x + w->w;
        min_x++;
        min_y++;
        max_x++;
        for (y = min_y; y <= max_y; y++) {
            TEXT_WRITE(w, x, y, CH_SPACE);
            COLOR_WRITE(w, x, y, COLOR(w->fg, TEXT_COLOR_BLACK));
        }

        for (x = min_x; x <= max_x; x++) {
            TEXT_WRITE(w, x, y, CH_SPACE);
            COLOR_WRITE(w, x, y, COLOR(w->fg, TEXT_COLOR_BLACK));
        }
    }
    text_demap_vram();

    window_title(w, w->title);
}
