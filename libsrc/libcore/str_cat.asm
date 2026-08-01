	.module str_cat
	.optsdcc -mz80
    .globl _str_cat
    .globl _str_catn
	.area _TEXT

;------------------------------------------------------------------------------
; _str_cat - Append a NUL-terminated string
; _str_catn - Append up to n bytes of a NUL-terminated string
;
; C prototypes:
;   void* str_cat(void* dst, const void* src);
;   void* str_catn(void* dst, const void* src, uint16_t n);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: destination pointer (dst)
;   - DE: source pointer (src)
;   - stack: maximum bytes to append (n), for str_catn only
;
; Returns:
;   - DE: original destination pointer
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_cat::
    ld bc, #0xffff
    jp _str_catn_bc_ready

_str_catn::
    pop iy          ; preserve return address
    pop bc          ; BC = n; remove stacked argument
    push iy

_str_catn_bc_ready:
    push hl         ; preserve original destination for return

.find_end:
    ld a, (hl)
    or a
    jr z, .copy
    inc hl
    jr .find_end

.copy:
    ld a, b         ; n == 0?
    or c
    jr z, .terminate

    ld a, (de)
    or a
    jr z, .terminate

    ld (hl), a
    inc hl
    inc de
    dec bc
    jr .copy

.terminate:
    ld (hl), #0
    pop de          ; return original destination
    ret

; void* str_catn(void* dst, const void* src, uint16_t n)
; {
;     uint8_t* d = dst;
;     const uint8_t* s = src;
;     while (*d != 0) d++;
;     while (n != 0 && *s != 0) {
;         *d++ = *s++;
;         n--;
;     }
;     *d = 0;
;     return dst;
; }
;
; void* str_cat(void* dst, const void* src)
; {
;     return str_catn(dst, src, UINT16_MAX);
; }
