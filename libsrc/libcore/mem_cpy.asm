	.module mem_cpy
	.optsdcc -mz80
    .globl _mem_cpy
	.area _TEXT
_mem_cpy::
    pop iy     ; store return in IY
    pop bc     ; get the size
    push iy    ; put return back
    ex de, hl  ; swap src/dst
    ld a, b    ; do nothing if size=0
    or c       ;
    ret z      ;
    push de    ; return value (dst)
    ldir       ; copy
    pop de     ; return value (dst)
    ret        ; adios!

; void* mem_cpy(void* dst, const void* src, size_t size)
; {
;     uint8_t* d       = dst;
;     const uint8_t* s = src;
;     while (size--) *d++ = *s++;
;     return dst;
; }