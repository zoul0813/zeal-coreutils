	.module window_save
	.optsdcc -mz80
    .globl _window_save
    .globl _MIRROR_TEXT
    .globl _MIRROR_COLOR
    .globl _text_map_vram
    .globl _text_demap_vram
	.area _TEXT

SCREEN_SIZE = 0x0C80
SCR_TEXT    = 0x0000
SCR_COLOR   = 0x1000

; Copy both 80x40 VRAM planes into their mirrors.
_window_save::
    call _text_map_vram

    ld hl, #SCR_TEXT
    ld de, #_MIRROR_TEXT
    ld bc, #SCREEN_SIZE
    ldir

    ld hl, #SCR_COLOR
    ld de, #_MIRROR_COLOR
    ld bc, #SCREEN_SIZE
    ldir

    jp _text_demap_vram
