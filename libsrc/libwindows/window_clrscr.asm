	.module window_clrscr
	.optsdcc -mz80
    .globl _window_clrscr
    .globl _text_map_vram
    .globl _text_demap_vram
	.area _TEXT

WIN_X      = 0
WIN_Y      = 1
WIN_W      = 2
WIN_H      = 3
WIN_FG     = 4
WIN_BG     = 5
WIN_OFFSET = 12
CH_SPACE   = 0x20

; void window_clrscr(window_t* w)
_window_clrscr::
    push iy
    push ix
    push bc                 ; local rows/color
    ld ix, #0
    add ix, sp
    push hl
    pop iy

    ld a, WIN_BG (iy)
    rlca
    rlca
    rlca
    rlca
    and #0xf0
    ld c, a
    ld a, WIN_FG (iy)
    and #0x0f
    or c
    ld 1 (ix), a            ; color

    ld a, WIN_OFFSET (iy)
    add a, a
    ld c, a
    ld a, WIN_H (iy)
    sub a, c
    ld 0 (ix), a            ; rows = h - 2*offset

    ld a, WIN_X (iy)
    add a, WIN_OFFSET (iy)
    ld c, a
    ld a, WIN_Y (iy)
    add a, WIN_OFFSET (iy)
    ld b, a
    call .address_bc
    call _text_map_vram

.row:
    ld a, WIN_OFFSET (iy)
    add a, a
    ld c, a
    ld a, WIN_W (iy)
    sub a, c
    ld c, a
    ld a, #CH_SPACE
    call .fill8
    ld a, h
    add a, #0x10
    ld h, a
    ld a, WIN_OFFSET (iy)
    add a, a
    ld c, a
    ld a, WIN_W (iy)
    sub a, c
    ld c, a
    ld a, 1 (ix)
    call .fill8
    ld a, h
    sub a, #0x10
    ld h, a
    ld de, #80
    add hl, de
    dec 0 (ix)
    jr nz, .row

    call _text_demap_vram
    pop bc
    pop ix
    pop iy
    ret

.address_bc:
    ld l, b
    ld h, #0
    ld d, h
    ld e, l
    add hl, hl
    add hl, hl
    add hl, de
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl
    ld a, l
    add a, c
    ld l, a
    ret nc
    inc h
    ret

.fill8:
    push af
    ld b, #0
    ld a, c
    or a
    jr z, .fill_empty
    pop af
    push hl
    ld (hl), a
    dec bc
    ld a, b
    or c
    jr z, .fill_done
    push hl
    pop de
    inc de
    ldir
.fill_done:
    pop hl
    ret
.fill_empty:
    pop af
    ret
