	.module itoa
	.optsdcc -mz80
    .globl _itoa
	.area _TEXT

;------------------------------------------------------------------------------
; _itoa - Convert an unsigned 16-bit integer to a NUL-terminated string
;
; C prototype:
;   void itoa(uint16_t num, char* str, uint16_t base, char alpha);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: number (num)
;   - DE: destination string (str)
;   - stack: base (uint16_t), then alphabetic digit origin (alpha)
;
; Supports bases 2..36. Invalid bases produce an empty string.
;------------------------------------------------------------------------------
_itoa::
    push ix
    ld ix, #0
    add ix, sp
    push de         ; -2/-1 (IX) = original destination

    ld c, 4 (ix)    ; BC = base
    ld b, 5 (ix)
    ld a, b
    or a
    jr nz, .invalid
    ld a, c
    cp #2
    jr c, .invalid
    cp #37
    jr nc, .invalid

    push de
    pop iy          ; IY = output cursor

    ld a, h         ; handle zero explicitly
    or l
    jr nz, .digits
    ld (iy), #'0
    inc iy
    jr .terminate

.digits:
    call .divmod16  ; HL = quotient, DE = remainder
    ld a, e
    cp #10
    jr c, .numeric
    add a, 6 (ix)   ; alpha + remainder - 10
    sub #10
    jr .store
.numeric:
    add a, #'0'
.store:
    ld (iy), a
    inc iy
    ld a, h
    or l
    jr nz, .digits

.terminate:
    ld (iy), #0

    ; Digits were emitted least-significant first. Reverse them in place.
    ld l, -2 (ix)
    ld h, -1 (ix)  ; HL = first digit
    push iy
    pop de
    dec de          ; DE = last digit
.reverse:
    ld a, l
    sub a, e
    ld a, h
    sbc a, d
    jr nc, .done
    ld c, (hl)
    ld a, (de)
    ld (hl), a
    ld a, c
    ld (de), a
    inc hl
    dec de
    jr .reverse

.invalid:
    ld l, -2 (ix)
    ld h, -1 (ix)
    ld (hl), #0

.done:
    ld sp, ix
    pop ix
    pop hl          ; return address
    pop af          ; discard base
    inc sp          ; discard alpha
    jp (hl)

; Divide HL by BC. Return quotient in HL and remainder in DE.
; BC must be nonzero. Uses restoring unsigned division over 16 bits.
.divmod16:
    ld de, #0
    ld a, #16
    ex af, af'      ; keep iteration count in alternate A
.divide_loop:
    add hl, hl
    rl e
    rl d

    ld a, e
    sub a, c
    ld a, d
    sbc a, b
    jr c, .no_subtract
    ld a, e
    sub a, c
    ld e, a
    ld a, d
    sbc a, b
    ld d, a
    inc l           ; shifted quotient bit is zero; set it to one
.no_subtract:
    ex af, af'
    dec a
    ret z
    ex af, af'
    jr .divide_loop
