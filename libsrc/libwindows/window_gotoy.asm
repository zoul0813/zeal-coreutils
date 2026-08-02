	.module window_gotoy
	.optsdcc -mz80
    .globl _window_gotoy
	.area _TEXT

; void window_gotoy(window_t* w, uint8_t y)
; HL = w, stack = y (one byte). Callee removes y.
_window_gotoy::
    pop de
    pop bc                  ; C = y
    dec sp
    push de
    ex de, hl               ; DE = w
    ld hl, #11
    add hl, de              ; HL = &pos_y
    inc de
    ld a, (de)              ; w->y
    add a, c
    inc hl
    add a, (hl)             ; offset
    dec hl
    ld (hl), a
    ret
