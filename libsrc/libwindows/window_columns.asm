	.module window_columns
	.optsdcc -mz80

    .globl _window_columns
    .globl _window
    .globl _text_map_vram
    .globl _text_demap_vram

	.area _TEXT

WIN_X = 0
WIN_Y = 1
WIN_H = 3

CH_TLINEI = 0xc1
CH_VLINE  = 0xb3

;------------------------------------------------------------------------------
; void window_columns(window_t* w, uint8_t* columns, uint8_t count)
;
; SDCC ABI:
;   HL    = w
;   DE    = columns
;   stack = count (one byte)
;
; Draws window first, then each vertical separator using a pointer advanced by
; the 80-byte screen stride. Preserves IX and IY.
;
; IX local frame:
;   0 = columns remaining
;   1 = unused
;   2..3 = current columns pointer
;------------------------------------------------------------------------------
_window_columns::
    pop af                  ; return address
    pop bc                  ; C = count; B = adjacent caller byte
    dec sp                  ; consume only one-byte count argument
    push af

    push iy
    push ix
    push de                 ; local columns pointer
    push bc                 ; local count
    ld ix, #0
    add ix, sp
    push hl
    pop iy                  ; IY = w

    push iy
    pop hl
    call _window
    call _text_map_vram

.column_loop:
    ld a, 0 (ix)
    or a
    jr z, .done

    ld l, 2 (ix)
    ld h, 3 (ix)
    ld a, (hl)              ; A = column-relative x
    inc hl
    ld 2 (ix), l
    ld 3 (ix), h
    call .column_address    ; HL = first interior-row cell

    ld a, WIN_H (iy)
    sub a, #2               ; number of interior rows
    ld b, a
    ld de, #80
    ld a, b
    or a
    jr z, .bottom

.row_loop:
    ld (hl), #CH_VLINE
    add hl, de
    djnz .row_loop

.bottom:
    ld (hl), #CH_TLINEI
    dec 0 (ix)
    jr .column_loop

.done:
    call _text_demap_vram
    pop bc                  ; discard local count
    pop de                  ; discard local columns pointer
    pop ix
    pop iy
    ret

; Input A = column-relative x. Return HL = SCR_TEXT[y + 1][x + column].
; Clobbers AF and DE; preserves BC, IX, IY.
.column_address:
    push af
    ld a, WIN_Y (iy)
    inc a
    ld l, a
    ld h, #0
    add hl, hl              ; row * 2
    add hl, hl              ; row * 4
    add hl, hl              ; row * 8
    add hl, hl              ; row * 16
    ld d, h
    ld e, l
    add hl, hl              ; row * 32
    add hl, hl              ; row * 64
    add hl, de              ; row * 80
    pop af
    add a, WIN_X (iy)
    add a, l
    ld l, a
    ret nc
    inc h
    ret
