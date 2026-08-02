	.module window_wherey
	.optsdcc -mz80
    .globl _window_wherey
	.area _TEXT

; uint8_t window_wherey(window_t* w) -- HL = w, return A.
_window_wherey::
    ld bc, #11
    add hl, bc
    ld a, (hl)
    inc hl
    sub a, (hl)
    ret
