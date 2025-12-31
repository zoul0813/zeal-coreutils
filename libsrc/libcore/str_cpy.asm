	.module str_cpy
	.optsdcc -mz80
    .globl _str_cpy
    .globl _str_cpyn
	.area _TEXT
;------------------------------------------------------------------------------
; _str_cpy - Copy a null-terminated string to a destination buffer
; _str_cpyn - Copy up to n bytes of a null-terminated string to a destination buffer
;
; C prototypes:
;   void* str_cpy(void* dst, const void* src);
;   void* str_cpyn(void* dst, const void* src, uint16_t n);
;
; Parameters (SDCC calling convention, Z80):
;   str_cpy:
;     - HL: destination pointer (dst)
;     - DE: source pointer (src)
;   str_cpyn:
;     - HL: destination pointer (dst)
;     - DE: source pointer (src)
;     - BC: maximum number of bytes to copy (n)
;
; Returns:
;   - DE: destination pointer (dst)
;
; Description:
;   str_cpy copies a null-terminated string from DE (src) to HL (dst), including
;   the null terminator, by calling str_cpyn with n = 0xFFFF.
;
;   str_cpyn copies up to n bytes from DE (src) to HL (dst), stopping early if a
;   null terminator is encountered in the source. The destination is always
;   null-terminated if the source is shorter than n. Returns the original
;   destination pointer in HL.
;
; Registers affected:
;   - AF, BC, DE, HL, IY
;
;------------------------------------------------------------------------------
_str_cpy::
    ld bc, #0xffff
    jp _str_cpyn_bc_ready

_str_cpyn::
  pop iy
  pop bc
  push iy
_str_cpyn_bc_ready:
  ex de, hl
  ld a, b       ; size=0
  or c
  ret z
  push de
.loop:
  ld a, (hl)
  ldi
  jp po, .done
  or a
  jr nz, .loop
.done:
  pop de
  ret


;  void* str_cpyn(void* dst, const void* src, uint16_t n) {
;      uint8_t* d = dst;
;      const uint8_t* s = src;
;      uint16_t i = 0;
;
;      while(i < n && *s != 0x00) {
;          *d++ = *s++;
;          i++;
;      }
;      if (i < n) *d = 0x00;
;      return dst;
;  }
;
;  void* str_cpy(void* dst, const void* src) {
;      return str_cpyn(dst, src, UINT16_MAX);
;  }