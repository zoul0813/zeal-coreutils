	.module mem_set
	.optsdcc -mz80
    .globl _mem_set
	.area _TEXT

_mem_set::
    ld d, h       ; DE = ptr (return value)
    ld e, l

    ld hl, #2     ; HL = SP + 2 -> value
    add hl, sp
    ld a, (hl)    ; value
    ex af, af'    ; keep value in A'
    inc hl
    ld c, (hl)    ; size low
    inc hl
    ld b, (hl)    ; size high

    ld h, d       ; HL = ptr for writes
    ld l, e

    ld a, b       ; size == 0?
    or c
    jr z, .done

    ex af, af'    ; A = value
    ld (hl), a    ; seed first byte
    ex af, af'
    dec bc        ; remaining bytes
    ld a, b
    or c
    jr z, .done

    push de       ; preserve return value
    inc de        ; DE = ptr + 1
    ldir          ; copy HL -> DE for BC bytes
    pop de

.done:
    pop hl        ; return address
    pop af        ; drop value + size low
    inc sp        ; drop size high
    jp (hl)

; void* mem_set(void* ptr, uint8_t value, size_t size)
; {
;     uint8_t* p = ptr;
;     while (size--) {
;         *p++ = value;
;     }
;     return ptr;
; }
