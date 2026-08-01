	.module str_cmp
	.optsdcc -mz80
    .globl _str_cmp
    .globl _str_cmpn
	.area _TEXT

;------------------------------------------------------------------------------
; _str_cmp - Compare two NUL-terminated strings
; _str_cmpn - Compare at most n bytes of two NUL-terminated strings
;
; C prototypes:
;   int16_t str_cmp(const void* src1, const void* src2);
;   int16_t str_cmpn(const void* src1, const void* src2, uint16_t n);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: first string pointer (src1)
;   - DE: second string pointer (src2)
;   - stack: maximum bytes to compare (n), for str_cmpn only
;
; Returns:
;   - DE: zero when equal, otherwise first differing unsigned src1 byte minus
;         src2 byte, in the range -255..255
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_cmp::
    ld bc, #0xffff
    jp _str_cmpn_bc_ready

_str_cmpn::
    pop iy          ; preserve return address
    pop bc          ; BC = n; remove stacked argument
    push iy

_str_cmpn_bc_ready:
    ld a, b         ; n == 0?
    or c
    jr z, .equal

    ex de, hl       ; DE = src1, HL = src2

.loop:
    ld a, (de)
    sub a, (hl)     ; unsigned src1 byte - unsigned src2 byte
    jr nz, .different

    ld a, (de)      ; equal NUL bytes terminate both strings
    or a
    jr z, .equal

    inc de
    inc hl
    dec bc
    ld a, b
    or c
    jr nz, .loop

.equal:
    ld de, #0
    ret

.different:
    ld e, a         ; low byte of src1 - src2
    ld d, #0
    ret nc          ; no borrow: result is 1..255
    dec d           ; borrow: sign-extend result to -255..-1
    ret

; Unlike the previous C implementation, the subtraction is not truncated to
; signed char. This gives strcmp-compatible negative/zero/positive ordering
; for all unsigned byte values.
