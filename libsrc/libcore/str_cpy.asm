	.module str_cpy
	.optsdcc -mz80
    .globl _str_cpy
    .globl _str_cpyn
	.area _TEXT
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
loop:
  ld a, (hl)
  ldi
  jp po, done
  or a
  jr nz, loop
done:
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