	.module window_gotoxy
	.optsdcc -mz80
    .globl _window_gotoxy
	.area _TEXT

; void window_gotoxy(window_t* w, uint8_t x, uint8_t y)
; HL = w, stack = x,y. Callee removes both bytes.
_window_gotoxy::
    pop de
    pop bc                  ; C = x, B = y
    push de
    ex de, hl               ; DE = w

    ld hl, #WIN_POS_X
    add hl, de              ; HL = &pos_x
    ld a, (de)              ; w->x
    add a, c
    inc hl
    inc hl                  ; HL = &offset
    add a, (hl)
    dec hl
    dec hl                  ; HL = &pos_x
    ld (hl), a

    inc de                  ; DE = &w->y
    inc hl                  ; HL = &pos_y
    ld a, (de)
    add a, b
    inc hl                  ; HL = &offset
    add a, (hl)
    dec hl                  ; HL = &pos_y
    ld (hl), a
    ret

WIN_POS_X  = 10
