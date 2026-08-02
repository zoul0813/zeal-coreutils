	.module window_gotoxy
	.optsdcc -mz80
    .globl _window_gotoxy
	.area _TEXT

; void window_gotoxy(window_t* w, uint8_t x, uint8_t y)
; HL = w, stack = x,y. Callee removes both bytes. Preserves IY.
_window_gotoxy::
    pop de
    pop bc                  ; C = x, B = y
    push de
    push iy
    push hl
    pop iy
    ld a, WIN_X (iy)
    add a, c
    add a, WIN_OFFSET (iy)
    ld WIN_POS_X (iy), a
    ld a, WIN_Y (iy)
    add a, b
    add a, WIN_OFFSET (iy)
    ld WIN_POS_Y (iy), a
    pop iy
    ret

WIN_X      = 0
WIN_Y      = 1
WIN_POS_X  = 10
WIN_POS_Y  = 11
WIN_OFFSET = 12
