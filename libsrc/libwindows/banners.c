#include <stddef.h>
#include "windows.h"

void _text_banner(uint8_t x, uint8_t y, uint8_t c, uint8_t centered, window_t* w, const char* s)
{
    uint8_t width = 0;
    switch (zvb_ctrl_video_mode) {
        case ZVB_CTRL_VID_MODE_TEXT_320: width = 40; break;
        case ZVB_CTRL_VID_MODE_TEXT_640: width = 80; break;
    }

    if (w != NULL) {
        x = w->x + x;
        y = w->y + y;
        if (x > 0) {
            width = w->w - x - w->_attrs.offset;
        } else {
            width = w->w - x;
        }
    } else {
        width -= x;
    }

    // Calculate visible length (skipping ESC sequences)
    uint8_t len = 0;
    for (uint8_t i = 0; s[i] != 0x00 && len < width; i++) {
        if (s[i] == 0x1B && s[i+1] != 0x00 && s[i+2] != 0x00) {
            i += 2;
        }
        len++;
    }

    char pad = 0;
    if (centered && len < width) {
        pad = (width - len) >> 1;
    }

    if (len > 0) {
        text_map_vram();
        for (uint8_t i = 0; i < pad; i++) {
            TEXT_WRITE(win_NonePtr, x, y, CH_SPACE);
            COLOR_WRITE(win_NonePtr, x, y, c);
            x++;
        }

        uint8_t clr = c;
        uint8_t chars_written = 0;
        for (uint8_t i = 0; s[i] != 0x00 && chars_written < len; i++) {
            char ch = s[i];
            if(ch == 0x1B && s[i+1] != 0x00 && s[i+2] != 0x00) {
                i++;
                clr = s[i];
                i++;
                ch = s[i];
            } else {
                clr = c;
            }

            TEXT_WRITE(win_NonePtr, x, y, ch);
            COLOR_WRITE(win_NonePtr, x, y, clr);
            x++;
            chars_written++;
        }

        for (uint8_t i = 0; i < (width - len - pad); i++) {
            TEXT_WRITE(win_NonePtr, x, y, CH_SPACE);
            COLOR_WRITE(win_NonePtr, x, y, c);
            x++;
        }
        text_demap_vram();
    }
}

void text_banner(uint8_t x, uint8_t y, uint8_t c, uint8_t centered, const char* s)
{
    _text_banner(x, y, c, centered, 0, s);
}

void text_header(uint8_t x, uint8_t y, uint8_t c, const char* s)
{
    _text_banner(x, y, c, 1, 0, s);
}

void text_menu(uint8_t x, uint8_t y, uint8_t c, const char* items)
{
    _text_banner(x, y, c, 0, 0, items);
}

void window_banner(window_t* w, uint8_t x, uint8_t y, uint8_t centered, const char* s)
{
    _text_banner(x, y, COLOR(w->bg, w->fg), centered, w, s);
    w->_attrs.pos_y++;
    if (w->_attrs.pos_y < w->y)
        w->_attrs.pos_y = w->y;
}
