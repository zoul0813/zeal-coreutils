	.module vram
	.optsdcc -mz80

    .globl _text_map_vram
    .globl _text_demap_vram

	.area _DATA
_mmu_page_current:
    .ds 1

	.area _TEXT

; Map ZVB text VRAM into MMU page 0. Preserve current MMU page for demap.
_text_map_vram::
    di
    xor a                   ; banked SFR high port byte = 0
    in a, (#0xf0)
    ld (_mmu_page_current), a
    ld a, #0x40             ; VID_MEM_PHYS_ADDR_START >> 14
    out (#0xf0), a
    ret

; Restore previous MMU page and existing unconditional-EI behavior.
_text_demap_vram::
    ld a, (_mmu_page_current)
    out (#0xf0), a
    ei
    ret
