	.module window_title
	.optsdcc -mz80
    .globl _window_title
    .globl _str_len
    .globl _min
    .globl _mem_cpy
    .globl _text_map_vram
    .globl _text_demap_vram
	.area _TEXT

WIN_X = 0
WIN_Y = 1
WIN_W = 2
CH_SPACE = 0x20

; void window_title(window_t* w, const char* title)
; HL=w, DE=title. Standalone member: title-only users avoid window renderer.
_window_title::
    ld a, d
    or e
    ret z
    push iy
    push ix
    push de                 ; title
    push hl                 ; w
    push bc                 ; title length
    push bc                 ; copy length / x
    ld ix, #0
    add ix, sp
    push hl
    pop iy

    ex de, hl
    call _str_len
    ld 2 (ix), e
    ld 3 (ix), d
    ld a, e
    add a, #4
    ld c, a
    ld a, WIN_W (iy)
    sub a, c
    ld l, a
    sbc a, a
    ld h, a
    sra h
    rr l
    ld a, l
    add a, WIN_X (iy)
    ld 1 (ix), a

    call _text_map_vram
    ld b, WIN_Y (iy)
    ld c, 1 (ix)
    call .address_bc
    ld (hl), #'['
    inc hl
    ld (hl), #CH_SPACE
    inc hl
    push hl

    ld l, 2 (ix)
    ld h, 3 (ix)
    ld de, #80
    call _min
    ld 0 (ix), e
    ld c, e
    ld b, #0
    ld e, 6 (ix)
    ld d, 7 (ix)
    pop hl
    push bc
    call _mem_cpy
    ld l, e
    ld h, d
    ld c, 0 (ix)
    ld b, #0
    add hl, bc
    ld (hl), #CH_SPACE
    inc hl
    ld (hl), #']'
    call _text_demap_vram

    pop bc
    pop bc
    pop hl
    pop de
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
