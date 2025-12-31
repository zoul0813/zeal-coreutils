	.module min
	.optsdcc -mz80
    .globl _min
	.area _TEXT
;------------------------------------------------------------------------------
; _min - Return the minimum of two 16-bit unsigned integers
;
; C prototype:
;   uint16_t min(uint16_t a, uint16_t b);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: first  argument (a)
;   - DE: second argument (b)
;
; Returns:
;   - DE: the smaller of a and b
;
; Description:
;   Compares the 16-bit values in HL and DE. If HL < DE, returns HL.
;   Otherwise, returns DE. The result is left in DE.
;
; Registers affected:
;   - AF, HL, DE
;
;------------------------------------------------------------------------------
_min::
	ld	a, l
	sub	a, e
	ld	a, h
	sbc	a, d
	ret	nc
    ex de, hl
	ret


; uint16_t min(uint16_t a, uint16_t b) {
;     if(a < b) return a;
;     return b;
; }