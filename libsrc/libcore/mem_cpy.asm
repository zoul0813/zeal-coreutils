	.module mem_cpy
	.optsdcc -mz80
    .globl _mem_cpy
	.area _TEXT
;------------------------------------------------------------------------------
; _mem_cpy - Copy a block of memory from source to destination
;
; C prototype:
;   void* mem_cpy(void* dst, const void* src, size_t size);
;
; Parameters (SDCC calling convention, Z80):
;   - HL: destination pointer (dst)
;   - DE: source pointer (src)
;   - BC: number of bytes to copy (size)
;
; Returns:
;   - DE: destination pointer (dst)
;
; Description:
;   Copies 'size' bytes from the memory area pointed to by 'src' (DE)
;   to the memory area pointed to by 'dst' (HL). If 'size' is zero,
;   the function returns immediately. The function preserves the return
;   address using IY and returns the destination pointer in HL.
;
;   This implementation uses the Z80 LDIR instruction for efficient
;   block memory copying.
;
; Registers affected:
;   - HL, DE, BC, AF, IY
;
;------------------------------------------------------------------------------
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