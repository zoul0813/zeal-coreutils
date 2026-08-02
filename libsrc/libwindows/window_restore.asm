	.module window_restore
	.optsdcc -mz80
    .globl _window_restore
    .globl _MIRROR_TEXT
    .globl _MIRROR_COLOR
    .globl _text_map_vram
    .globl _text_demap_vram
	.area _TEXT

SCREEN_SIZE = 0x0C80
SCR_TEXT    = 0x0000
SCR_COLOR   = 0x1000

; Restore both 80x40 VRAM planes from their mirrors.
_window_restore::
    call _text_map_vram

    ld hl, #_MIRROR_TEXT
    ld de, #SCR_TEXT
    ld bc, #SCREEN_SIZE
    ldir

    ld hl, #_MIRROR_COLOR
    ld de, #SCR_COLOR
    ld bc, #SCREEN_SIZE
    ldir

    jp _text_demap_vram
