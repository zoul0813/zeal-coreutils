	.module address_bc
	.optsdcc -mz80
    .globl __address_bc
	.area _TEXT

; Internal helper: B=y, C=x -> HL=SCR_TEXT+y*80+x.
; Clobbers AF, DE, HL. Preserves BC, IX, IY.
__address_bc::
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
    ret nc
    inc h
    ret
