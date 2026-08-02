	.module window_wherex
	.optsdcc -mz80
    .globl _window_wherex
	.area _TEXT

; uint8_t window_wherex(window_t* w) -- HL = w, return A.
_window_wherex::
    ld bc, #10
    add hl, bc
    ld a, (hl)
    inc hl
    inc hl
    sub a, (hl)
    ret
