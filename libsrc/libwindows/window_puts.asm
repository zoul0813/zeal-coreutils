	.module window_puts
	.optsdcc -mz80

    .globl _window_puts
    .globl _window_puts_color
    .globl __window_putc_color_mapped
    .globl _text_map_vram
    .globl _text_demap_vram

	.area _TEXT

WIN_FG  = 4
WIN_BG  = 5
CH_ESC  = 0x1b

;------------------------------------------------------------------------------
; uint8_t window_puts(window_t* w, const char* s)
;
; SDCC ABI:
;   HL = w
;   DE = s
;   A  = returned line count
;
; Builds default color and delegates to window_puts_color.
;------------------------------------------------------------------------------
_window_puts::
    push iy
    push hl
    pop iy
    ld a, WIN_BG (iy)
    rlca
    rlca
    rlca
    rlca
    and #0xf0
    ld c, a
    ld a, WIN_FG (iy)
    and #0x0f
    or c
    pop iy

    push af                 ; push one-byte color argument
    inc sp
    call _window_puts_color
    ret

;------------------------------------------------------------------------------
; uint8_t window_puts_color(window_t* w, const char* s, uint8_t color)
;
; SDCC ABI:
;   HL    = w
;   DE    = s
;   stack = color (one byte)
;   A     = returned line count
;
; Maps VRAM once, parses optional ESC/color/character triples, and delegates
; character rendering to _window_putc_color_mapped.
;
; IX local frame:
;   0 = default color
;   1 = accumulated lines
;   2..3 = window_t pointer
;   4..5 = current string pointer
;------------------------------------------------------------------------------
_window_puts_color::
    pop af                  ; return address
    pop bc                  ; C = color; B = adjacent caller byte
    dec sp                  ; consume only one-byte color argument
    push af

    push iy
    push ix
    push de                 ; local current string pointer
    push hl                 ; local window pointer
    push bc                 ; local color + line-count byte
    ld ix, #0
    add ix, sp
    ld 1 (ix), #0           ; lines = 0
    push hl
    pop iy                  ; IY = w

    ld l, 4 (ix)
    ld h, 5 (ix)
    ld a, (hl)
    or a
    jr z, .empty

    call _text_map_vram

.loop:
    ld l, 4 (ix)
    ld h, 5 (ix)
    ld a, (hl)
    or a
    jr z, .done
    cp #CH_ESC
    jr nz, .normal

    inc hl                  ; p + 1: encoded color
    ld a, (hl)
    or a
    jr z, .normal
    ld b, a
    inc hl                  ; p + 2: encoded character
    ld a, (hl)
    or a
    jr z, .normal
    ld c, a
    inc hl                  ; p += 3
    jr .emit

.normal:
    ld l, 4 (ix)
    ld h, 5 (ix)
    ld c, (hl)
    inc hl                  ; p++
    ld b, 0 (ix)            ; reset color after each normal character

.emit:
    ld 4 (ix), l
    ld 5 (ix), h
    push bc                 ; color:character arguments
    push iy
    pop hl                  ; HL = w
    call __window_putc_color_mapped
    add a, 1 (ix)
    ld 1 (ix), a
    jr .loop

.done:
    call _text_demap_vram
    ld a, 1 (ix)
    jr .return

.empty:
    xor a

.return:
    pop bc                  ; discard local color/lines
    pop hl                  ; discard local w
    pop de                  ; discard local p
    pop ix
    pop iy
    ret

; C reference behavior: NUL terminates output; a complete ESC/color/character
; triple emits encoded character/color; incomplete triples emit ESC normally.
