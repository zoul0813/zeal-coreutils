#include <core.h>
#include "dialogs.h"

#define DIALOG_BOTTOM_SHADOW 0xDF

static const dialog_button_style_t default_button_style = {
    TEXT_COLOR_BLACK,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_DARK_RED,
    TEXT_COLOR_DARK_GRAY,
};

static uint8_t dialog_action_width(const dialog_action_t* action)
{
    return str_len(action->label) + 2;
}

static void dialog_put_button(window_t* window,
                              const dialog_action_t* action,
                              const dialog_button_style_t* style)
{
    const char* label = action->label;
    uint8_t highlight = action->highlight_index;
    uint8_t button_color = COLOR(style->fg, style->bg);
    uint8_t highlight_color = COLOR(style->highlight, style->bg);

    window_putc_color(window, ' ', button_color);

    for (uint8_t i = 0; label[i] != 0; i++) {
        if (i == highlight)
            window_putc_color(window, label[i], highlight_color);
        else
            window_putc_color(window, label[i], button_color);
    }

    window_putc_color(window, ' ', button_color);
}

void dialog_draw(dialog_t* dialog)
{
    window_t* win = dialog->window;
    uint8_t actions_width = 0;
    uint8_t border_width = (win->flags & WIN_BORDER) ? 2 : 0;
    uint8_t content_width = win->w - border_width;
    uint8_t content_height = win->h - border_width;
    uint8_t action_x;
    const dialog_button_style_t* button_style = dialog->button_style
        ? dialog->button_style
        : &default_button_style;
    uint8_t right_shadow_color = COLOR(button_style->shadow, button_style->shadow);
    uint8_t bottom_shadow_color = COLOR(button_style->shadow, win->bg);

    window(win);
    window_gotoxy(win, dialog->message_x, dialog->message_y);
    window_puts(win, dialog->message);

    for (uint8_t i = 0; i < dialog->action_count; i++) {
        actions_width += dialog_action_width(&dialog->actions[i]);
        if (i > 0)
            actions_width += dialog->action_spacing;
    }

    /* Include the final right-hand shadow when centering the button row. */
    action_x = actions_width < content_width
        ? (content_width - actions_width - 1) / 2
        : 0;

    for (uint8_t i = 0; i < dialog->action_count; i++) {
        uint8_t width = dialog_action_width(&dialog->actions[i]);

        window_gotoxy(win, action_x, dialog->actions_y);
        dialog_put_button(win, &dialog->actions[i], button_style);

        /* Right edge of the one-cell DOS-style drop shadow. */
        if (action_x + width < content_width)
            window_putc_color(win, ' ', right_shadow_color);

        /* Bottom edge, shifted one cell to the right. */
        if (dialog->actions_y + 1 < content_height) {
            window_gotoxy(win, action_x + 1, dialog->actions_y + 1);
            for (uint8_t shadow = 0; shadow < width; shadow++)
                window_putc_color(win, DIALOG_BOTTOM_SHADOW, bottom_shadow_color);
        }

        action_x += width + dialog->action_spacing;
    }
}

uint8_t dialog_handle_key(const dialog_t* dialog, uint8_t key)
{
    for (uint8_t i = 0; i < dialog->action_count; i++) {
        if (dialog->actions[i].key == key)
            return i;
    }
    return DIALOG_RESULT_NONE;
}
