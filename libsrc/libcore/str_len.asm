	.module str_len
	.optsdcc -mz80
    .globl _str_len
	.area _TEXT

;------------------------------------------------------------------------------
; _str_len - Calculate the length of a null-terminated string
;
; C prototype:
;   uint16_t str_len(const char* str);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: pointer to the null-terminated string (str)
;
; Returns:
;   - DE: length of the string (number of characters before the null terminator)
;
; Description:
;   Iterates through the string pointed to by HL, counting characters until
;   the null terminator (0x00) is found. Returns the length in DE.
;
; Registers affected:
;   - HL, DE, AF
;
;------------------------------------------------------------------------------
_str_len::
    ld      de, #0          ; length = 0

.loop:
    ld      a, (hl)         ; read *str
    or      a
    ret     z               ; if zero, done (DE = length)

    inc     hl              ; str++
    inc     de              ; length++
    jr      .loop


; uint16_t str_len(const char* str)
; {
;     uint16_t length = 0;
;     while (str[length]) length++;
;     return length;
; }