	.module window_clreol
	.optsdcc -mz80
    .globl _window_clreol
    .globl _text_map_vram
    .globl _text_demap_vram
    .globl __address_bc
    .globl __fill8
	.area _TEXT

WIN_X      = 0
WIN_W      = 2
WIN_FG     = 4
WIN_BG     = 5
WIN_POS_X  = 10
WIN_POS_Y  = 11
WIN_OFFSET = 12
CH_SPACE   = 0x20

; void window_clreol(window_t* w)
_window_clreol::
    push iy
    push hl
    pop iy
    call _text_map_vram

    ld b, WIN_POS_Y (iy)
    ld c, WIN_POS_X (iy)
    call __address_bc

    ld a, WIN_X (iy)
    add a, WIN_W (iy)
    sub a, WIN_OFFSET (iy)
    sub a, WIN_POS_X (iy)   ; width = x + w - offset - pos_x
    ld c, a
    ld a, #CH_SPACE
    call __fill8

    ld a, h
    add a, #0x10
    ld h, a
    ld a, WIN_X (iy)
    add a, WIN_W (iy)
    sub a, WIN_OFFSET (iy)
    sub a, WIN_POS_X (iy)
    ld c, a
    ld a, WIN_BG (iy)
    rlca
    rlca
    rlca
    rlca
    and #0xf0
    ld b, a
    ld a, WIN_FG (iy)
    and #0x0f
    or b
    call __fill8

    call _text_demap_vram
    ld a, WIN_X (iy)
    add a, WIN_OFFSET (iy)
    ld WIN_POS_X (iy), a
    inc WIN_POS_Y (iy)
    pop iy
    ret
