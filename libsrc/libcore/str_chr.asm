	.module str_chr
	.optsdcc -mz80
    .globl _str_chr
	.area _TEXT

;------------------------------------------------------------------------------
; _str_chr - Find the first occurrence of a character in a string
;
; C prototype:
;   char* str_chr(const char* str, char c);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: string pointer (str)
;   - stack: character to find (c)
;
; Returns:
;   - DE: pointer to the matching character, or NULL
;
; Notes:
;   Preserves existing behavior: searching for the NUL terminator returns NULL.
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_chr::
    pop iy          ; preserve return address
    pop bc          ; C = character; B = caller stack byte
    dec sp          ; consume only the one-byte character argument
    push iy

    ld a, h         ; NULL string?
    or l
    jr z, .not_found

.loop:
    ld a, (hl)
    or a
    jr z, .not_found
    cp c
    jr z, .found
    inc hl
    jr .loop

.found:
    ex de, hl
    ret

.not_found:
    ld de, #0
    ret
