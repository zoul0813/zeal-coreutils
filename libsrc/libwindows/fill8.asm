	.module fill8
	.optsdcc -mz80
    .globl __fill8
	.area _TEXT

; Internal helper: fill C bytes at HL with A; return original HL.
; Clobbers AF, BC, DE. Preserves IX, IY.
__fill8::
    push af
    ld b, #0
    ld a, c
    or a
    jr z, .empty
    pop af
    push hl
    ld (hl), a
    dec bc
    ld a, b
    or c
    jr z, .done
    push hl
    pop de
    inc de
    ldir
.done:
    pop hl
    ret
.empty:
    pop af
    ret
