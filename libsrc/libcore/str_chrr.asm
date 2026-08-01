	.module str_chrr
	.optsdcc -mz80
    .globl _str_chrr
	.area _TEXT

;------------------------------------------------------------------------------
; _str_chrr - Find the last occurrence of a character in a string
;
; C prototype:
;   char* str_chrr(const char* str, char c);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: string pointer (str)
;   - stack: character to find (c)
;
; Returns:
;   - DE: pointer to the last match, or NULL
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_chrr::
    pop iy          ; preserve return address
    pop bc          ; C = character; B = caller stack byte
    dec sp          ; consume only the one-byte character argument
    push iy

    ld de, #0       ; last = NULL; also NULL return for NULL string
    ld a, h
    or l
    ret z

.loop:
    ld a, (hl)
    or a
    jr z, .done
    cp c
    jr nz, .next
    push hl         ; last = current string position
    pop de
.next:
    inc hl
    jr .loop

.done:
    ld a, c         ; searching for NUL returns terminator pointer
    or a
    ret nz
    ex de, hl
    ret
