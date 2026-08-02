	.module window
	.optsdcc -mz80

    .globl _window
    .globl _window_title
    .globl _text_map_vram
    .globl _text_demap_vram

	.area _TEXT

WIN_X       = 0
WIN_Y       = 1
WIN_W       = 2
WIN_H       = 3
WIN_FG      = 4
WIN_BG      = 5
WIN_FLAGS   = 7
WIN_TITLE   = 8
WIN_POS_X   = 10
WIN_POS_Y   = 11
WIN_OFFSET  = 12

WIN_BORDER  = 0x01
WIN_SHADOW  = 0x02

CH_SPACE    = 0x20
CH_ULCORNER = 0xda
CH_URCORNER = 0xbf
CH_LLCORNER = 0xc0
CH_LRCORNER = 0xd9
CH_HLINE    = 0xc4
CH_VLINE    = 0xb3

;------------------------------------------------------------------------------
; void window(window_t* w)
;
; Draw base planes, optional border and shadow, then title. IY holds window_t.
; Fixed-width fills are inlined with seeded LDIR to avoid repeated C-call ABI
; overhead inside row loops.
;
; IX local frame:
;   0 = loop counter
;   1 = window color
;------------------------------------------------------------------------------
_window::
    push iy
    push ix
    push bc
    ld ix, #0
    add ix, sp
    push hl
    pop iy                  ; IY = w

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
    ld 1 (ix), a

    ld a, WIN_FLAGS (iy)
    and #WIN_BORDER
    jr z, .no_offset
    ld a, #1
.no_offset:
    ld WIN_OFFSET (iy), a
    ld c, a
    add a, WIN_X (iy)
    ld WIN_POS_X (iy), a
    ld a, WIN_Y (iy)
    add a, c
    ld WIN_POS_Y (iy), a

    call _text_map_vram

    ; Base rectangle: w rows in text and color planes.
    ld b, WIN_Y (iy)
    ld c, WIN_X (iy)
    call .address_bc
    ld a, WIN_H (iy)
    ld 0 (ix), a
.base_row:
    ld c, WIN_W (iy)
    ld a, #CH_SPACE
    call .fill8
    ld a, h
    add a, #0x10
    ld h, a
    ld c, WIN_W (iy)
    ld a, 1 (ix)
    call .fill8
    ld a, h
    sub a, #0x10
    ld h, a
    ld de, #80
    add hl, de
    dec 0 (ix)
    jr nz, .base_row

    ld a, WIN_FLAGS (iy)
    and #WIN_BORDER
    call nz, .draw_border

    ld a, WIN_FLAGS (iy)
    and #WIN_SHADOW
    call nz, .draw_shadow

    call _text_demap_vram
    ld e, WIN_TITLE (iy)
    ld d, WIN_TITLE + 1 (iy)
    push iy
    pop hl
    call _window_title

    pop bc
    pop ix
    pop iy
    ret

; Draw border text plane. Assumes mapped VRAM and valid width/height.
.draw_border:
    ld b, WIN_Y (iy)
    ld c, WIN_X (iy)
    call .address_bc        ; HL = top-left
    push hl
    ld (hl), #CH_ULCORNER
    ld a, WIN_W (iy)
    dec a
    ld e, a
    ld d, #0
    add hl, de
    ld (hl), #CH_URCORNER
    pop hl

    push hl
    ld de, #80
    ld a, WIN_H (iy)
    dec a
    ld b, a
.border_bottom_seek:
    add hl, de
    djnz .border_bottom_seek
    push hl                 ; bottom-left
    ld (hl), #CH_LLCORNER
    ld a, WIN_W (iy)
    dec a
    ld e, a
    ld d, #0
    add hl, de
    ld (hl), #CH_LRCORNER
    pop hl                  ; bottom-left
    inc hl
    ld a, WIN_W (iy)
    sub a, #2
    ld c, a
    ld a, #CH_HLINE
    call .fill8
    pop hl                  ; top-left
    inc hl
    ld a, WIN_W (iy)
    sub a, #2
    ld c, a
    ld a, #CH_HLINE
    call .fill8

    ld b, WIN_Y (iy)
    inc b
    ld c, WIN_X (iy)
    call .address_bc        ; first interior left edge
    ld a, WIN_H (iy)
    sub a, #2
    ld 0 (ix), a
    ret z
    ld de, #80
.border_vertical:
    ld (hl), #CH_VLINE
    push hl
    ld a, WIN_W (iy)
    dec a
    ld c, a
    ld b, #0
    add hl, bc
    ld (hl), #CH_VLINE
    pop hl
    add hl, de
    dec 0 (ix)
    jr nz, .border_vertical
    ret

; Draw black shadow column and bottom row.
.draw_shadow:
    ld b, WIN_Y (iy)
    inc b
    ld a, WIN_X (iy)
    add a, WIN_W (iy)
    ld c, a
    call .address_bc
    ld a, WIN_H (iy)
    dec a
    ld 0 (ix), a
    ld de, #80
.shadow_column:
    ld (hl), #CH_SPACE
    push hl
    ld a, h
    add a, #0x10
    ld h, a
    ld a, WIN_FG (iy)
    and #0x0f
    ld (hl), a
    pop hl
    add hl, de
    dec 0 (ix)
    jr nz, .shadow_column

    ld b, WIN_Y (iy)
    ld a, WIN_H (iy)
    add a, b
    ld b, a
    ld a, WIN_X (iy)
    inc a
    ld c, a
    call .address_bc
    ld c, WIN_W (iy)
    ld a, #CH_SPACE
    call .fill8
    ld a, h
    add a, #0x10
    ld h, a
    ld c, WIN_W (iy)
    ld a, WIN_FG (iy)
    and #0x0f
    call .fill8
    ret

; Input B=y, C=x. Return HL = SCR_TEXT + y*80+x. Clobbers AF, DE.
.address_bc:
    ld l, b
    ld h, #0
    ld d, h
    ld e, l
    add hl, hl              ; y*2
    add hl, hl              ; y*4
    add hl, de              ; y*5
    add hl, hl              ; y*10
    add hl, hl              ; y*20
    add hl, hl              ; y*40
    add hl, hl              ; y*80
    ld a, l
    add a, c
    ld l, a
    ret nc
    inc h
    ret

; Fill C bytes at HL with A. Return original HL. Clobbers AF, BC, DE.
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
