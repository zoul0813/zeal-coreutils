	.module max
	.optsdcc -mz80
    .globl _max
	.area _TEXT
_max::
    ld	a, e
    sub	a, l
    ld	a, d
    sbc	a, h
    ret	NC
    ex de, hl
    ret

; uint16_t max(uint16_t a, uint16_t b) {
;     if(a > b) return a;
;     return b;
; }