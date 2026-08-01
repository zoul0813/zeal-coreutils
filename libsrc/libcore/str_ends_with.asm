	.module str_ends_with
	.optsdcc -mz80
    .globl _str_ends_with
    .globl _str_len
	.area _TEXT

;------------------------------------------------------------------------------
; _str_ends_with - Test whether a string ends with a suffix
;
; C prototype:
;   int16_t str_ends_with(const char* str, const char* suffix);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: string pointer (str)
;   - DE: suffix pointer (suffix)
;
; Returns:
;   - DE: 1 when str ends with suffix, otherwise 0
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_ends_with::
    ld a, h         ; NULL string?
    or l
    jr z, .false
    ld a, d         ; NULL suffix?
    or e
    jr z, .false

    push de         ; save suffix pointer
    push hl
    pop iy          ; IY = original string pointer

    call _str_len   ; DE = string length
    ld b, d
    ld c, e         ; BC = string length

    pop hl          ; HL = original suffix pointer
    push hl         ; retain suffix pointer for comparison
    call _str_len   ; DE = suffix length

    ld a, c         ; BC = string length - suffix length
    sub a, e
    ld c, a
    ld a, b
    sbc a, d
    ld b, a
    jr c, .false_pop ; suffix longer than string

    push iy
    pop hl          ; HL = string + suffix offset
    add hl, bc
    pop de          ; DE = suffix pointer

.compare:
    ld a, (de)
    or a
    jr z, .true     ; end of suffix: all bytes matched
    cp a, (hl)
    jr nz, .false
    inc de
    inc hl
    jr .compare

.false_pop:
    pop hl          ; discard retained suffix pointer
.false:
    ld de, #0
    ret

.true:
    ld de, #1
    ret
