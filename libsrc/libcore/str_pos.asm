	.module str_pos
	.optsdcc -mz80
    .globl _str_pos
	.area _TEXT

;------------------------------------------------------------------------------
; _str_pos - Return the position of a character in a string
;
; C prototype:
;   int16_t str_pos(const char* str, char c);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: string pointer (str)
;   - stack: character to find (c)
;
; Returns:
;   - DE: zero-based position, or -1 when not found
;
; Notes:
;   Preserves existing behavior: searching for the NUL terminator returns -1.
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_pos::
    pop iy          ; preserve return address
    pop bc          ; C = character; B = caller stack byte
    dec sp          ; consume only the one-byte character argument
    push iy

    ld a, h         ; NULL string?
    or l
    jr z, .not_found

    ld de, #0       ; position = 0
.loop:
    ld a, (hl)
    or a
    jr z, .not_found
    cp c
    ret z
    inc hl
    inc de
    jr .loop

.not_found:
    ld de, #0xffff
    ret
