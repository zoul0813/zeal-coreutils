/**
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DIALOGS_H
#define DIALOGS_H

#include <stdint.h>
#include "windows.h"

#define DIALOG_RESULT_NONE       0xFF
#define DIALOG_NO_HIGHLIGHT      0xFF
/* Retained for source compatibility; actions are now always drawn as buttons. */
#define DIALOG_ACTION_LABEL_BOX  (1 << 0)

/** One selectable action displayed by a dialog. */
typedef struct {
    const char* label;
    uint8_t key;
    uint8_t highlight_index;
    uint8_t flags;
} dialog_action_t;

/** Colors used to draw DOS-style dialog action buttons. */
typedef struct {
    uint8_t fg;
    uint8_t bg;
    uint8_t highlight;
    uint8_t shadow;
} dialog_button_style_t;

/** A caller-owned dialog definition. */
typedef struct {
    window_t* window;
    const char* message;
    const dialog_action_t* actions;
    uint8_t action_count;
    uint8_t message_x;
    uint8_t message_y;
    uint8_t actions_y;
    uint8_t action_spacing;
    /** NULL selects the default DOS-style button colors. */
    const dialog_button_style_t* button_style;
} dialog_t;

/** Draw window, message, and centered actions. */
void dialog_draw(dialog_t* dialog);

/** Return matching action index, or DIALOG_RESULT_NONE. */
uint8_t dialog_handle_key(const dialog_t* dialog, uint8_t key);

#endif
