	.module window_putc
	.optsdcc -mz80

    .globl _window_putc
    .globl _window_putc_color
    .globl __window_putc_color_mapped
    .globl _text_map_vram
    .globl _text_demap_vram

	.area _TEXT

; window_t byte offsets
WIN_X       = 0
WIN_W       = 2
WIN_FG      = 4
WIN_BG      = 5
WIN_POS_X   = 10
WIN_POS_Y   = 11
WIN_OFFSET  = 12

CH_TAB      = 0x09
CH_NEWLINE  = 0x0a
CH_SPACE    = 0x20

;------------------------------------------------------------------------------
; uint8_t window_putc(window_t* w, char c)
;
; SDCC ABI:
;   HL    = w
;   stack = c (one byte)
;   A     = returned line count
;
; Builds default color from w->fg/w->bg and delegates to window_putc_color.
;------------------------------------------------------------------------------
_window_putc::
    pop de                  ; return address
    pop bc                  ; C = c; B = adjacent caller byte
    dec sp                  ; consume only one-byte c argument
    push de

    push iy
    push hl
    pop iy
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
    ld b, a                 ; BC = color:c
    pop iy

    push bc
    call _window_putc_color
    ret

;------------------------------------------------------------------------------
; uint8_t window_putc_color(window_t* w, char c, uint8_t color)
;
; SDCC ABI:
;   HL    = w
;   stack = c, color (one byte each)
;   A     = returned line count
;
; Maps VRAM around shared leaf worker. Arguments are removed before return.
;------------------------------------------------------------------------------
_window_putc_color::
    pop de                  ; return address
    pop bc                  ; C = c, B = color
    push de                 ; arguments now consumed

    push bc
    push hl
    call _text_map_vram
    pop hl
    pop bc

    push bc
    call __window_putc_color_mapped

    push af                 ; demap may clobber returned line count
    call _text_demap_vram
    pop af
    ret

;------------------------------------------------------------------------------
; uint8_t _window_putc_color_mapped(window_t* w, char c, uint8_t color)
;
; Internal/exported worker used by window_puts_color while VRAM is already
; mapped. Leaf routine: no C or libcore calls.
;
; SDCC ABI matches window_putc_color. IY and IX are preserved. During worker:
;   IY    = w
;   0(IX) = c
;   1(IX) = color
;   C     = returned line count
;------------------------------------------------------------------------------
__window_putc_color_mapped::
    pop de                  ; return address
    pop bc                  ; C = c, B = color
    push de                 ; arguments now consumed

    push iy
    push ix
    push hl
    pop iy                  ; IY = w
    push bc
    ld ix, #0
    add ix, sp              ; IX = local c/color pair
    ld c, #0                ; lines = 0

    ld a, 0 (ix)
    cp #CH_NEWLINE
    jr z, .newline
    cp #CH_TAB
    jr z, .tab

.character:
    call .address_current
    ld a, 0 (ix)
    ld (hl), a              ; SCR_TEXT[y][x] = c
    ld a, h
    add a, #0x10
    ld h, a
    ld a, 1 (ix)
    ld (hl), a              ; SCR_COLOR[y][x] = color
    inc WIN_POS_X (iy)
    jr .wrap

.newline:
    inc WIN_POS_Y (iy)
    ld a, WIN_X (iy)
    add a, WIN_OFFSET (iy)
    ld WIN_POS_X (iy), a
    inc c
    jr .wrap

.tab:
    ld a, WIN_POS_X (iy)
    sub a, WIN_OFFSET (iy)
    sub a, WIN_X (iy)
    and #0x03
    jr nz, .tab_width_ready
    ld a, #4
.tab_width_ready:
    ld b, a                 ; B = tab_width
    call .address_current
    ld e, WIN_POS_X (iy)    ; E = loop x (preserve existing behavior)

.tab_loop:
    ld a, e
    cp b
    jr nc, .tab_done
    ld (hl), #CH_SPACE
    push hl
    ld a, h
    add a, #0x10
    ld h, a
    ld a, 1 (ix)
    ld (hl), a
    pop hl
    inc hl
    inc e
    jr .tab_loop

.tab_done:
    ld a, WIN_POS_X (iy)
    add a, b
    ld WIN_POS_X (iy), a

.wrap:
    ld a, WIN_X (iy)
    add a, WIN_W (iy)
    dec a
    sub a, WIN_OFFSET (iy)  ; A = maximum writable x
    cp WIN_POS_X (iy)
    jr nc, .done            ; max_x >= pos_x: no wrap

    ld a, WIN_X (iy)
    add a, WIN_OFFSET (iy)
    ld WIN_POS_X (iy), a
    inc WIN_POS_Y (iy)
    inc c

.done:
    ld a, c
    pop bc                  ; discard local c/color pair
    pop ix
    pop iy
    ret

; Calculate SCR_TEXT + (w->pos_y * 80) + w->pos_x.
; Returns HL. Clobbers AF and DE; preserves BC, IX, and IY.
.address_current:
    ld l, WIN_POS_Y (iy)
    ld h, #0
    add hl, hl              ; y * 2
    add hl, hl              ; y * 4
    add hl, hl              ; y * 8
    add hl, hl              ; y * 16
    ld d, h
    ld e, l
    add hl, hl              ; y * 32
    add hl, hl              ; y * 64
    add hl, de              ; y * 80
    ld a, l
    add a, WIN_POS_X (iy)
    ld l, a
    ret nc
    inc h
    ret

; C reference implementation formerly lived in window_putc.c. Keep behavior
; synchronized with windows.h, especially one-byte SDCC arguments, wrapping,
; and existing tab-loop semantics.
