	.module mem_cmp
	.optsdcc -mz80
    .globl _mem_cmp
	.area _TEXT

;------------------------------------------------------------------------------
; _mem_cmp - Compare two blocks of memory
;
; C prototype:
;   int mem_cmp(const void* p1, const void* p2, size_t size);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: first memory pointer (p1)
;   - DE: second memory pointer (p2)
;   - stack: number of bytes to compare (size)
;
; Returns:
;   - DE: zero when equal, otherwise first differing p1 byte minus p2 byte
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_mem_cmp::
    pop iy          ; preserve return address
    pop bc          ; BC = size; remove stacked argument
    push iy

    ld a, b         ; size == 0?
    or c
    jr z, .equal

    ex de, hl       ; HL = p2, DE = p1 (both support required byte loads)

.loop:
    ld a, (de)      ; x = *p1
    sub a, (hl)     ; x - *p2
    jr nz, .different

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
    ld e, a         ; low byte of x - y
    ld d, #0
    ret nc          ; no borrow: result is 1..255
    dec d           ; borrow: sign-extend result to -255..-1
    ret

; int mem_cmp(const void* p1, const void* p2, size_t size)
; {
;     const uint8_t* a = p1;
;     const uint8_t* b = p2;
;     while (size--) {
;         uint8_t x = *a++;
;         uint8_t y = *b++;
;         if (x != y) return x - y;
;     }
;     return 0;
; }
