	.module itoa_pad
	.optsdcc -mz80
    .globl _itoa_pad
    .globl _itoa
    .globl _str_len
	.area _TEXT

;------------------------------------------------------------------------------
; _itoa_pad - Convert an integer and left-pad the result to a minimum width
;
; C prototype:
;   void itoa_pad(uint16_t num, char* str, uint16_t base, char alpha,
;                 char pad, uint8_t size);
;------------------------------------------------------------------------------
_itoa_pad::
    push ix
    ld ix, #0
    add ix, sp
    push af         ; reserve one local byte at -1 (IX)

    ld c, e         ; BC = destination across itoa call
    ld b, d
    push bc
    ld a, 6 (ix)    ; alpha
    push af
    inc sp
    ld e, 4 (ix)    ; base
    ld d, 5 (ix)
    push de
    ld e, c
    ld d, b
    call _itoa
    pop bc

    push bc
    ld l, c
    ld h, b
    call _str_len   ; DE = converted length
    pop bc

    ld a, d         ; uint16_t length cannot fit uint8_t width
    or a
    jr nz, .done
    ld a, 8 (ix)    ; width
    sub a, e
    jr c, .done     ; length > width
    jr z, .done     ; length == width
    ld -1 (ix), a   ; padding count (1..255)

    push bc
    pop iy          ; IY = original destination

    ; Move converted string, including NUL, right by padding count.
    push iy
    pop hl
    add hl, de      ; HL = source NUL
    push de         ; save converted length
    ld c, -1 (ix)
    ld b, #0
    push hl
    add hl, bc
    ex de, hl       ; DE = destination NUL
    pop hl          ; HL = source NUL
    pop bc          ; BC = converted length
    inc bc          ; include NUL
    lddr

    ; Fill newly opened prefix with pad character.
    push iy
    pop hl
    ld c, -1 (ix)
    ld b, #0
    ld a, 7 (ix)
.pad_loop:
    ld (hl), a
    inc hl
    dec bc
    ld d, a         ; preserve pad value while checking count
    ld a, b
    or c
    ld a, d
    jr nz, .pad_loop

.done:
    ld sp, ix
    pop ix
    pop hl          ; return address
    pop af          ; discard base
    pop af          ; discard alpha and pad
    inc sp          ; discard size
    jp (hl)
