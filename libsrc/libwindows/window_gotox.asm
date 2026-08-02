	.module window_gotox
	.optsdcc -mz80
    .globl _window_gotox
	.area _TEXT

; void window_gotox(window_t* w, uint8_t x)
; HL = w, stack = x (one byte). Callee removes x.
_window_gotox::
    pop de
    pop bc                  ; C = x
    dec sp
    push de
    ex de, hl               ; DE = w
    ld hl, #10
    add hl, de              ; HL = &pos_x
    ld a, (de)              ; w->x
    add a, c
    inc hl
    inc hl
    add a, (hl)             ; offset
    dec hl
    dec hl
    ld (hl), a
    ret
