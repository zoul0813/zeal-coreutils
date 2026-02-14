#include <stdint.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>
#include <zos_video.h>
#include <zvb_hardware.h>

#include <core.h>
#include "ansi.h"


#define ANSI_STATE_NORMAL 0x0
#define ANSI_STATE_ESC    0x1
#define ANSI_STATE_CSI    0x2

#define SCREEN_COL80_WIDTH  80
#define SCREEN_COL80_HEIGHT 40
#define SCREEN_SIZE         (80 * 40)

uint8_t mmu_page_current;
const __sfr __banked __at(0xF2) mmu_page2_ro;
__sfr __at(0xF2) mmu_page2;
uint8_t __at(0x8000) screen[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];
uint8_t __at(0x9000) palette[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];

static inline void text_map_vram(void)
{
    mmu_page_current = mmu_page2_ro;
    mmu_page2        = VID_MEM_PHYS_ADDR_START >> 14;
}

static inline void text_demap_vram(void)
{
    mmu_page2 = mmu_page_current;
}

// --- parser state ---
static uint8_t ansi_state = 0;
static uint8_t ansi_buf[ANSI_BUF_MAX];
static uint8_t ansi_len = 0;
static uint8_t ansi_fg  = TEXT_COLOR_WHITE;
static uint8_t ansi_bg  = TEXT_COLOR_BLACK;
static uint8_t ansi_gm  = 0; // graphics mode
static uint8_t ansi_cx  = 0;
static uint8_t ansi_cy  = 0;

uint8_t colors[] = {
    // normal colors
    TEXT_COLOR_BLACK, TEXT_COLOR_DARK_RED, TEXT_COLOR_DARK_GREEN, TEXT_COLOR_BROWN, TEXT_COLOR_DARK_BLUE,
    TEXT_COLOR_DARK_MAGENTA, TEXT_COLOR_DARK_CYAN, TEXT_COLOR_LIGHT_GRAY,

    // bright colors
    TEXT_COLOR_DARK_GRAY, TEXT_COLOR_RED, TEXT_COLOR_GREEN, TEXT_COLOR_YELLOW, TEXT_COLOR_BLUE, TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN, TEXT_COLOR_WHITE};

void hw_set_fg(uint8_t color)
{
    if ((ansi_gm == ANSI_GRAPHICS_BOLD) && (color < 8)) {
        ansi_fg = colors[color + 8];
    } else if ((ansi_gm == ANSI_GRAPHICS_DIM) && (color > 7)) {
        ansi_fg = colors[color - 8];
    } else {
        ansi_fg = colors[color];
    }
    SET_COLORS(ansi_fg, ansi_bg);
}

void hw_set_bg(uint8_t color)
{
    ansi_bg = colors[color];
    SET_COLORS(ansi_fg, ansi_bg);
}

void hw_cursor_move(uint8_t row, uint8_t col)
{
    row--;
    col--;
    if (row > 40) {
        row = 39;
    } else if (col > 80) {
        col = 79;
    }
    zvb_peri_text_curs_x = col;
    zvb_peri_text_curs_y = row;
}

void hw_cursor_down(uint8_t n)
{
    int16_t y  = zvb_peri_text_curs_y;
    y         += n;
    if (y > 39) {
        zvb_peri_text_curs_y = 39;
    } else {
        zvb_peri_text_curs_y = y;
    }
}

void hw_cursor_forward(uint8_t n)
{
    int16_t x;
    x  = zvb_peri_text_curs_x;
    x += n;
    if (x > 79) {
        zvb_peri_text_curs_x = 79;
    } else {
        zvb_peri_text_curs_x = x;
    }
}

void hw_cursor_up(uint8_t n)
{
    int16_t y;
    y  = zvb_peri_text_curs_y;
    y -= n;
    if (y < 0) {
        zvb_peri_text_curs_y = 0;
    } else {
        zvb_peri_text_curs_y = y;
    }
}

void hw_cursor_back(uint8_t n)
{
    zvb_peri_text_curs_x += n;
    int16_t x;
    x  = zvb_peri_text_curs_x;
    x -= n;
    if (x < 0) {
        zvb_peri_text_curs_x = 0;
    } else {
        zvb_peri_text_curs_x = x;
    }
}

void hw_clear_screen(uint8_t mode)
{
    uint8_t cx = zvb_peri_text_curs_x;
    uint8_t cy = zvb_peri_text_curs_y;
    switch (mode) {
        // case 0: { // cursor to end of screen
        //     // for(uint8_t y = cy; y < SCREEN_COL80_HEIGHT; y++) {
        //     //     for(uint8_t x = 0; x < SCREEN_COL80_WIDTH; x++) {
        //     //         screen[y][x] = 0;
        //     //     }
        //     // }
        // } break;
        // case 1: { // cursor to beginning of screen
        //     // for(uint8_t y = 0; y <= cy; y++) {
        //     //     for(uint8_t x = 0; x <= SCREEN_COL80_WIDTH; x++) {
        //     //         screen[y][x] = 0;
        //     //     }
        //     // }
        // } break;
        case 2: { // entire screen
            text_map_vram();
            for (uint8_t y = 0; y < SCREEN_COL80_HEIGHT; y++) {
                for (uint16_t x = 0; x < SCREEN_COL80_WIDTH; x++) {
                    screen[y][x]  = 0x00;
                    palette[y][x] = COLOR(TEXT_COLOR_LIGHT_GRAY, TEXT_COLOR_BLACK);
                }
            }
            text_demap_vram();
        } break;
        // case 3: { // erase saved lines???
        // } break;
    }
}

void hw_clear_line(uint8_t mode)
{
    uint8_t f  = 0;
    uint8_t cx = zvb_peri_text_curs_x;
    uint8_t cy = zvb_peri_text_curs_y;
    text_map_vram();
    switch (mode) {
        case 1: { // start to cursor
            for (uint16_t x = 0; x <= cx; x++) {
                screen[cy][x]  = 0x00;
                palette[cy][x] = 0x0F;
            }
        } break;
        case 2: { // entire line
            for (uint16_t x = 0; x <= SCREEN_COL80_WIDTH; x++) {
                screen[cy][x]  = 0x00;
                palette[cy][x] = 0x0F;
            }
        } break;
        case 0:    // fall thru
        default: { // line to end
            for (uint16_t x = cx; x < SCREEN_COL80_WIDTH; x++) {
                screen[cy][x]  = 0x00;
                palette[cy][x] = 0x0F;
            }
        } break;
    }
    text_demap_vram();
}

// --- small helpers ---
static unsigned char ansi_param(unsigned char index)
{
    unsigned char i = 0, count = 0, val = 0;
    while (i < ansi_len) {
        if (ansi_buf[i] == ';') {
            if (count == index)
                return val;
            val = 0;
            count++;
        } else {
            val = val * 10 + (ansi_buf[i] - '0');
        }
        i++;
    }
    if (count == index) {
        return val;
    }
    return ANSI_IGNORE;
}

// --- main handler ---
uint8_t ansi_parse(unsigned char c, char response[ANSI_BUF_MAX])
{
    switch (ansi_state) {
        case ANSI_STATE_NORMAL: { // normal
            if (c == ANSI_ESC) {  // ESC
                ansi_state = 1;
                return ANSI_IGNORE;
            }
            response[0] = c;
            response[1] = 0x0;
            return ANSI_PRINT;
        }
        case ANSI_STATE_ESC: { // got ESC
            if (c == ANSI_CSI) {
                ansi_state = 2; // CSI
                ansi_len   = 0;
            } else {
                ansi_state = 0; // ignore
            }
            return ANSI_IGNORE;
        }
        case ANSI_STATE_CSI: { // CSI collecting params
            if (c >= '0' && c <= '9') {
                if (ansi_len < ANSI_BUF_MAX) {
                    ansi_buf[ansi_len++] = c;
                }
            } else if (c == ';') {
                if (ansi_len < ANSI_BUF_MAX) {
                    ansi_buf[ansi_len++] = c;
                }
            } else if (c >= '@' && c <= '~') {
                // dispatch
                switch (c) {
                    case 'A': { // cursor up
                        hw_cursor_up(ansi_param(0) ? ansi_param(0) : 1);
                    } break;
                    case 'B': { // cursor down
                        hw_cursor_down(ansi_param(0) ? ansi_param(0) : 1);
                    } break;
                    case 'C': { // cursor forward
                        hw_cursor_forward(ansi_param(0) ? ansi_param(0) : 1);
                    } break;
                    case 'D': { // cursor back
                        hw_cursor_back(ansi_param(0) ? ansi_param(0) : 1);
                    } break;
                    case 'E': { // move cursor to 0,+n
                        hw_cursor_down(ansi_param(0) ? ansi_param(0) : 1);
                        hw_cursor_back(255);
                    } break;
                    case 'F': { // move cursor to 0,-n
                        hw_cursor_up(ansi_param(0) ? ansi_param(0) : 1);
                        hw_cursor_back(255);
                    } break;
                    case 'G': { // move cursor to x,n
                        hw_cursor_move(zvb_peri_text_curs_x + 1, ansi_param(0) ? ansi_param(0) : 1);
                    } break;
                    case 'H': { // cursor move (x,y)
                        hw_cursor_move(ansi_param(0) ? ansi_param(0) : 1, ansi_param(1) ? ansi_param(1) : 1);
                    } break;
                    case 'J': { // clear screen
                        hw_clear_screen(ansi_param(0) ? ansi_param(0) : 0);
                    } break;
                    case 'K': { // cursor line
                        hw_clear_line(ansi_param(0) ? ansi_param(0) : 0);
                    } break;
                    case 'c': { // device attributes
                        // Device Attributes: respond as VT100 / ANSI
                        str_cpy(response, "\x1B[?1;2c]");
                        return ANSI_SEND;
                        break;
                    } break;
                    case 'n': { // cursor position, negotiation
                        uint8_t param = ansi_param(0);
                        switch (param) {
                            case 5: { // Are you OK?
                                str_cpy(response, "\x1B[0n");
                                return ANSI_SEND;
                            } break;
                            case 6: { // Cursor Position
                                uint8_t x = zvb_peri_text_curs_x + 1;
                                uint8_t y = zvb_peri_text_curs_y + 1;
                                char y_str[6];
                                char x_str[6];
                                itoa(y, y_str, 10, 'A');
                                itoa(x, x_str, 10, 'A');
                                str_cpy(response, "\x1B[");
                                str_cat(response, y_str);
                                str_cat(response, ";");
                                str_cat(response, x_str);
                                str_cat(response, "R");
                                return ANSI_SEND;
                            }
                        }
                    } break;
                    case 'm': { // colors
                        // SGR (Select Graphic Rendition) with multiple params
                        unsigned char i        = 0;
                        unsigned char val      = 0;
                        unsigned char have_val = 0;

                        if (ansi_len == 0) {
                            // No params = same as "0"
                            hw_set_fg(7);
                            hw_set_bg(0);
                            break;
                        }

                        while (i <= ansi_len) {
                            unsigned char c2 = (i < ansi_len) ? ansi_buf[i] : ';'; // fake ';' at end

                            if (c2 >= '0' && c2 <= '9') {
                                val      = val * 10 + (c2 - '0');
                                have_val = 1;
                            } else if (c2 == ';') {
                                if (have_val) {
                                    // --- dispatch one param ---
                                    if (val == 0) {
                                        hw_set_fg(7); // reset to white on black
                                        hw_set_bg(0);
                                    } else if (val == 1) {
                                        // bold
                                        ansi_gm = ANSI_GRAPHICS_BOLD;
                                    } else if (val == 2) {
                                        // dim
                                        ansi_gm = ANSI_GRAPHICS_DIM;
                                    } else if (val == 22) {
                                        // reset bold/dim
                                        ansi_gm = 0;
                                    } else if (val >= 30 && val <= 37) {
                                        hw_set_fg(val - 30); // 0–7
                                    } else if (val >= 40 && val <= 47) {
                                        hw_set_bg(val - 40); // 0–7
                                    } else if (val >= 90 && val <= 97) {
                                        hw_set_fg((val - 90) + 8); // 8–15
                                    } else if (val >= 100 && val <= 107) {
                                        hw_set_bg((val - 100) + 8); // 8–15
                                    }
                                    // other attributes (bold=1, underline=4, etc.) ignored for now
                                }
                                val      = 0;
                                have_val = 0;
                            }
                            i++;
                        }
                    } break;
                    case 's': {
                        ansi_cx = zvb_peri_text_curs_x;
                        ansi_cy = zvb_peri_text_curs_y;
                    } break;
                    case 'u': {
                        zvb_peri_text_curs_x = ansi_cx;
                        zvb_peri_text_curs_y = ansi_cy;
                    } break;
                }
                ansi_state = 0;
                ansi_len   = 0;
            } else {
                // unexpected, reset
                ansi_state = 0;
                ansi_len   = 0;
            }
            return ANSI_IGNORE;
        }
    }

    ansi_state  = 0;
    response[0] = c;
    response[1] = 0x0;
    return ANSI_PRINT;
}

uint8_t ansi_init(void)
{
    text_map_vram();
    return 0;
}

uint8_t ansi_deinit(void)
{
    text_demap_vram();
    return 0;
}

uint8_t ansi_print(char* buffer)
{
    uint8_t n = str_len(buffer);
    for (uint8_t i = 0; i < n; i++) {
        char value = buffer[i];
        switch (value) {
            case ANSI_LF: { // new line
                zvb_peri_text_ctrl |= (1 << ZVB_PERI_TEXT_CTRL_NEXTLINE);
            } break;
            case ANSI_CR: {
                zvb_peri_text_curs_x = 0;
            } break;
            case ANSI_BS: { // backspace
                uint8_t x                = zvb_peri_text_curs_x - 1;
                zvb_peri_text_curs_x     = x;
                zvb_peri_text_print_char = '\0';
                zvb_peri_text_curs_x     = x;
            } break;
            case ANSI_FF: { // form feed (clear screen)
                zos_err_t err = ioctl(DEV_STDOUT, CMD_CLEAR_SCREEN, NULL);
                if (err)
                    return err;
            } break;
            default: {
                zvb_peri_text_print_char = buffer[i];
            }
        }
    }
    return 0;
}
