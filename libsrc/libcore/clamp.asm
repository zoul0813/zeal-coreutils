	.module clamp
	.optsdcc -mz80
    .globl _clamp
	.area _TEXT
;------------------------------------------------------------------------------
; _clamp - Clamp a 16-bit unsigned value between a minimum and maximum
;
; C prototype:
;   uint16_t clamp(uint16_t val, uint16_t min, uint16_t max);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: value to clamp (val)
;   - DE: minimum value (min)
;   - BC: maximum value (max)
;
; Returns:
;   - DE: clamped value (min, max, or val)
;
; Description:
;   Returns min if val < min, max if val > max, otherwise returns val.
;   The result is returned in DE.
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_clamp::
    pop iy      ; store return in IY
    pop bc      ; get max
    push iy     ; put return back

    ; if (val < min) return min
    or a               ; clear carry
    sbc hl, de          ; HL = val - min
    jr c, .ret_min     ; carry => val < min

    ; restore val into HL
    add hl, de          ; HL = val again

    ; if (val > max) return max
    or a
    sbc hl, bc          ; HL = val - max
    jr c, .ret_val     ; carry => val <= max

.ret_max:               ; val > max
    ld d, b
    ld e, c
    ret

.ret_min:
    ret             ; DE already = min

.ret_val:           ; return val
    add hl, bc
    ex de, hl
    ret



; uint16_t clamp(uint16_t val, uint16_t min, uint16_t max) {
;     if (val < min) return min;
;     if (val > max) return max;
;     return val;
; }
