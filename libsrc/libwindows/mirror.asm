	.module mirror
	.optsdcc -mz80
    .globl _MIRROR_TEXT
    .globl _MIRROR_COLOR
	.area _DATA

SCREEN_SIZE = 0x0C80

_MIRROR_TEXT::
    .ds SCREEN_SIZE
_MIRROR_COLOR::
    .ds SCREEN_SIZE
