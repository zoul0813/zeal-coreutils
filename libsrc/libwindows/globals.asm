	.module globals
	.optsdcc -mz80
    .globl _win_None
    .globl _win_NonePtr

	.area _INITIALIZED
_win_None::
    .ds 13
_win_NonePtr::
    .ds 2

	.area _INITIALIZER
__xinit__win_None:
    .db 0, 0, 0, 0, 0, 0, 0, 0
    .dw 0
    .db 0, 0, 0
__xinit__win_NonePtr:
    .dw _win_None
