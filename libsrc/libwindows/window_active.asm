	.module window_active
	.optsdcc -mz80
    .globl _window_active
    .globl _text_map_vram
    .globl _text_demap_vram
	.area _TEXT

WIN_X            = 0
WIN_Y            = 1
WIN_W            = 2
WIN_H            = 3
WIN_FG           = 4
WIN_BG           = 5
WIN_FG_HIGHLIGHT = 6
WIN_FLAGS        = 7
WIN_TITLE        = 8

WIN_BORDER       = 0x01
WIN_TITLE_LEFT   = 0x04
WIN_TITLE_RIGHT  = 0x08

; void window_active(window_t* w, uint8_t active)
; HL=w, stack=active (one byte). Standalone member; preserves IX/IY.
;
; IX local frame:
;   0 = selected color
;   1 = loop count / title length
_window_active::
    pop de
    pop bc                  ; C = active
    dec sp
    push de
    push iy
    push ix
    push bc
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
    ld b, a
    ld a, c
    or a
    jr z, .normal_fg
    ld a, WIN_FG_HIGHLIGHT (iy)
    jr .color_ready
.normal_fg:
    ld a, WIN_FG (iy)
.color_ready:
    and #0x0f
    or b
    ld 0 (ix), a

    call _text_map_vram
    ld a, WIN_FLAGS (iy)
    and #WIN_BORDER
    jr z, .title_path

    ; Horizontal border color spans.
    ld b, WIN_Y (iy)
    ld c, WIN_X (iy)
    call .color_address_bc
    ld c, WIN_W (iy)
    ld a, 0 (ix)
    call .fill8

    ld a, WIN_Y (iy)
    add a, WIN_H (iy)
    dec a
    ld b, a
    ld c, WIN_X (iy)
    call .color_address_bc
    ld c, WIN_W (iy)
    ld a, 0 (ix)
    call .fill8

    ; Vertical border color cells, excluding corners.
    ld a, WIN_H (iy)
    sub a, #2
    ld 1 (ix), a
    jr z, .done
    ld b, WIN_Y (iy)
    inc b
    ld c, WIN_X (iy)
    call .color_address_bc
    ld de, #80
.vertical:
    ld a, 0 (ix)
    ld (hl), a
    push hl
    ld a, WIN_W (iy)
    dec a
    ld c, a
    ld b, #0
    add hl, bc
    ld a, 0 (ix)
    ld (hl), a
    pop hl
    add hl, de
    dec 1 (ix)
    jr nz, .vertical
    jr .done

.title_path:
    ld l, WIN_TITLE (iy)
    ld h, WIN_TITLE + 1 (iy)
    ld a, h
    or l
    jr z, .done

    ; Low-byte string length plus four, matching uint8_t C assignment.
    ld c, #0
.length_loop:
    ld a, (hl)
    or a
    jr z, .length_done
    inc hl
    inc c
    jr .length_loop
.length_done:
    ld a, c
    add a, #4
    ld 1 (ix), a            ; len

    ld c, WIN_X (iy)        ; x = min_x
    ld a, WIN_FLAGS (iy)
    and #WIN_TITLE_RIGHT
    jr z, .not_right
    ld a, WIN_W (iy)
    sub a, 1 (ix)
    dec a
    add a, c
    ld c, a
    jr .title_positioned
.not_right:
    ld a, WIN_FLAGS (iy)
    and #WIN_TITLE_LEFT
    jr nz, .title_positioned
    ld a, WIN_W (iy)
    sub a, 1 (ix)
    ld l, a
    sbc a, a
    ld h, a
    sra h
    rr l
    ld a, l
    add a, c
    ld c, a
.title_positioned:
    inc c                   ; current C code colors x+1 through x+len
    ld b, WIN_Y (iy)
    call .color_address_bc
    ld c, 1 (ix)
    ld a, 0 (ix)
    call .fill8

.done:
    call _text_demap_vram
    pop bc
    pop ix
    pop iy
    ret

; B=y, C=x -> HL=SCR_COLOR+y*80+x.
.color_address_bc:
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
    jr nc, .color_plane
    inc h
.color_plane:
    ld a, h
    add a, #0x10
    ld h, a
    ret

; Fill C bytes at HL with A; return original HL.
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
